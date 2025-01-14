.PHONY: all clean
all: montador ligador

montador:
	g++ montador.cpp -o montador

ligador:
	g++ ligador.cpp -o ligador

clean:
	rm -rf montador ligador
