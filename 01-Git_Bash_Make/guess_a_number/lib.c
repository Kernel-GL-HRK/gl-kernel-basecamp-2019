/**
 * @file lib.c
 * library implementation.
 * @author George Molchanov
 * @date 2019.03.28
 */

#include <stdlib.h>

#define RND_MIN 0
#define RND_MAX 9

_Bool chek_num(int num)
{
	_Bool result = 0;
	if (num >= RND_MIN && num <= RND_MAX) {
		int random_num = rand() % (RND_MAX - RND_MIN + 1) + RND_MIN;
		if (num == random_num) {
			result = 1;
		}
	}
	return result;
}
