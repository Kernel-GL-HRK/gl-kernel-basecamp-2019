all: game

game: main.o guess_game.o
	gcc main.o guess_game.o -o game

main.o: main.c
	gcc -c main.c

guess_game.o: guess_game.c guess_game.h
	gcc -c guess_game.c

clean:
	rm -rf *.o game
