LIBRARY_C := $(wildcard *.c)
LIBRARY_O := $(LIBRARY_C:.c=.o)

all: $(LIBRARY_O)

%.o : %.c
	gcc -c $< -o ../bin/library/$@ -I ./include -fPIC -masm=intel