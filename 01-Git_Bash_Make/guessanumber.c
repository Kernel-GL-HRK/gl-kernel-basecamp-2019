#include <stdio.h>
#include <time.h>

void main()
{
	srand(unsigned(time(NULL)));

	int RNum, UsrNum;

	printf(" Enter your number:\t");
	scanf("%i", &UsrNum);

	RNum = rand()%10;

	if(UsrNum == RNum) {


	} else {


	}


}
