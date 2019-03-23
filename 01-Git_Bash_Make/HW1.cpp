#include <iostream>

int main()
{ 
    int value = 4, input;
    std::cout << "Please input number to play a lottery!\n" << std:: endl;
    std::cin >> input;

    if (value == rand()%10) {
        std::cout << "You won!"<< std::endl;
    } else {
        std::cout << "You lose!"<< std::endl;
    }
    return 1;
}
