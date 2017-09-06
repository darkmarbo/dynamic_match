
import os
import sys
import string

if len(sys.argv)<2:
	print("usage: %s file_in file_out "%(sys.argv[0]));
	sys.exit(0);


fp_in=open(sys.argv[1], "r");
fp_out=open(sys.argv[2], "w");

for line in fp_in:
	#print("%s"%(line));
	vec=line.split("\t");
	if len(vec)<2:
		break;

	if len(vec) == 2:
		name=vec[0]
		fp_out.write("%s\t"%(name));
	else:
		st=vec[3];
		end=vec[4];
		fp_out.write("%s\t%s\n"%(st, end));

fp_in.close();
fp_out.close();



