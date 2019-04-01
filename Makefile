CC = gcc
Opt = -Wall
main.exe:main.o Second.o
	$(CC) $(Opt) ./main.o ./Second.o -o ./main.exe
main.o:main.c
	$(CC) $(Opt) -c ./main.c -o ./main.o
Second.o:Second.c
	$(CC) $(Opt) -c ./Second.c -o ./Second.o


