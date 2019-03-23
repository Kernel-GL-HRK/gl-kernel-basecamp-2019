#include <stdio.h>
#include <stdlib.h>

int main(void)
{
        int input = 0;
        int ai = rand()%10;

        printf("Input number from 0 to 9\n");
        scanf("%d",&input);

        if ( ai == input ) 
                printf("Victory\n");
        else 
                printf("Defeat\n");

        return 0;
}