#!/bin/sh

###     ./run_wer.sh  result-ok.txt result-res.txt  result-ok.txt.wer

### sed -i  's/\/n:noise[0-9]//g'   result-ok.txt 

if(($#<2));then
    echo "usage: $0 file_label  file_asr "
    exit 0;
fi

file_lab=$1
file_asr=$2
file_wer=${file_lab}.wer

dos2unix ${file_lab}  ${file_asr}

./wer_tools/wer  ${file_lab}  ${file_asr}  ${file_wer} 

#./wer_tools/match  test/duian.txt  test/duian.asr test/duian.out > match.log 


