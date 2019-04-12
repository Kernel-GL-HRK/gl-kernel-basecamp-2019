#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	cout << "Enter value: ";
	int inputvalue = 0;
	cin >> inputvalue;

	if (inputvalue < 0 || inputvalue > 9)
	{
		cout << "Input is out of range!" << endl;
		exit(-1);
	}

	return 0;
}
