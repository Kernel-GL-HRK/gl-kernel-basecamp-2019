#include <iostream>

int main()
{ 
    int value = 4;

    if (value == rand()%10) {
        std::cout << "You won!"<< std::endl;
    } else {
        std::cout << "You lose!"<< std::endl;
    }
    return 1;
}
