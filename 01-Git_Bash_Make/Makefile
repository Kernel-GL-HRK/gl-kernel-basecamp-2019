C=g++

all : compiled

compiled : guessNumber.o guessCheck.o
	${C}  -o $@ $^

guessCheck.o : guessCheck.h guessCheck.cpp
	${C} -c $^

guessNumber.o : guessNumber.cpp
	${C} -c $< -o $@
