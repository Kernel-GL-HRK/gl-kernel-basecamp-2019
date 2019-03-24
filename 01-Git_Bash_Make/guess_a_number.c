#include <stdio.h>
#include <stdlib.h>

#define GEN_MAX_VALUE   9

int generate_number(void);

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

int generate_number(void)
{
    srand(time(NULL));
    return rand() % (GEN_MAX_VALUE + 1);
}
