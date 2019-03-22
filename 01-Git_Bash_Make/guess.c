#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void game(void)
{
        int seed = 10;
        int computer = rand() % seed + 0x30;
        printf("Guess the number: ");
        int human = getchar();
        if (human == computer)
                puts("You win!");
        else
                printf("You lose! I guessed %c\n", computer);
}

int main(void)
{
        srand(time(NULL));
        return 0;
}
