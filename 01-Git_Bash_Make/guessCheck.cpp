#include <iostream>
#include <stdlib.h>
#include "guessCheck.h"

using namespace std;

void checkGuess(int guess){
	srand( time(0) );
        int secret = rand() % 10;
	
	if( guess == secret ) {
                cout << "You win" << endl;
        } else {
                cout << "You loose" << endl;
        }
}
