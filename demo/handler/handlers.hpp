#include "../../src/server/context.hpp"

void TestHandler1(vogro::Context& ctx)
{
    ctx.response->addBody("hello from handler1<br>");
    ctx.Next();
}

void TestHandler2(vogro::Context& ctx)
{
    ctx.response->addBody("hello from handler2<br>");
    ctx.Next();
}

void TestHandler3(vogro::Context& ctx)
{
    ctx.response->addBody("hello from handler3<br>");
}