#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    int random_number = rand() % 10;
<<<<<<< HEAD

    int userNumber;
    scanf("%d",&userNumber);
<<<<<<< HEAD
=======
>>>>>>> 00c36c1... Git_Lesson_v0.0.2
=======
    userNumber == random_number ? printf("\nYou win!\n") : printf("\nYou win!\n");
>>>>>>> e7dc850... Git_Lesson_v0.0.4
    return 0;
}
