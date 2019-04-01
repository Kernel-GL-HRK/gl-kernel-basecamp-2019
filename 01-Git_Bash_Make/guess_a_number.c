#include <stdio.h>
#include "generator.h"

int main()
{
    int user_number = 0;

    printf("Please input some number from 0 to 9: ");
    scanf("%d", &user_number);

    if(user_number == generate_number()) {
        printf("You win!");
    } else {
        printf("You loose!");
    }

    return 0;
}

