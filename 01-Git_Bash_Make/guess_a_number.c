#include <stdio.h>
#include <stdlib.h>

#define GEN_MAX_VALUE   9

int generate_number(void);

int main()
{
    return 0;
}

int generate_number(void)
{
    srand(time(NULL));
    return rand() % (GEN_MAX_VALUE + 1);
}
