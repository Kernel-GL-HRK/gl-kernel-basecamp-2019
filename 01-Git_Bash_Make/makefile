COMPILER=gcc
OBJS=main.o random_func.o

install: ${OBJS}
	${COMPILER} -o Guess_a_number_game ${OBJS}

main.o: random_func.o task_1.c
	${COMPILER} -o $@ -c task_1.c $<

random_func.o: random_func.c
	${COMPILER} -o $@ -c $<

clean:
	rm *.o

