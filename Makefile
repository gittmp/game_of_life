all: gameoflife.o libgol.so
	export LD_LIBRARY_PATH=`pwd`:$LD_LIBRARY_PATH
	gcc -Wall -Wextra -pedantic -std=c11 -o gameoflife gameoflife.o -lgol -L.

libgol.so: gol.o gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -shared -o libgol.so gol.o

gol.o: gol.c gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -c -fPIC gol.c

gameoflife.o: gameoflife.c gol.h
	gcc -Wall -Wextra -pedantic -std=c11 -c -fPIC gameoflife.c

clean:
	rm -rf gameoflife.o gol.o libgol.so gameoflife