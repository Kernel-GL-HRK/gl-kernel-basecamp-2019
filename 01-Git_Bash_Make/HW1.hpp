#ifndef _HW1_HPP_
#define _HW1_HPP_
 
#include <iostream>

void play()
{
    
    int value = 4, input;
    while (1){
        std::cout << "Please input number to play a lottery!\n" << std:: endl;
        std::cin >> input;

    if (value == input){
        std::cout << "You won!"<< std::endl;
        break;
    } else
        std::cout << "You lose!"<< std::endl;
}
}

#endif

