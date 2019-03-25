#include <stdio.h>

int main(){
  int guess;
  printf("your guess: ");
  scanf("%d",&guess);

  srand ( time(NULL) );
  int random = rand() % 10 + 1;

  if (random == guess){
    printf("you are right!\r\n");
  } else {
    printf("unfortunately random number is: %d \r\n", random);
  }

  return 0;
}
