#include "Check.h"
#include <stdio.h>

void Check(int &numbers, int &rand_number)
{
	if (numbers == rand_number) {
		printf("You win");
	}
	else {
		printf("You loose");
	}

}
