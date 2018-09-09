#include <fstream>
#include <iostream>
#include <string>

int main(void) {
  std::ifstream ifs("test.txt");
  ifs.seekg(0, std::ios::end);
  int64_t len = ifs.tellg();
  ifs.seekg(0, std::ios::beg);

  std::cout << "len:" << len << std::endl;

  ifs.seekg(-3, std::ios::end);
  std::cout << ifs.rdbuf() << std::endl;
  return 0;
}