CMP=gcc
OBJS=main.o func.o

prog: ${OBJS}
	${CMP} -o $@ ${OBJS}

main.o: func.h main.c
	${CMP} -c main.c

func.o: func.h func.c
	${CMP} -c func.c 

clean:
	rm -rf *.o
