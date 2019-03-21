#include <stdio.h>
//#include <ctime.h>

int main(){
  int x;
  printf("your number: ");
  scanf("%d",&x);
  printf("your number is: %d \r\n", x);

  srand ( time(NULL) );
  int random = rand() % 10 + 1;
  printf("random number is: %d \r\n", random);

  return 0;
}
