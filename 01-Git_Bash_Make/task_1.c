#include <stdio.h>
#include "random_func.h"
int main()
{
    int usr_input;
    int random_number = Generate_random_number()  % 10;
    printf("Enter a number ");
    scanf("%d", &usr_input);
    if(usr_input == random_number)
        printf("You win\n");
    else
        printf("You loose\n");
    return 0;
}
