#coding=utf-8

import sys
import os
import xlrd
reload(sys)
sys.setdefaultencoding('utf-8')

if len(sys.argv) < 4:
    print("usage: %s in_excel out_txt spk_id "%(sys.argv[0]));
    sys.exit(0);


excelFile=sys.argv[1]
out=sys.argv[2]
id=sys.argv[3]

fp_out=open(out,"w");

#print("test");

data = xlrd.open_workbook(excelFile)

table = data.sheets()[0]

nrows = table.nrows 
ncols = table.ncols #列数

## 每行 
for i in xrange(0, nrows):
    if i == 0:
        continue;

    #某一行数据
    rowValues = table.row_values(i) 
    
    ### spk + id
    fp_out.write("%s_%s\t"%(id, rowValues[0])); 
    for j in range(1, 7):
        fp_out.write("%s\t"%(rowValues[j])); 
    #for item in rowValues:
    #    fp_out.write("%s\t"%(item)); 

    fp_out.write("\n"); 


#print("over!");


