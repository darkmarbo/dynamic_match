import ctypes
import sys
import string
so = ctypes.CDLL("./libsplitwav.so")

#nRet = so.splitWave(wavfile,segfile_file,spos,epos)
print "python split.py wave ............"
st=int((string.atof(sys.argv[3]))*100)*10
ed=int((string.atof(sys.argv[4]))*100)*10
print "st:%f\ted:%f"%(st,ed)
nRet = so.splitWave(sys.argv[1],sys.argv[2],st,ed)
print nRet
