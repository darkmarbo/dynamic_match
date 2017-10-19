#!/bin/sh


in_txt=CONTENT_all.txt
out=CONTENT_txt

######  1.  excel 转 txt 
rm -rf $out  && mkdir -p $out
ls -1 CONTENT | while read line
do
    name=${line%.xlsx}
    txt=${name}.txt
    echo $line $name  $txt
    python  read_excel.py  CONTENT/$line  $out/$txt  $name 

done

cat $out/*.txt  > $in_txt
dos2unix   $in_txt

awk -F"\t" '{{print $1"\t"$2}}'  $in_txt > txt.ques 
awk -F"\t" '{{print $1"\t"$3}}'  $in_txt > txt.Cortana
awk -F"\t" '{{print $1"\t"$4}}'  $in_txt > txt.lingxi
awk -F"\t" '{{print $1"\t"$5}}'  $in_txt > txt.GOOGLE
awk -F"\t" '{{print $1"\t"$6}}'  $in_txt > txt.baidu
awk -F"\t" '{{print $1"\t"$7}}'  $in_txt > txt.siri

./wer_tools/wer  txt.ques  txt.Cortana  wer.Cortana 
./wer_tools/wer  txt.ques  txt.lingxi  wer.lingxi 
./wer_tools/wer  txt.ques  txt.GOOGLE  wer.GOOGLE 
./wer_tools/wer  txt.ques  txt.baidu  wer.baidu 
./wer_tools/wer  txt.ques  txt.siri  wer.siri 


#### 2.  统计所有的 句子开头 del error
#
#iconv -f gbk -t utf-8 1.txt  >  1.txt.utf8
#dos2unix  1.txt.utf8
#
#awk -F"\t" '{if(NR>1){print NR"\t"$1}}'  1.txt.utf8 > txt.ques 
#awk -F"\t" '{if(NR>1){print NR"\t"$2}}'  1.txt.utf8 > txt.Cortana
#awk -F"\t" '{if(NR>1){print NR"\t"$3}}'  1.txt.utf8 > txt.lingxi
#awk -F"\t" '{if(NR>1){print NR"\t"$4}}'  1.txt.utf8 > txt.GOOGLE
#awk -F"\t" '{if(NR>1){print NR"\t"$5}}'  1.txt.utf8 > txt.baidu
#awk -F"\t" '{if(NR>1){print NR"\t"$6}}'  1.txt.utf8 > txt.siri
#
#
#./wer_tools/wer  txt.ques  txt.Cortana  wer.Cortana 
#./wer_tools/wer  txt.ques  txt.lingxi  wer.lingxi 
#./wer_tools/wer  txt.ques  txt.GOOGLE  wer.GOOGLE 
#./wer_tools/wer  txt.ques  txt.baidu  wer.baidu 
#./wer_tools/wer  txt.ques  txt.siri  wer.siri 





####     ./run_wer.sh  result-ok.txt result-res.txt  result-ok.txt.wer
#
#### sed -i  's/\/n:noise[0-9]//g'   result-ok.txt 
#
#if(($#<2));then
#    echo "usage: $0 file_label  file_asr "
#    exit 0;
#fi
#
#file_lab=$1
#file_asr=$2
#file_wer=${file_lab}.wer
#
#dos2unix ${file_lab}  ${file_asr}
#
#./wer_tools/wer  ${file_lab}  ${file_asr}  ${file_wer} 
#
##./wer_tools/match  test/duian.txt  test/duian.asr test/duian.out > match.log 


