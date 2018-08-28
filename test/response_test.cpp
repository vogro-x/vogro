#include "../response.hpp"
#include <iostream>
#include <memory>

int main(void) {
    auto response = std::make_shared<vogro::Response>();

    std::cout << response->makeResponseMsg() << std::endl;

    response->setCode(500);
    response->setPhrase("Internal Server Error");
    std::cout << response->makeResponseMsg() << std::endl;
    return 0;
}