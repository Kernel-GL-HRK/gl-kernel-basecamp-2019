#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) 
{
        int d;
        int r;
    
        srand(time(NULL));
        printf("Enter number 0-9>");
        scanf("%d", &d);
        r = rand() % 10;
        if (d == r) {
                printf("You win\n");
                return 0;
        }
        else {
                printf("You loose\n");
                return 1;
        }
    
        return 0;
}
