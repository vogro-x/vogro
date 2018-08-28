#include "../status_code.hpp"
#include <iostream>
#include <string>
#include <assert.h>

int main(void) {
    auto codeMap = vogro::StatusCodeMap::GetInstance();

    assert(codeMap.getPharseByCode(200)=="OK");
    
    assert(codeMap.getPharseByCode(500)=="Internal Server Error");
    
    assert(codeMap.getPharseByCode(999)=="");
    return 0;
}
