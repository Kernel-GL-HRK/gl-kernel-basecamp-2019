#include<iostream>
using std::cout;
using std::cin;
#include<stdint.h>
#include<ctime>
#include<stdlib.h>

int main()
{
int num;
char answer = 'y';
while(answer == 'y')
{
cout << "Please, enter some number from 0 to 9:\n";
cin >> num;
while(num > 9 || num < 0)
{
cout << "You entered wrong number\n"
<< " Please, enter number from 0 to 9:\n";
cin >> num;
}
srand(time(0));		//auto randomisation
int randNum = rand() % 10;
cout << "Your number is: " << num <<" and my number is: " << randNum;
cout << "\nIf want to continue, press 'y', if you want to exit, press another"
<< " character\n";
cin >> answer;
}
return 0;
}
