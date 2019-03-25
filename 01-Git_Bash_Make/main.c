#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int n;
    int g;
    char c;

    srand(time(NULL));
    n = 1 + (rand() % 9);

    puts("I'm thinking of a number between 1 and 9.");
    puts("Try to guess it:");

    while (1) {
        if (scanf("%d", &g) != 1) {
		/* ignore one char, in case user gave a non-number */
		scanf("%c", &c);
		continue;
	}

    }
}
