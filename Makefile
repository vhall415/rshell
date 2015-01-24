VPATH = src

CFLAGS = -Wall -Werror -ansi -pedantic

all: rshell

rshell:
	g++ -std=c++11 src/shell.cpp
