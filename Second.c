/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#define Game_algorithm  while(1) {int PlayerVallue;printf ("Введите Ваше число\n");scanf("%d",&PlayerVallue);srand(time(NULL));int AiVallue = rand() % 10 +1;printf ("%d \n", AiVallue);if(PlayerVallue==AiVallue)    {printf ("Ничья\n"); }    if(PlayerVallue > AiVallue){      printf ("Вы победили\n");    }    if (PlayerVallue < AiVallue){  printf ("Поражение\n"); }    printf ("Ваше число = %d,Число противника = %d\n", PlayerVallue, AiVallue);} 

/* while(1) {
       int PlayerVallue;
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
    {  printf ("Поражение\n");
    }
    printf ("Ваше число = %d,Число противника = %d\n", PlayerVallue, AiVallue);

    }*/
