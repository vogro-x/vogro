To register middlewares, use the `server.Use()` interface. the following is an example:
```
server.Use(TestMiddleWare1);
server.Use(TestMiddleWare2);
```
register middlewares like this, then the middleware will be applied globally. If you want to register a middleware to be applied internal group globally, you can refer to [this page](https://github.com/vogro-x/vogro/wiki/how-to-make-a-handlers'-group%3F)

