CFLAGS = -Wall -Werror -ansi -pedantic

all:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/shell.cpp -o bin/rshell

rshell:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/shell.cpp -o bin/rshell
