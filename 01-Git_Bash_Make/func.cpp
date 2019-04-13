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
