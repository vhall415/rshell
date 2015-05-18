CFLAGS = -Wall -Werror -ansi -pedantic

all:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/shell.cpp -o bin/rshell
	g++ $(CFLAGS) src/cp.cpp -o bin/cp
	g++ $(CFLAGS) src/mv.cpp -o bin/mv
	g++ $(CFLAGS) src/rm.cpp -o bin/rm

rshell:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/shell.cpp -o bin/rshell

cp:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/cp.cpp -o bin/cp
mv:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/mv.cpp -o bin/mv

rm:
	if [ ! -d bin ]; then mkdir bin; fi
	g++ $(CFLAGS) src/rm.cpp -o bin/rm
