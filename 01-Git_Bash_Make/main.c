#include "random.h"

int main()
{
    printf("%s",get_user_number() == get_random_number() ? "You win!\n" : "You loose!\n");
    return 0;
}
