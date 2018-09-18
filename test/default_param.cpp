#include <iostream>
#include <string>


void F (int a, std::string b, char c = 'A', int d=100,std::string e= "hello,world"){
    std::cout<<a<<" "<<b<<" "<<c<<" "<<d<<" "<<e<<std::endl;
}


int main(void){
    F(2,"nnn");


    return 0;
}