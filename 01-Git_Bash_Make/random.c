#include "random.h"

int get_random_number()
{
	 srand(time(NULL));
     return (rand() % 10);
}

int get_user_number()
{
	int user_number;
    scanf("%d",&user_number);
    return user_number;
}