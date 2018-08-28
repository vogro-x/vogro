#include <iostream>
#include "../common.hpp"

int main(void) {

    std::string s0 = "";
    std::string s1 = "abc  ";
    std::string s2 = "  bugbugbug";
    std::string s3 = "   balabala   ";

    s0 = trim(s0);
    s1 = rtrim(s1);
    s2 = ltrim(s2);
    s3 = trim(s3);

    std::cout << "s0:" << "|" << s0 << "|" << std::endl;
    std::cout << "s1:" << "|" << s1 << "|" << std::endl;
    std::cout << "s2:" << "|" << s2 << "|" << std::endl;
    std::cout << "s3:" << "|" << s3 << "|" << std::endl;

    return 0;
}