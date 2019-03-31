OBJS=dz1.o game.o

all: game.o dz1.o
	gcc -o dz1 $^
	
game.o: game.c game.h
	gcc -c $<
	
dz1.o: dz1.c
	gcc -c $<
	
.PHONY : clean
clean:
	rm $(OBJS)
