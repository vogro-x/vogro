#include "./handler/handlers.hpp"
#include "./middleware/middleware.hpp"

#include "../src/server/http.hpp"
#include "../src/server/json.hpp" // for start http server
// #include "../src/server/https.hpp" //for start https server

#include "../src/server/template_engine.hpp"

int main() {
    vogro::Server server(8080, 4);
    // server.InitLogger("file","debug","hhh.log");
    // vogro::Server server(8080,4,"../cert/server.crt","../cert/server.key");

    server.Use(TestMiddleWare1);
    server.Use(TestMiddleWare2);

    server.addRoute("/", "GET", [](vogro::Context &ctx) {
        ctx.response->addBody("<h1>Index Page</h1>");
        return;
    });

    server.addRoute("/username/{str:name}/", "GET", [](vogro::Context &ctx) {
        auto name = ctx.request->getPathParam("name");
        auto key = ctx.getValue("key");
        ctx.response->addBody(name);
        ctx.response->addBody(key);
        return;
    });

    server.addRoute("/course/{str:coursename}/user/{int:id}/", "GET", [](vogro::Context &ctx) {
        auto coursename = ctx.request->getPathParam("coursename");
        auto id = ctx.request->getPathParam("id");
        ctx.response->addBody("coursename:" + coursename + ",id: " + id);
        return;
    });


    server.addRoute("/a/post/", "POST", [](vogro::Context &ctx) {
        auto json = ctx.request->ReadJSON();
        auto name = json.at("name").get<std::string>();
        ctx.response->addBody(name);
        return;
    });

    server.onError(404, [](vogro::Request &request, vogro::Response &response) {
        response.addBody("<h1>Custom Not Found</h1>");
        return;
    });

    auto userGroup = server.makeGroup("/user/", [](vogro::Context &ctx) {
        ctx.response->addBody("hello from group global handler<br>");
        ctx.Next();
    });
    {
        userGroup->Get("/get", TestHandler1, TestHandler2, TestHandler3);

        userGroup->Post("/post", [](vogro::Context &ctx) {
            ctx.response->addBody("post");
        });

        userGroup->Put("/put", [](vogro::Context &ctx) {
            ctx.response->addBody("put");
        });

        userGroup->Delete("/delete", [](vogro::Context &ctx) {
            ctx.response->addBody("delete");
        });
    }


    server.Get("/ggg", TestHandler1, TestHandler2, TestHandler3);

    server.Get("/setcookie/", [](vogro::Context &ctx) {
        ctx.response->setCookie("id", "123456", "/", "true", "20");
        ctx.response->addBody("ok");
    });

    server.Get("/getcookie/", [](vogro::Context &ctx) {
        auto cookie = ctx.request->getCookie();
        ctx.response->addBody(cookie);
    });


    server.Get("/auth/", [](vogro::Context &ctx) {
        auto auth = ctx.request->basicAuth();
        ctx.response->addBody(auth.first);
        ctx.response->addBody("|");
        ctx.response->addBody(auth.second);

    });

    server.Get("/redict", [](vogro::Context &ctx) {
        auto url = ctx.request->getQueryParam("url");
        ctx.response->redirect(url);
    });

    server.Get("/json/", [](vogro::Context &ctx) {
        nlohmann::json j;
        j["id"] = 1;
        j["name"] = "andrew";
        ctx.response->writeJSON(j);
    });

    server.Get("/tpl/{str:name}/", [](vogro::Context &ctx) {
        auto name = ctx.request->getPathParam("name");
        vtpl::Environment env;
        env["name"] = name;
        ctx.response->renderStringTemplate("<h1>Hello, {{ name }} </h1>", env);
    });

    server.Get("/tpl/file/",[](vogro::Context & ctx){
       vtpl::Environment env;
       env["obj"]["title"] = "this is title";
       env["obj"]["content"] = "this is content";
       env["obj"]["foot1"] = "this is foot1";
       env["obj"]["foot2"] = "this is foot2";
       env["obj"]["show1"] = true;
       env["obj"]["show2"] = false;
       ctx.response->renderFileTemplate("../templates/example.html",env);

    });

    server.Get("/hhh", [](vogro::Context &ctx) {
        ctx.response->addBody("hello");
    });

    auto g=server.makeGroup("/group");
    {
        g->Get("/get",[](vogro::Context & ctx){
            ctx.response->addBody("/group/get");
        });
    }
    server.run();
    return 0;
}
