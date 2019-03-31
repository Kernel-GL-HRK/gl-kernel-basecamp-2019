all: function.o main.o
	gcc fun.o homework01.o -o homework
function.o: fun.c fun.h
	gcc -c fun.c 
main.o: homework01.c fun.h
	gcc -c homework01.c

