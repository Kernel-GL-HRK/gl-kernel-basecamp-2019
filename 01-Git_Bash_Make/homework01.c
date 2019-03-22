#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    int computer = rand() % 10;
    int user = 0;
    while(0 < user || user > 9) {
        printf("Pleace, Enter number 0-9: \n");
        printf("For exit, enter number: -1");
        scanf("%d", &user);
        if (user == computer)
        {
            printf("You Win!!!");
        }
        else
        {
            printf("You Lose!!!");
        }
    }
    return 0;
}
