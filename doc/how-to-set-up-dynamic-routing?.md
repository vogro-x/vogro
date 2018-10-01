In interfaces such as `server.addRoute()`，`server.Get()`,`server.Post()`,`server.Put()`,`server.Delete()`, we need to describe the pattern of the URL requested by the user to match the corresponding handler and execute it.

## static URL pattern
We can declare a static URL pattern, which requires that the requested URL match exactly with the URL we declare to match successfully. The following are examples of static URL patterns:

``` 
/hello/world/
```
For the above URL pattern, only `/hello/world/` or `/hello/world` will match successfully，any other will fail to match.

```
/a/static/url/pattern
```
That is to say, the URL pattern of the declaration or the URL of the request can be added a tail-slash or not. So for the above URL pattern,`/a/static/url/pattern` or `/a/static/url/pattern/` can match successfully.

## dynamic URL pattern
In many cases, what we need is a dynamic URL pattern. such as the following:
```
/api/user/{name}/
```
```
/api/user/{name}/id/{id}/
```
In vogro, We support dynamic URL patterns. We can use the following syntax to declare it:
```
.../{flag:name}/.../{flag:name}/.../{name}/...
```
`flag` is a specific string, which indicates the type of the dynamic part.
For now, we support two flags: `int`,`str`.
`int` flag indicates that all characters of the dynamic part must be composed of numeric characters,`str` flag indicates that the dynamic part must be a string.
The Flag part can be leave out, and the default flag is `int`.

The following are examples of vogro dynamic URL patterns:
```
/user/{str:name}/
```
`/user/bob/`,`/user/linus/` can match successfully.

```
/comment/{str:title}/{int:id}/
```
`/comment/cc/112/`,`/comment/title/253/` can match successfully, but `/comment/title/12a/` cannot match successful.