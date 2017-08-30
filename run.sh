#!/bin/sh

#####  目录下的每个wav  对应相同目录下都有一个txt 

## nohup ./run.sh   1   dir_input    &
## nohup ./run.sh  2    dir_input    &

if(($#<2));then
    echo "usage:    $0  1/2  dir_input "
    echo "example 语音文本初始对齐:  $0  1 dir_input "
    echo "example 人工矫正重新切分:  $0  2 dir_input "
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
dir_in=$2
dir_run=run_dynamic_match

### 1. 判断输入文件是否存在 
if [ ! -d ${dir_in} ];then
    echo "ERROR: input dir=${dir_in} not exist!"
    rm -rf lock;
    exit 0;
fi

###  目录下所有的wav 
find ${dir_in}  -iname "*.wav" > wav.list

if [ x"$flag" == x"1" ];then

	### 检测是否每个wav 都有对应的 txt 
	for wav in `cat wav.list` ;
	do
		[ "x" = "x${wav}" ] && echo "wav=${wav} 为空！" && continue	
	
		tmp=${wav%.wav}
		txt=${tmp}.txt
	
		if [ ! -f ${txt} ];then
			echo "ERROR:wav=${wav} 没有对应的txt=${txt}"	
			continue;
		fi
	
		name_wav=`basename ${wav}`
		name_txt=`basename ${txt}`
		echo "LOG:开始处理:" $name_txt   $name_wav
	
		###### 拷贝单个wav和txt 到执行目录 运行 获取结果
		rm -rf ${dir_run}/${name_wav}  ${dir_run}/${name_txt}
		cp -r ${wav}  ${txt}  ${dir_run}
	
		cd ${dir_run}   
			./run_remote.sh  1  ${name_txt}  ${name_wav}  
			num_match=`wc -l ${name_wav}_seg.match | awk '{print $1}'`
			if [ ${num_match} -gt 2 ];then
			   mv ${name_wav}_seg.match  ../ 
		           rm -rf ${name_wav} ${name_txt} ${name_wav}_reseg  ${name_wav}_seg*  
			fi 
		cd -
	
		#### 将match结果保存到对应txt的目录下
		rm -rf  ${wav}_seg.match  &&  mv  ${name_wav}_seg.match  ${wav}_seg.match
		echo "处理 wav=${name_wav} 完毕!"
		 
	done

elif [ x"$flag" == x"2" ];then

	### 检测是否每个wav 都有对应的 _seg.match
	for wav in `cat wav.list` ;
	do
		[ "x" = "x${wav}" ] && echo "wav=${wav} 为空！" && continue	
	
		tmp=${wav%.wav}
		dir_wav=`dirname ${wav}`
		match=${wav}_seg.match
	
		if [ ! -f ${match} ];then
			echo "ERROR:wav=${wav} 没有对应的_seg.match=${match}"	
			continue;
		fi
	
		name_wav=`basename ${wav}`
		name_match=`basename ${match}`
		echo "LOG:开始处理:" $name_match   $name_wav
	
		###### 拷贝单个wav和match 到执行目录 运行 获取结果
		rm -rf ${dir_run}/${name_wav}  ${dir_run}/${name_match}
		cp -r ${wav}  ${match}  ${dir_run}
	
		cd ${dir_run}   
    			######  将原始语音 wav  按照match的时间点  切分到目录 dir_out 中 
			name_reseg=${name_wav}_reseg
			./run_remote.sh  2  ${name_match}  ${name_wav}  
			num_match=`ls -1 ${name_reseg} | wc -l | awk '{print $1}'`
			if [ ${num_match} -gt 2 ];then
				mv ${name_reseg}  ../ 
				rm -rf ${name_wav} ${name_match}
			fi 
		cd -
	
		#### 将match结果保存到对应txt的目录下
		rm -rf ${dir_wav}/${name_reseg} && mv  ${name_reseg}  ${dir_wav}
		echo "处理 wav=${name_wav} 完毕!"
		 
	done

    
    
fi

rm -rf lock && exit 0;


