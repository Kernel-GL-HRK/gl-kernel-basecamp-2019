#include <stdlib.h>
#include <time.h>

int main()
{
    unsigned int user_number = 0;
    unsigned int rand_number = 0;
    printf("Enter any number from 0 to 9 >");
    scanf("%i", &user_number);
    srand(time(NULL));
    rand_number = rand() % 10;
    return 0;
}
