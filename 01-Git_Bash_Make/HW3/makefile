C=g++
OBJS=main.o game.o

guess: main.o game.o
	${C} -o $@ ${OBJS}
main.o: main.c
	${C} -c main.c
game.o: game.c game.h
	${C} -c game.c
clean:
	rm -r *.o

