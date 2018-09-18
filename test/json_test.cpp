#include "../src/json.hpp"
#include <iostream>
#include <string>

using json = nlohmann::json;
int main (void){
    json j;
    j["pi"]= 1.234;
    auto s = j.dump();
    std::cout<<s<<std::endl;
    return 0;
}