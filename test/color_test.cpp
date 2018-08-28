#include "../common.hpp"
#include <iostream>

int main(void){
    INITCOLOR(RED_COLOR);
    std::cout << "this is red." << std::endl;
    INITCOLOR(ZERO_COLOR);

    INITCOLOR(YELLOW_COLOR);
    std::cout << "this is yellow." << std::endl;
    INITCOLOR(ZERO_COLOR);

    INITCOLOR(BLUE_COLOR);
    std::cout << "this is blue." << std::endl;
    INITCOLOR(ZERO_COLOR);
   
    return 0;
}