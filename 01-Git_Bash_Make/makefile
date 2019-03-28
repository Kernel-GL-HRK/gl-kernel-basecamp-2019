cmp=g++

game: main.o guesanumber.o
	${cmp} main.o guesanumber.o -o $@
main.o:main.c
	${cmp} -c main.c
guesanumber.o:guesanumber.h guesanumber.c
	${cmp} -c guesanumber.c
clean:
	rm -r *.o
