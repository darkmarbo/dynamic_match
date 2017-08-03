


OUT=wer_tools
src=src
INCLUDEDIR=-I $(src)
LIBDIR=-lm
CPPFLAGS = -g -finline-functions -Wall -Winline -pipe
name=wer
name2=match
GCC=g++

obj = \
	common.o \
	UTF8Util.o \
	werC.o 


all: clean $(name) $(name2)
	rm -rf $(OUT) && mkdir -p $(OUT)
	mv $(name) $(name2) $(OUT)
	rm -rf *.o


$(name) : $(obj) main_wer.o
	$(GCC) -o $(name) $(obj) main_wer.o $(INCLUDEDIR) $(LIBDIR)
$(name2) : $(obj) main_match.o
	$(GCC) -o $(name2) $(obj) main_match.o $(INCLUDEDIR) $(LIBDIR)

clean:
	rm -rf $(OUT)
	rm -rf *.o

%.o : $(src)/%.cpp
	$(GCC) $(CPPFLAGS) -c $< -o $@  $(INCLUDEDIR) $(LIBDIR)



