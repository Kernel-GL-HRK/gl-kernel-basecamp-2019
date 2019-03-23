#include <cstdlib>
#include<stdio.h>
#include<time.h>
using namespace std;

int main(int argc, char** argv) {
int PlayerVallue;
while(1){
printf ("ВВедите Ваше число\n");
scanf("%d",&PlayerVallue);
srand(time(NULL));
int AiVallue = rand() %10 +1;

printf("%d\n", AiVallue);
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