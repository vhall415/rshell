CFLAGS = -Wall -Werror -ansi -pedantic

all:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/shell.cpp -o bin/rshell
	g++ $(CFLAGS) src/mv.cpp -o bin/mv
	g++ $(CFLAGS) src/rm.cpp -o bin/rm

rshell:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/shell.cpp -o bin/rshell


mv:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/mv.cpp -o bin/mv

rm:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/rm.cpp -o bin/rm
