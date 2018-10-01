we have a lot of interfaces to help users to register handlers:

``` C++
server.addRoute()
server.Get()
server.Put()
server.Post()
server.Delete()

group = server.makeGroup()
group.Get()
group.Post()
group.Put()
group.Delete()
```