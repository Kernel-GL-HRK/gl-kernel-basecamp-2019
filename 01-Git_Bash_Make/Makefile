CC=gcc
CFLAGS= -c -Wall
OBJS = main.o random.o

TARGET: main clean

main: $(OBJS)
	$(CC) $(OBJS) -o $@

main.o: main.c
	$(CC) $(CFLAGS) $<

random.o: random.c
	$(CC) $(CFLAGS) $<
clean:
	rm -f *.o
