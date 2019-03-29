#include <iostream>

int main()
{ 
    int value = 4, input;
    while (1){
        std::cout << "Please input number to play a lottery!\n" << std:: endl;
        std::cin >> input;

        if (value == input){
            std::cout << "You won!"<< std::endl;
            break;
        } else {
            std::cout << "You lose!"<< std::endl;
        }
    }     
    return 1;
}