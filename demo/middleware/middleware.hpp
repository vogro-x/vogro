#include "vogro/server/context.hpp"

void TestMiddleWare1(vogro::Context& ctx) {
        std::cout << "hello from middleware1" << std::endl;
        ctx.setValue("key", "hhhhhh");
        ctx.Next();
}


void TestMiddleWare2(vogro::Context& ctx) {
    std::cout << "hello from middleware2" << std::endl;
    ctx.Next();
}
