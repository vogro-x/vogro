#include <string>
#include <iostream>
#include "../log.hpp"

int main (void) {
    auto logger = vogro::Logger::getLoggerInstance();
    logger.printLogtoFile("DEBUG","/abc/df/","hhh",200);
    logger.printLogtoFile("DEBUG","/abc/df/","hhh",200);
    logger.printLogtoFile("DEBUG","/abc/df/","hhh",200);
    return 0;
}