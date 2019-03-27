C = gcc
CFLAGS = -Wall -c
OBJS = guess.o main.o

game: ${OBJS}
	${C} $? -o $@
guess.o: guess.c
	${C} ${CFLAGS} $? -o $@
main.o: main.c
	${C} ${CFLAGS} $? -o $@
clean:
	rm *.o
uninstall:
	rm game
