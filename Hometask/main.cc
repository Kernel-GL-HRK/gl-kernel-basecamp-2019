#include <stdio.h>

void InputDaTa(int &k)
{
	printf("Please Enter number beetwens 0 to 9:");
	scanf_s("%d", &k);
}


int main()
{
        int rand_numbers;
        int numbers;
	Input(numbers);
        rand_numbers = rand() % 10;
	return 0;
}
