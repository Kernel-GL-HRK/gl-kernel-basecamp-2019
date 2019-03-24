#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    int random_number = rand() % 10;

    int userNumber;
    scanf("%d",&userNumber);
    return 0;
}
