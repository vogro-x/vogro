#include <string>
#include <iostream>

#include "../utils.hpp"

int main (void){
    auto header_pair=vogro::parse_header("Content-Length: 1024");
    std::cout<<"|"<<header_pair.first<<":"<<header_pair.second<<"|"<<std::endl;


    auto threesome=vogro::parse_request_line("GET /abc/dd?a=1&b=2 HTTP/1.1");
    std::cout<<"|"<<threesome.first<<"|"<<std::endl;
    std::cout<<"|"<<threesome.second.first<<"|"<<std::endl;
    std::cout<<"|"<<threesome.second.second<<"|"<<std::endl;
    return 0;
}