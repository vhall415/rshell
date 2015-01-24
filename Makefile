VPATH = src

CFLAGS = -Wall -Werror -ansi -pedantic

all: rshell
	mkdir bin/rshell

rshell:
	g++ -std=c++11 src/shell.cpp

clean:
	rm -rf bin
