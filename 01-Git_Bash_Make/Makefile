C=gcc

DESTFILE=GuessANumber

MAIN=random_game

GAMECODE=game_code

all: $(DESTFILE)

$(DESTFILE): $(MAIN).o $(GAMECODE).o
	$(C) $(MAIN).o $(GAMECODE).o -o $(DESTFILE)

$(MAIN).o: $(MAIN).c
	$(C) -c $(MAIN).c -o $(MAIN).o

$(GAMECODE).o: $(GAMECODE).c $(GAMECODE).h
	$(C) -c $(GAMECODE).c -o $(GAMECODE).o

clean:
	rm *.o $(DESTFILE)
