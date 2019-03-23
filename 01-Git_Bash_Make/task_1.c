#include <stdio.h>
#include <stdlib.h> //for rand()
#include <time.h>
int main()
{
    int usr_input;
    int random_number = rand() % 10;
    for(int i = 0; i < 100; ++i){
    printf("Enter a number");
    scanf("%d", &usr_input);
    if(usr_input == random_number)
        printf("You win\n");
    else
        printf("You loose\n");
    return 0;
}
