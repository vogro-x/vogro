#include <iostream>
#include "utils.hpp"
#include <string>


int main (){
    auto s ="123=123";
    auto v =split(s,'=');
    for(auto i:v)
        std::cout<<i<<std::endl;
    return 0;
}