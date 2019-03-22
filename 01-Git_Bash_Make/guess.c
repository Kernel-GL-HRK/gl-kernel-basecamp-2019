#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void game(void)
{
        int seed = 10;
        int computer = rand() % seed + 0x30;
        printf("Guess the number: ");
        int human = getchar();
}

int main(void)
{
        srand(time(NULL));
        return 0;
}
