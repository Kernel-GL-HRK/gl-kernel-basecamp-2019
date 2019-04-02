#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"

int main(void) 
{
   
        if (game())
                printf("You win\n");
        else
                printf("You loose\n");
    
        return 0;
}
