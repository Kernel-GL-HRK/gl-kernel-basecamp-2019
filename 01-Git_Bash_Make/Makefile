CC=gcc
CFLAGS=-c
LDFLAGS=-o
OBJECTS=guess_a_number.o generator.o
EXECUTABLE=guess_a_number

all: $(OBJECTS)
	$(CC) $(LDFLAGS) $(EXECUTABLE) $(OBJECTS)

guess_a_number.o: guess_a_number.c generator.h
	$(CC) $(CFLAGS) $<

generator.o: generator.c generator.h
	$(CC) $(CFLAGS) $<

clean:
	rm -rf *.o $(EXECUTABLE)
