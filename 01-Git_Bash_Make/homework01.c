#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    int computer = rand() % 10;
    int user = 0;
    while (1) {
        printf("Pleace, Enter number 0-9: \n");
        printf("For exit, enter number: -1\n");
        scanf("%d", &user);
        if (user < -1 || user > 9)
        {
            printf("Number should be within of 0-9 range\n");
            continue;
        }
        if (user == -1)
        {
            break;
        }
        if (user == computer)
        {
            printf("You Win!!!\n");
        }
        else
        {
            printf("You Lose!!!\n");
        }
    }
    printf("Goodbye\n");
    return 0;
}
