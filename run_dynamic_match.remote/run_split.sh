#!/bin/sh 

##   ./run.sh  00news_decl.wav  00news_decl.txt  00news_decl.wav_seg 
if(($# < 3))
then
	echo "usage $0 wav_file time_file  out_dir "
    echo "文本格式: name+tab+time+tab+time"
	exit 0
fi

wav=$1
name=${wav%.wav}
txt=$2
dir=$3

rm -rf $dir && mkdir -p $dir

num_tab=`awk -F"\t" '{if(NF==3){print $0}}' ${txt} |wc -l|awk '{print $1}'`
num_line=`awk -F"\t" '{if(length($0)>10){print $0}}' ${txt} | wc -l |awk '{print $1}'`
echo $num_line $num_tab
if [ $num_tab -lt ${num_line} ];then
    echo "txt format error!"
    exit 0
fi

cat $txt | while read time 
do
	echo "time=$time"
    time=`echo $time|sed 's/\t/ /g'`
	name=`echo ${time} |awk -F" " '{print $1}'`
	st=`echo ${time} |awk -F" " '{print $2}'`
	ed=`echo ${time} |awk -F" " '{print $3}'`
	echo "name=${name}  st=${st}    ed=$ed"
    ##echo " python split.py $wav  $dir/${name}.wav $st $ed "
    python split.py $wav  $dir/${name}.wav $st $ed
done

echo "切分wav=${wav} 完成!"

#if(($# < 2))
#then
#	echo "usage $0 wav_dir wav_out"
#    echo "同名的txt文本放到lab目录中 "
#	exit 0
#fi
#
#wav_dir=$1
#wav_out=$2
#lab_dir=lab
#rm -rf $wav_out  && mkdir -p $wav_out
#
#ls -1 $wav_dir|while read line
#do
#	echo "line is $line"
#	name=${line%.wav}
#	txt="${name}.txt"
#	cat $lab_dir/$txt|while read time
#	do
#		echo "time is $time"
#		time_tmp=`echo $time|sed 's/\t/ /g'`
#		time_tmp=${time_tmp# *}
#		time_tmp=${time_tmp%* }
#		st=${time_tmp% *}
#		ed=${time_tmp#* }
#		#echo "st:${st}ed:$ed"
#		#echo "python split.py $wav_dir/$line $wav_out/${name}_${st}_${ed}.wav $st $ed"
#		python split.py $wav_dir/$line $wav_out/${name}_${st}_${ed}.wav $st $ed
#	done
#done




