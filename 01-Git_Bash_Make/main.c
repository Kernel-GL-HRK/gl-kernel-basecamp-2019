#include "guess_game.h"

int main(void)
{
        int UsrNum = 0, AiNum = 0;

        UserInput(&UsrNum);

        AiRand(&AiNum);

        NumCmp(UsrNum,AiNum);

        return 0;
}
