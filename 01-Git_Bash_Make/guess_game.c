#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "guess_game.h"

void UserInput(int *_num)
{
        printf("Input number from 0 to 9\n");
        scanf("%d",_num);
}

void AiRand(int *_num)
{
	srand(time(NULL));
        *_num = rand()%10;
}

void NumCmp(int _usr, int _ai)
{
        if ( _usr == _ai )
                printf("Victory\n");
        else
                printf("Defeat\n");
}
