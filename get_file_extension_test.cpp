#include <assert.h>
#include <iostream>
#include "vogro_utils.hpp"

int main(void) {
    assert(getFileExtension("/dsfgfhdsfg/dfsdf.Jpg") == "jpg");
    std::cout << "get_file_extension_test passed!" << std::endl;

    return 0;
}