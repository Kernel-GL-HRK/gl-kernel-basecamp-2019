#include <stdio.h>
#include <stdlib.h>
#include "InputData.h"
#include "Check.h"

int main()
{
	int rand_number;
	int numbers;
	InputData(numbers);
	rand_number = rand() % 10;
	Check(numbers, rand_number);
	return 0;
}
