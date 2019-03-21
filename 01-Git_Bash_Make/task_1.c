#include <stdio.h>
#include <stdlib.h> //for rand()
int main(){
    int usr_input;
    printf("Enter a number");
    scanf("%d", &usr_input);
    int random_number = rand() % 10;

    return 0;
}
