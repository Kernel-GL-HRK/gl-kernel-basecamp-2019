#include <iostream>
#include<stdlib.h>

using namespace std;

int main()
{
	srand( time(0) );
	
	int secret = rand() % 10 + 1;
	int guess;
	
	cout << "Guess a number:" << endl;
	cin >> guess;
	
	if( guess == secret ) {
		cout << "You win" << endl;
	} else {
		cout << "You loose" << endl;
	}
	
	return 0;
}
