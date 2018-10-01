# [vogro](https://github.com/vogro-x/vogro)
vogro: an excellent web framework, born for simplicity and high performance.It provides the user with the processing function of the request message, responds to the message generation function, and provides a full-featured test tool, and these functions are provided to the user with a well-defined and easy-to-use interface.

## install
`vogro` is based on `boost/asio` and `boost/ssl`. So before use vogro, you have to install boost and ssl. For `Ubuntu18.04` or higher, just to type the following commands in your terminal:
```
$ sudo apt-get install boost
$ sudo apt-get install libssl-dev
```
for other operating system and other vsrsion of ubuntu, you can google it for how to install `boost` and `ssl` in your system. I don't want to talk too much about that.

## build
run the following commands to build the demo project in the source code tree:
``` bash
$ git clone https://github.com/vogro-x/vogro.git &&
cd vogro/demo && mkdir -p build && cd build && cmake .. &&
make
```

## get start
``` c++
#include "src/server/http.hpp"
#include "src/server/context.hpp"

void TestMiddleWare(vogro::Context& ctx) {
        ctx.response->addBody("hello for middleware<br>");
        ctx.setValue("key", "value from middleware");
        ctx.Next();
}

nt main() {

    vogro::Server server(8080, 4);

    server.Use(TestMiddleWare1); 

    server.addRoute("/", "GET", [](vogro::Context &ctx) {
        ctx.response->addBody("hello, vogro<br>");

        auto val = ctx.getValue("key");

        ctx->response->addBody(val);
        
        return;
    });

    server.run();
}
```
the above code started an http server which have 4 threads, each thread runs an `io_service` object. And the server listens on port `8080`. And It has one route `/` with method `Get`, the handler of this route is a C++ anonymous function, which just return a simple string `hello, vorgo` to clients.

Want to know more ? See [Wiki Page](https://github.com/vogro-x/vogro/wiki).


