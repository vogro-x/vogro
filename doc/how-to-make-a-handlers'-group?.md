Sometimes we need to organize multiple handlers into a group, they have the same URL prefix, and there may be global middleware in the group.
`Context::makeGroup()` can help us with this feature.

``` C++
void GroupGlobalMiddleWare(vogro::Context & ctx) {
    ctx.response->addBody("hello from group global handler<br>");
    ctx.Next();
}

void TestHandler1(vogro::Context& ctx) {
    ctx.response->addBody("hello from handler1<br>");
    ctx.Next();
}

void TestHandler2(vogro::Context& ctx) {
    ctx.response->addBody("hello from handler2<br>");
    ctx.Next();
}

void TestHandler3(vogro::Context& ctx) {
    ctx.response->addBody("hello from handler3<br>");
}

void PostHandler(vogro::Context & ctx) {
    ctx.response->addBody("post");
}

void PutHandler(vogro::Context & ctx) {
    ctx.response->addBody("put");
}

void DeleteHandler(vogro::Context & ctx) {
    ctx.response->addBody("delete");
}

auto userGroup = server.makeGroup("/group", GroupGlobalMiddleWare);
{
    userGroup->Get("/get", TestHandler1, TestHandler2, TestHandler3);
    userGroup->Post("/post", PostHandler);
    userGroup->Put("/put", PutHandler);
    userGroup->Delete("/delete", DeleteHandler);
}
```
`/group/get/` --> GroupGlobalMiddleWare --> TestHandler1 --> TestHandler2 --> TestHandler3

`/group/post/` --> GroupGlobalMiddleWare --> PostHandler

`/group/put/` --> GroupGlobalMiddleWare --> PutHandler

`/group/delete/` --> GroupGlobalMiddleWare --> DeleteHandler