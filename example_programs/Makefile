all: area

FLAGS := -L../bin/library -lcidozit -I../src/include

area:
	gcc area_calculator.c $(FLAGS) -o area_calc.out
	gcc the_visualizer.c -lSDL2 $(FLAGS) -o the_visualizer.out
	gcc graphing_calculator.c -lSDL2 $(FLAGS) -o graphing_calculator.out