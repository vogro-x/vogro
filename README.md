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

## start

### the server part code
``` c++
#include "vogro/server/http.hpp"
#include "vogro/server/context.hpp"

void GlobalMiddleWare(vogro::Context &ctx) {
    ctx.response->addBody("hello for global middleware<br>");
    ctx.setValue("key", "value from global middleware");
    ctx.Next();
}

void GroupMiddleWare(vogro::Context &ctx) {
    ctx.response->addBody("hello for group middleware<br>");
    ctx.Next();
}

void TestHandler1(vogro::Context &ctx) {
    ctx.response->addBody("hello from handler1<br>");
    ctx.Next();
}

void TestHandler2(vogro::Context &ctx) {
    ctx.response->addBody("hello from handler2<br>");
    ctx.Next();
}

void TestHandler3(vogro::Context &ctx) {
    ctx.response->addBody("hello from handler3<br>");
}

int main(int argc, char **argv) {

    vogro::Server server(8080, 4);

    server.Use(GlobalMiddleWare);

    server.addRoute("/", "GET", [](vogro::Context &ctx) {
        ctx.response->addBody("hello, vogro<br>");
        auto val = ctx.getValue("key");
        ctx.response->addBody(val);
    });

    server.Get("/hello/{str:name}/{int:age}/", [](vogro::Context &ctx) {
        auto name = ctx.request->getPathParam("name");
        auto age = ctx.request->getPathParam("age");

        std::stringstream msg_string;
        msg_string << "hello " << name << ", you are " << age << "years old.";

        ctx.response->addBody(msg_string.str());
    });

    auto methodGroup = server.makeGroup("/method/", GroupMiddleWare);
    {
        methodGroup->Get("/get/", TestHandler1, TestHandler2, TestHandler3);

        methodGroup->Post("/post/", [](vogro::Context &ctx) {
            ctx.response->addBody("post");
        });

        methodGroup->Put("/put/", [](vogro::Context &ctx) {
            ctx.response->addBody("put");
        });

        methodGroup->Delete("/delete/", [](vogro::Context &ctx) {
            ctx.response->addBody("delete");
        });
    }

    server.Post("/json/", [](vogro::Context &ctx) {
        auto json = ctx.request->ReadJSON();
        auto name = json.at("name").get<std::string>();
        ctx.response->addBody(name);
    });

    server.onError(404, [](vogro::Request &request, vogro::Response &response) {
        response.addBody("Custom Not Found");
        return;
    });

    server.run();
}
```

### the test part code
``` c++
#include "vogro/test-suite/client.hpp"

int main() {
    vogro::VogroTestClient client("127.0.0.1", "8080");

    client.Get("/").Expect().Body().Contains("value from global middleware");

    client.Get("/hello/{name}/{age}/").withPath("name", "Andrewpqc").withPath("age", "22").
            Expect().Body().Equal("hello Andrewpqc, you are 22 years old.");

    client.Get("/method/get/").Expect().Body().Contains("get");

    client.Post("/method/post/").Expect().Body().Contains("post");

    client.Put("/method/put/").Expect().Body().Contains("put");

    client.Delete("/method/delete").Expect().Body().Contains("delete");

    client.Post("/json/").withJSON("{\"name\":\"Andrewpqc\"}").Expect().
            Body().Contains("Andrewpqc");

    client.Get("/not/found/").Expect().Body().Equal("Custom Not Found");

    return 0;
}
```
the above code started an http server which have 4 threads, each thread runs an `io_service` object. And the server listens on port `8080`. And It has one route `/` with method `Get`, the handler of this route is a C++ anonymous function, which just return a simple string `hello, vorgo` to clients.

Want to know more ? See [Wiki Page](https://github.com/vogro-x/vogro/wiki).


## help
vogro needs a full-feature static server for large file tranfer and we also need websocket support. So, if you want to imporve vogro, please pull request to this project.


