all: 
	make -C src/
	make -C bin/library/
	make -C test/
	make -C example_programs/

debug:
	make -C src/ FLAGS="-DDEBUG"
	make -C bin/library/
	make -C test/
	make -C example_programs/

run: all
	test/test.out

run_dbg: debug
	test/test.out

examples: all
	make -C example_programs/
	

testing:
	nasm -f elf64 -o test.nasm.o test.nasm
	gcc -c -o test.c.o test.c
	gcc -o testing.out test.c.o test.nasm.o -no-pie

	./testing.out