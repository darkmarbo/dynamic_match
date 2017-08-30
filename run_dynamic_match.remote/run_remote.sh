#!/bin/sh

## nohup ./run_vad.sh   1   test/duian.txt   test/duianyoushenme_sox.wav    &
## nohup ./run_vad.sh  2  duianyoushenme_sox.wav_seg.match  test/duianyoushenme_sox.wav    &

if(($#<3));then
    echo "usage:    $0  1/2 input_text   input_wav "
    echo "example 语音文本初始对齐:  $0  1 input_text   input_wav "
    echo "example 人工矫正重新切分:  $0  2 input_text   input_wav "
    exit 0;
fi

while(true)
do
    if [ -f lock ];then
        echo "等待...  date:`date +%H:%M:%S`"
        sleep 2;
    else
        touch lock;
        echo "开始..."
        break;
    fi
done

dir_pwd=` pwd `
flag=$1
file_lab=$2
file_wav=$3

### 获取wav的名字 
name_wav=`basename ${file_wav}`


host=root@10.10.10.158
dir_asr=/home/szm/cd/kaldi-master/test
res_asr=ttt.log

dir_seg=${name_wav}_seg
dir_reseg=${name_wav}_reseg
dir_seg_16k=${dir_seg}_16k
file_res=${dir_seg}.res
file_match=${dir_seg}.match


### 1. 判断输入文件是否存在 
if [ ! -f ${file_lab} ];then
    echo "ERROR: input text=${file_lab} not exist!"
    rm -rf lock;
    exit 0;
fi
if [ ! -f ${file_wav} ];then
    echo "ERROR: wav=${file_wav} not exist!"
    rm -rf lock;
    exit 0;
fi

dos2unix  ${file_lab}
sox ${file_wav}  temp.wav  && mv temp.wav ${file_wav} 

### 2. 检测文本是否是  id tab content 格式
num_tab=`awk -F"\t" ' BEGIN{num=0} {if(NF>1){num=num+1}} END{print num} ' ${file_lab}`
num_all=`wc -l ${file_lab} | awk '{print $1}'`
if(( ${num_tab} < ${num_all}/2 ));then
    echo "num_tab=${num_tab}   num_all=${num_all}"
    echo "将原始文本 ${file_lab} 修改成 id+tab+cont 格式"
    awk -v ddd="${name_wav}"  '{print ddd"_"NR"\t"$0 }' ${file_lab} > temp.txt  && mv temp.txt   ${file_lab}

fi

### rm -rf lock && exit 0;


#iconv -f gbk -t utf-8 audioSegment/$file  >  ttt/$file


if [ x"$flag" == x"1" ];then
    	####   1. 初始切分 
    	####sox duianyoushenme.wav -t wav duianyoushenme_sox.wav
    	
    	echo "开始初始切分wav语音..."
    	rm -rf ${dir_seg}  && mkdir -p ${dir_seg}
    	./vad   1   ${file_wav}  ${dir_seg}
    	
    	#     2.  sox 转换成 16k 
    	rm -rf ${dir_seg_16k}   &&   mkdir -p   ${dir_seg_16k}
    	ls -1 ${dir_seg} | while read line
    	do
    	    sox ${dir_seg}/${line}  -r 16000 ${dir_seg_16k}/${line}
    	
    	done
    
    
    	###    3.   asr 识别 
	###   上传目录到目标服务器 
	dd=`date +%H%M%S`
	data_wav_remote=${dir_seg_16k}_${dd}
	scp  -r  ${dir_seg_16k}  ${host}:${dir_asr}/${data_wav_remote} 

	ssh ${host}  "
		cd ${dir_asr};
    	    	./run_asr.sh  ${data_wav_remote} 
		
	" 

    	scp -r  ${host}:${dir_asr}/${res_asr}   ${dir_pwd}
    	if [ ! -f ${res_asr} ];then
    	    echo "ERROR: wav=${res_asr} not exist!"
    	    rm -rf lock;
    	    exit 0;
    	fi
    	
    	sed -i "s#${dir_pwd}/##g"   ${res_asr}
    	
    	###    4.   动态对齐  
    	sort -n   ${res_asr}  >   ${file_res} 
    	./match ${file_lab}  ${file_res}  ${file_match}
    
elif [ x"$flag" == x"2" ];then
    	######  6.   矫正结果time  重新切分  
    	######       将原始语音 wav  按照duian.out的时间点  切分到目录 dir_out 中 
    	rm -rf ${dir_reseg}  && mkdir -p ${dir_reseg}
    	./vad   2   ${file_wav}  ${file_lab}  ${dir_reseg}
    
fi


####   5.   人工矫正  


rm -rf lock



