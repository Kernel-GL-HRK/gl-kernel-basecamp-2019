/**
 * @file main.c
 * contains main function.
 * @author George Molchanov
 * @date 2019.03.24
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lib.h"

/**
 * Entry point.
 * 
 * @return exit code
 */
int main(void)
{
	srand((unsigned)time(NULL));

	printf("Input a number: ");
	int guessed_num = 0;
	scanf("%d", &guessed_num);

	if (chek_num(guessed_num))
		puts("You win");
	else
		puts("You loose");

	return 0;
}
