#include "guesanumber.h"
#include <stdlib.h>
#include <stdio.h>

int startGame()
{
        int number;

        for (int i = 0; i < 10; ++i) {
                printf("\nInsert the number -> ");
                scanf("%d", &number);

                if (rand() % 10 == number) {
                        printf("You win!");
                } else {
                        printf("You loose!");
                }
        }

        return 0;
}
