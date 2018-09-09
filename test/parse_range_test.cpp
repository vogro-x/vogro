#include <assert.h>
#include <iostream>
#include <string>

std::pair<size_t, size_t> parseRange(const std::string& Ranges) {
    auto pos1 = Ranges.find_first_of('=');
    auto pos2 = Ranges.find_last_of('-');
    auto start = Ranges.substr(pos1 + 1, pos2 - pos1 - 1);
    auto end = Ranges.substr(pos2 + 1);
    std::cout << "|" << start << "|" << end << "|" << std::endl;
    auto int_start = (start == "") ? 0 : std::stol(start);
    auto int_end = (end == "") ? 0 : std::stol(end);
    return std::make_pair(int_start, int_end);
}

int main(int argc, char const* argv[]) {
    auto range1 = "range=123-256";
    auto range2 = "range=123-";
    auto range3 = "range=-256";
    auto p1 = parseRange(range1);
    auto p2 = parseRange(range2);
    auto p3 = parseRange(range3);

    assert(p1.first == 123 && p1.second == 256);
    assert(p2.first == 123 && p2.second == 0);
    assert(p3.first == 0 && p3.second == 256);
    return 0;
}
