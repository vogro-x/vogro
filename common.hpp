#include <string>
#include <cstdio>
#include <map>

#define __VOGRO_VERSION__ "0.1";

//color mocro
// #define	CLEARSCREEN() printf("\033[H\033[2J")
// #define	INITCOLOR(color) printf("\033[%sm", color)
// #define	RED_COLOR "31"
// #define	GREEN_COLOR	"32"
// #define	YELLOW_COLOR "33"
// #define	BLUE_COLOR "34"
// #define	ZERO_COLOR "0"

#ifndef __COMMON_HPP__
#define __COMMON_HPP__ 


std::string ltrim(std::string str) {
    auto length = str.length();
    while(str.length()) {
        if (str[0] == ' ')
            str.erase(0, 1);
        else
            return str;
    }
    return str;
}

std::string rtrim(std::string str) {
    auto length = str.length();
    for (auto i = length - 1; i >= 0; i--) {
        if (str[i] == ' ')
            str.pop_back();
        else
            return str;
    }
    return str;
}

std::string trim(std::string str) {
    return rtrim(ltrim(str));
}



#endif