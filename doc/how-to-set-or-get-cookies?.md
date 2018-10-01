``` C++
server.Get("/setcookie/", [](vogro::Context &ctx) {
    ctx.response->setCookie("id", "123456", "/", "true", "20");
    ctx.response->addBody("ok");
});

server.Get("/getcookie/", [](vogro::Context &ctx) {
    auto cookie = ctx.request->getCookie();
    ctx.response->addBody(cookie);
});
```