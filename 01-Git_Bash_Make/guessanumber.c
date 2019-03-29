#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void main()
{
	srand(time(NULL));

	int RNum, UsrNum;

	RNum = rand()%10;

	printf(" Enter your number:\t");
	scanf("%i", &UsrNum);

	if (UsrNum == RNum)
		printf(" \n Wow, you win!\n");
	else
		printf(" \n Oops, you lost...\n");
}
