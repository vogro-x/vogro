If the requestd URL is `http://xxx.xx/xx/xx/?name=demoName&id=demoId`
``` C++
auto name = ctx.request->getQueryParam("name");
auto id =  ctx.request->getQueryParam("id");
```
the `name` is string `demoName` and `id` is string `demoId`.

If the query string parameter does not exist, then an empty string is returned. Such as the following:

the requested URL is `http://xx.xx/xx/?name=demoName`
``` C++
auto id = ctx.request->getQueryParam("id");  // id ==""
```

