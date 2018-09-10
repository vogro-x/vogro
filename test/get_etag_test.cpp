#include <sys/stat.h>
#include <iostream>
#include <sstream>

// E-tag:a-b-c
// a: i-node number (serial number)
// b: time of last modification
// c: time of last file status changes

std::string getEtag(std::string filename) {
    struct stat statbuf;
    stat(filename.c_str(), &statbuf);
    std::stringstream ss;
    ss << statbuf.st_ino << "-" << statbuf.st_mtime << "-" << statbuf.st_size;
    return ss.str();
}

int main(void) {
    std::string s = getEtag("/home/andrew/Music/vogro/test/Makefile");
    std::cout << s << std::endl;

    return 0;
}