#include <assert.h>
#include <iostream>
#include "../mime_type.hpp"


int main(void) {
  MimeTypeMap &mime = MimeTypeMap::GetInstance();
  assert(mime.getMimeTypeByExt("notExist") == "");
  assert(mime.getMimeTypeByExt("html") == "text/html");
  std::cout << "mine_type_test test pass" << std::endl;
  return 0;
}