#include "http.hpp"

int main()
{
    vogro::Server server(12345, 4);

    server.use([](vogro::Context& ctx) {
        std::cout<<"coming................"<<std::endl;
        ctx.setValue("key","hhhhhh");
        ctx.Next();
    });

    server.addRoute("/", "GET", [](vogro::Context& ctx) {
        ctx.response->addBody("<h1>Index Page</h1>");
        return;
    });

    server.addRoute("/username/{str:name}/", "GET", [](vogro::Context& ctx) {
        auto name = ctx.request->getPathParam("name");
       auto key= ctx.getValue("key");
        ctx.response->addBody(name);
        ctx.response->addBody(key);
        return;
    });

    server.addRoute("/course/{str:coursename}/user/{int:id}/", "GET", [](vogro::Context& ctx) {
        auto coursename = ctx.request->getPathParam("coursename");
        auto id = ctx.request->getPathParam("id");
        ctx.response->addBody("coursename:" + coursename + ",id: " + id);
        return;
    });

    /**************************************************
    此处请求的时候一定要有Content-Length头部，后面这里需要改进
    示例请求

    POST /a/post/ HTTP/1.1
    Host: 127.0.0.1:12345
    Content-Type: application/json
    Content-Length: 20
    Cache-Control: no-cache
    Postman-Token: c8810efa-1872-fb0c-978f-c365b14af5f5

    {"name":"andrewpqc"}
    ****************************************************/
    server.addRoute("/a/post/", "POST", [](vogro::Context& ctx) {
        auto json = ctx.request->GetJsonTree();
        auto name = json.get<std::string>("name");
        ctx.response->addBody(name);
        return;
    });

    server.customErrorHandler(404, [](vogro::Context& ctx) {
        ctx.response->addBody("<h1>Custom Not Found</h1>");
        return;
    });

    server.runServer();
    return 0;
}
