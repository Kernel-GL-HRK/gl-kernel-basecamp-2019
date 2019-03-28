#include <iostream>
#include <stdlib.h>
#include "guessCheck.h"

using namespace std;

int main()
{
	int guess; 
	
	cout << "Guess a number:" << endl;
	cin >> guess;
	
	checkGuess(guess);
	
	return 0;
}
