#include <stdio.h>
#include <stdlib.h> //for rand()
int main(){
    int usr_input;
    printf("Enter a number");
    scanf("%d", &usr_input);
    int random_number = rand() % 10;
    if(usr_input == random_number)
    {
        //some action
    }
    else
    {
        //another action
    }

    return 0;
}
