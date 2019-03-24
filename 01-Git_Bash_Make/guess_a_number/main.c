/**
 * @file main.c
 * contains main function.
 * @author George Molchanov
 * @date 2019.03.24
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define RND_MIN 0
#define RND_MAX 9

/**
 * Compares the entered number with a randomly generated number.
 * @param num entered number
 * @return 1 if the num equals the generated number; 0 - otherwise
 */
_Bool chek_num(int num);

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

_Bool chek_num(int num)
{
	_Bool result = 0;
	if (num >= RND_MIN && num <= RND_MAX) {
		int random_num = rand() % (RND_MAX - RND_MIN + 1) + RND_MIN;
		if (num == random_num) {
			result = 1;
		}
	}
	return result;
}
