C=gcc
NAME_PROG=homework
compile: all
	rm *.o
all: function.o main.o
	$(C) fun.o homework01.o -o $(NAME_PROG)
main.o: homework01.c fun.h
	$(C) -c homework01.c
function.o: fun.c fun.h
	$(C) -c fun.c

