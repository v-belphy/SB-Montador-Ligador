.PHONY: all
all: montador ligador

montador:
	g++ montador.cpp -o montador

ligador:
	g++ ligador.cpp -o ligador
