#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

int main(){
	srand(time(NULL)); 
	int computer = rand() % 10;
	unsigned int user;
	printf("Pleace, Enter number 0-9: \n");
	scanf("%d", &user);
	if (user == computer)
	{
		printf("You Win!!!");
	}
	else
	{	
		printf("You Lose!!!");
	}
		
}
