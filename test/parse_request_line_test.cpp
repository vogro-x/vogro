#include <iostream>
#include <string>

void parse_request_line(std::string request_line) {
    std::string method, url, version;
    char flag = 'm';
    bool versionNumberStart = false;
    for (auto i = 0; i < request_line.length(); i++) {
        if (flag == 'm'){
            if (request_line[i] == ' ') {
                flag = 'u';
                continue;
            }
            method += request_line[i];
        }
        if (flag == 'u'){
            if (request_line[i] == ' ') {
                flag = 'v';
                continue;
            }
            url += request_line[i];
        }
        if (flag == 'v'){
            if (request_line[i] == '/') {
                versionNumberStart = true;
                ++i;
            }
            if (versionNumberStart)
                version += request_line[i];
        }
    }
    std::cout << "|" << method << "|" << std::endl;
    std::cout << "|" << url << "|" << std::endl;
    std::cout << "|" << version << "|" << std::endl
              << std::endl;
}

int main(void) {
    parse_request_line("GET /abc/dd?a=1&b=2 HTTP/1.1");
    parse_request_line("POST /abc HTTP/1.1");
    parse_request_line("PUT / HTTP/1.1");
    return 0;
}