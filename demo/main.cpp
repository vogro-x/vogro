#include "./handler/handlers.hpp"
#include "./middleware/middleware.hpp"

#include "../src/server/http.hpp"
#include "../src/json.hpp" // for start http server
// #include "https.hpp" //for start https server

int main()
{
    vogro::Server server(8080, 4);
    // vogro::Server server(8080,4,"../secret/server.crt","../secret/server.key");

    server.Use(TestMiddleWare1);
    server.Use(TestMiddleWare2);

    server.addRoute("/", "GET", [](vogro::Context& ctx) {
        ctx.response->addBody("<h1>Index Page</h1>");
        return;
    });

    server.addRoute("/username/{str:name}/", "GET", [](vogro::Context& ctx) {
        auto name = ctx.request->getPathParam("name");
        auto key = ctx.getValue("key");
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
        auto json = ctx.request->ReadJSON();
        auto name = json.at("name").get<std::string>();
        ctx.response->addBody(name);
        return;
    });

    server.onError(404, [](vogro::Request& request, vogro::Response& response) {
        response.addBody("<h1>Custom Not Found</h1>");
        return;
    });

    auto userGroup = server.makeGroup("/user", [](vogro::Context& ctx) {
        ctx.response->addBody("hello from group global handler<br>");
        ctx.Next(); 
    });
    {
        userGroup->Get("/get", TestHandler1, TestHandler2, TestHandler3);

        userGroup->Post("/post", [](vogro::Context& ctx) {
            ctx.response->addBody("post");
        });

        userGroup->Put("/put", [](vogro::Context& ctx) {
            ctx.response->addBody("put");
        });

        userGroup->Delete("/delete", [](vogro::Context& ctx) {
            ctx.response->addBody("delete");
        });
    }


    server.Get("/ggg",TestHandler1,TestHandler2,TestHandler3);

    server.Get("/setcookie/",[](vogro::Context& ctx){
        ctx.response->setCookie("id","123456","/","true","20");
        ctx.response->addBody("ok");
    });

    server.Get("/getcookie/",[](vogro::Context& ctx){
        auto cookie = ctx.request->getCookie();
        ctx.response->addBody(cookie);
    });


    server.Get("/auth/", [](vogro::Context& ctx ){
        auto auth =ctx.request->basicAuth();
        ctx.response->addBody(auth.first);
        ctx.response->addBody("|");
        ctx.response->addBody(auth.second);

    });

    server.Get("/redict",[](vogro::Context& ctx){
        auto url = ctx.request->getQueryParam("url");
        ctx.response->redirect(url);
    });

    server.Get("/json/",[](vogro::Context& ctx){
        nlohmann::json j;
        j["id"] =1;
        j["name"] = "andrew";
        ctx.response->writeJSON(j);
    });
    
    server.runServer();
    return 0;
}
