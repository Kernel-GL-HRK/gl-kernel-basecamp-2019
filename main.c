/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: vvi
 *
 * Created on March 29, 2019, 6:10 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Second.c"

/*
 * 
 */
int main(int argc, char** argv) {
    Text
    while(1) {
    printf ("Введите Ваше число\n");
    scanf("%d",&PlayerVallue);
    srand(time(NULL));
    int AiVallue = rand() % 10 +1;

    printf ("%d \n", AiVallue);

    if(PlayerVallue==AiVallue)
    {
    printf ("Ничья\n");
    }
    if(PlayerVallue > AiVallue)
    {
        printf ("Вы победили\n");
    }
    if (PlayerVallue < AiVallue)
    {
    printf ("Поражение\n");
    }
    printf ("Ваше число = %d,Число противника = %d\n", PlayerVallue, AiVallue);

    }
    return 0;
}

/*
 * 
 */

