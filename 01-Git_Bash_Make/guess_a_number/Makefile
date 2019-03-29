CC=clang
CC_FLAGS=-O3 -target x86_64-mingw64

guess_a_number.exe: obj
	${CC} *.o -o $@ ${CC_FLAGS}

obj: main.c lib.c
	${CC} $^ -c ${CC_FLAGS}

clean:
	del *.o
	del *.exe
