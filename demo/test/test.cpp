
#include "../../src/test-suite/client.hpp"

//1.3 ms pre request ----> sync client 
int main(void) {
    vogro::VogroTestClient client("127.0.0.1", "8080");

    for (int i = 0; i < 1; i++) {

        client.Get("/").Expect().Body().Contains("Page");

        client.Get("/username/{name}").withPath("name","Andrewpqc").Expect().
            Body().Contains("Andrewpqc");

        client.Get("/course/{coursename}/user/{id}/").withPath("coursename","A1310B").
            withPath("id","123").Expect().Body().Contains("A1310B");

        client.Get("/user/get/").Expect().Status(200).Body().Contains("hello");

        client.Post("/user/post/").Expect().Status(200).Body().Contains("post");

        client.Put("/user/put/").Expect().Status(200).Body().Contains("put");

        client.Delete("/user/delete/").Expect().Status(200).Body().Contains("delete");

        client.Get("/auth/").withBasicAuth("andrew", "12345").Expect().Body().Contains("andrew");

        client.Get("/json/").Expect().Body().Contains("\"name\":\"andrew\"");

    }
    return 0;
}