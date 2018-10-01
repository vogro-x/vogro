A middleware is a function, the function signature is `void Func(vogro::Context & ctx)`, `ctx.Next()` calls the next handler/middleware in the handler/middleware array.

The following is an simple example middleware:
``` C++
void TestMiddleWare1(vogro::Context& ctx) {
    std::cout << "hello from middleware1" << std::endl;
    ctx.Next();
}
```

If you need to pass information between middlewares，you can use `ctx.setValue()`,`ctx.getValue()`. the following is an example:
``` C++
void TestMiddleWare1(vogro::Context& ctx) {
    std::cout << "hello from middleware1" << std::endl;
    ctx.setValue("key","value from middleware1");
    ctx.Next();
}

void TestMiddleWare2(vogro::Context& ctx) {
    std::cout << "hello from middleware2" << std::endl;
    auto val = ctx.getValue("key");  // val == "value from middleware1"
    ctx.Next();
}
```
