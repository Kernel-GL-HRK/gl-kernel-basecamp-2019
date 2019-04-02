OBJS=dz1.o game.o
CC=gcc

all: game.o dz1.o
	$(CC) -o dz1 $^
	
game.o: game.c game.h
	$(CC) -c $<
	
dz1.o: dz1.c
	$(CC) -c $<
	
.PHONY : clean
clean:
	rm $(OBJS)
