#include <iostream>
#include <string>
#include "../common.hpp"

void parse_header(std::string header) {
    std::string header_key, header_val;
    bool headerValStart = false;
    for (int i = 0; i < header.length(); i++) {
        if (header[i] == ':') {
            headerValStart = true;
            continue;
        }
        if (!headerValStart) {
            header_key += header[i];
        }
        else{
            header_val += header[i];
        }
    }

    header_key = trim(header_key);
    header_val = trim(header_val);

    std::cout << "|" << header_key << ":" << header_val << "|" << std::endl;
}

int main(void){
    parse_header(" Host : 127.0.0.1 ");
    parse_header("Content-Length: 1024");
    return 0;
}