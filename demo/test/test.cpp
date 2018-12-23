#include "vogro/test-suite/client.hpp"

int main() {
    auto client = std::make_shared<vogro::VogroTestClient>("127.0.0.1", "8080");

    client->Get("/")->Expect()->Status(200)->Body()->Contains("Index")->Equal("<h1>Index Page</h1>");

    client->Get("/username/{name}")->withPath("name", "Andrewpqc")->Expect()->
            Body()->Contains("Andrewpqc");

    client->Get("/username/{name}")->withPath("name", "阿超")->Expect()->
            Body()->Contains("阿超");

    client->Get("/course/{coursename}/user/{id}/")->withPath("coursename", "A1310B")->
            withPath("id", "123")->withQuery("nn", "123")->Expect()->Body()->Contains("A1310B");

    client->Get("/user/get/")->Expect()->Status(200)->Body()->Contains("hello");

    client->Post("/user/post/")->Expect()->Status(200)->Body()->Contains("post");

    client->Put("/user/put/")->Expect()->Status(200)->Body()->Contains("put");

    client->Delete("/user/delete/")->Expect()->Status(200)->Body()->Contains("delete");

    client->Get("/auth/")->withBasicAuth("andrew", "12345")->Expect()->Body()->Contains("andrew");

    client->Get("/json/")->Expect()->Body()->Contains("\"name\":\"andrew\"");

    client->Post("/a/post/")->withJSON("{\"name\":\"Andrewpqc\"}")->Expect()->
            Body()->Contains("Andrewpqc");

    return 0;
}
