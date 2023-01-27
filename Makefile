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