


OUT=wer_tools
src=src
INCLUDEDIR=-I $(src)
LIBDIR=-lm
CPPFLAGS = -g -finline-functions -Wall -Winline -pipe
name=wer
GCC=g++

obj = \
	werC.o \
	main.o

all: clean $(name)
	rm -rf $(OUT) && mkdir -p $(OUT)
	mv $(name) $(OUT)
	rm -rf *.o


$(name) : $(obj)
	$(GCC) -o $@ $^ $(INCLUDEDIR) $(LIBDIR)

clean:
	rm -rf $(OUT)
	rm -rf *.o

%.o : $(src)/%.cpp
	$(GCC) $(CPPFLAGS) -c $< -o $@  $(INCLUDEDIR) $(LIBDIR)



