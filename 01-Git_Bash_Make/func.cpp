#include <iostream>
#include "func.h"

int input()
{
	std::cout << "Enter value: ";
  int inputvalue = 0;
	std::cin >> inputvalue;

  if (inputvalue < 0 || inputvalue > 9)
  {
		std::cout << "Input is out of range!" << std::endl;
    exit(-1);
  }

	return inputvalue;
}

void play()
{
  int user = input();
  int random = rand() % 10;
  std::cout << random << std::endl;

  if (user == random)
    std::cout << "You win!" << std::endl;
  else
    std::cout << "You lose!" << std::endl;
}
