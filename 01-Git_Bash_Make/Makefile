CC=g++

all: main
	$(CC) main.cpp func.cpp -o main

main: main.o func.o
	$(CC) main.o func.o -o main

main.o: main.cpp func.h
	$(CC) -c main.cpp

func.o: func.cpp func.h
	$(CC) -c func.cpp

clean:
	rm -rf *.o main
