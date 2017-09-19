
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
		vec_st=st.split(":");	
		if len(vec_st)>1:
			num = int(vec_st[0]) * 60 + float(vec_st[1]);
			st="%.4f"%(num);

		vec_end=end.split(":");	
		if len(vec_end)>1:
			num = int(vec_end[0]) * 60 + float(vec_end[1]);
			end="%.4f"%(num);

		fp_out.write("%s\t%s\n"%(st, end));

fp_in.close();
fp_out.close();



