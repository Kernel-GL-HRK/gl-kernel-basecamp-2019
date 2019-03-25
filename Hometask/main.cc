#include <stdio.h>

void InputDaTa(int &k)
{
	printf("Please Enter number beetwens 0 to 9:");
	scanf_s("%d", &k);
}

void Check(int &numbers, int &number_random)
{
	if (numbers == number_random) {
		printf("You win");
	}
	else {
		printf("You loose");
	}
}

int main()
{
        int rand_numbers;
        int numbers;
	Input(numbers);
        rand_numbers = rand() % 10;
        Check(numbers, rand_numbers);
	return 0;
}
