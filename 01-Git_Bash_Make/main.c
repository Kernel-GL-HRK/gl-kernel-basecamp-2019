#include <stdlib.h>
#include <time.h>
#include "guess.h"

int main(void)
{
	srand(time(NULL));
	game();
	return 0;
}
