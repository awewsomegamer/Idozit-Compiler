LIBRARY_C := $(wildcard *.c)
LIBRARY_O := $(LIBRARY_C:.c=.o)
FLAGS ?=

all: $(LIBRARY_O)
%.o : %.c
	gcc -c $< -o ../bin/library/$@ -I ./include -fPIC -masm=intel $(FLAGS)