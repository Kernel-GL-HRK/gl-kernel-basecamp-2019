#include <stdlib.h>
#include "generator.h"

int generate_number(void)
{
    srand(time(NULL));
    return rand() % (GEN_MAX_VALUE + 1);
}
