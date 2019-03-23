#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void Game(void)
{
    uint32_t number;
    srand(time(0));

    scanf("%u", &number);

    if(number == (rand() % 10))
    {
        printf("You win! \n");
    }
    else
    {
        printf("You loose! \n");
    }
}

int main()
{
    Game();
    return 0;
}
