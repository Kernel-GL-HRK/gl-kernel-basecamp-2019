FUNC=guessanumber
MAIN=main
HEAD=header
COMPILE=gcc

$(FUNC).game: $(MAIN).o $(FUNC).o
	$(COMPILE) $(MAIN).o $(FUNC).o -o $(FUNC).game

$(MAIN).o: $(MAIN).c
	$(COMPILE) -c $(MAIN).c -o $(MAIN).o

$(FUNC).o: $(FUNC).c $(HEAD).h
	$(COMPILE) -c $(FUNC).c -o $(FUNC).o
