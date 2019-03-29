#include "game_code.h"

void Game()
{
	srand(time(NULL));

	int input;
	int answer = rand() % 10;

	printf("Input a number from 0 to 9: ");
	scanf("%d", &input);

	if(input == answer)
		printf("Correct! You win!\n");
	else
		printf("Wrong! You lose!\n");
}
