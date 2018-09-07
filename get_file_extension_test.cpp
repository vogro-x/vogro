#include "vogro_utils.hpp"
#include <assert.h>
#include <iostream>

int main (void){

    assert(getFileExtension("/dsfgfhdsfg/dfsdf.Jpg")=="jpg");
    std::cout<<"get_file_extension_test passed!"<<std::endl;

    return 0;
}