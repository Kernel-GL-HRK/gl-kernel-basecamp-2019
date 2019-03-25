#include <stdio.h>

void InputDaTa(int &k)
{
	printf("Please Enter number beetwens 0 to 9:");
	scanf_s("%d", &k);
}


int main()
{
        int numbers;
	Input(numbers);
	return 0;
}
