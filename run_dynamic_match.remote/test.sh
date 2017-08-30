#!/bin/sh

####  目录 $1 下每个文件的名字  加个前缀 $2
#### ./test.sh    feibi.wav_reseg  feibi
#dir=$1
#ls  -1   $dir |while read line
#do
#    new=${2}_${line}
#    echo "$line  --->  ${new}"
#    mv ${dir}/${line}   ${dir}/${new}
#
#done


#### 文件每行 开头增加前缀 $2
awk -v ddd="${2}"  '{if(length($0)>3){print ddd"_"$0}}'  $1 > temp.txt
mv temp.txt    $1



