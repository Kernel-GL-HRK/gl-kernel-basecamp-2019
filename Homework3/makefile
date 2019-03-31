CC=gcc
CFLAGS=-c -Wall

all: prog

prog: main.o InputData.o Check.o
	 $(CC) main.o InputData.o Check.o -o prog

main.o: main.cpp InputData.h Check.h
	$(CC) $(CFLAGS)  main.cpp

InputData.o: InputData.cpp InputData.h
	$(CC) $(CFLAGS)  InputData.cpp

Check.o: Check.cpp Check.h
	$(CC) $(CFLAGS)  Check.cpp

clean:
	 rm -rf *.o prog


