#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv)
{
	srand(time(NULL));

	cout << "Enter value: ";
	int inputvalue = 0;
	cin >> inputvalue;

	if (inputvalue < 0 || inputvalue > 9)
	{
		cout << "Input is out of range!" << endl;
		exit(-1);
	}

	int random = rand() % 10;
	cout << random << endl;

	if (inputvalue == random)
		cout << "You win!" << endl;
	else
		cout << "You lose!" << endl;

	return 0;
}
