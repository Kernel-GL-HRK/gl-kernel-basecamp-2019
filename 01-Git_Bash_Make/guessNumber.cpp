#include <iostream>
#include <stdlib.h>

using namespace std;

int main()
{
	srand( time(0) );
	
	int secret = rand() % 10;
	int guess; 
	
	cout << "Guess a number:" << endl;
	cin >> guess;
	
	return 0;
}
