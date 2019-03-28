C=gcc
СFLAGS=-c -Wall

all: game

game: main.o guess_game.o
	$(C) main.o guess_game.o -o game

main.o: main.c
	$(C) $(CFLAGS) main.c

guess_game.o: guess_game.c guess_game.h
	$(C) $(CFLAGS) guess_game.c

clean:
	rm -rf *.o game
