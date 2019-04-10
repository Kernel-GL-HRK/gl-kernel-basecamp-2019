#include <stdio.h>

int main()
{
	int num = 0;
	printf("enter number: ");
	scanf("%i", &num);

	if (num < 0 || num > 9)
	{
		printf("the number is bigger than 9 or smaller then 0.\n");
		exit(1);
	}

	return 0;
}
