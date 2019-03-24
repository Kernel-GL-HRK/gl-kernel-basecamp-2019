#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
	srand(time(NULL));

	int input;
	int answer = rand() % 10;

	printf("Input a number from 0 to 9: ");
	scanf("%d", &input);
	return 0;
}
