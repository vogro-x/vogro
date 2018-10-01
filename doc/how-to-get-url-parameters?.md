when we set up a dynamic URL pattern, We may want to get the dynamic part of the requested URL in our handlers or middlewares.

This part we will tell you how to get the URL parameters.

``` c++
server.addRoute("/username/{str:name}/", "GET", [](vogro::Context &ctx) {
    auto name = ctx.request->getPathParam("name");
    ctx.response->addBody(name);
    return;
  });
```
As the code shows, we can use `ctx.request->getPathParam()` to get the URL parameters.