C=gcc

CFLAGS=-c

all: guesanumber

guesanumber: main.o guesanumber.o
	${C} $^ -o $@

main.o: main.c
	${C} ${CFLAGS} $^ -o $@

guesanumber.o: guesanumber.c guesanumber.h
	${C} ${CFLAGS} $< -o $@

clean:
	rm -rfv *.o guesanumber
