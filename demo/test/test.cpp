
#include "../../src/test-suite/client.hpp"

//1.3 ms pre request ----> sync client 
int main(void)
{
    vogro::VogroTestClient client("127.0.0.1", "8080");

    for (int i = 0; i < 10000; i++) {

        client.Get("/").Expect().Body().Contains("Page");

        client.Get("/user/get/").Expect().Status(200).Body().Contains("hello");

        client.POST("/user/post/").Expect().Status(200).Body().Contains("post");

        client.PUT("/user/put/").Expect().Status(200).Body().Contains("put");

        client.DELETE("/user/delete/").Expect().Status(200).Body().Contains("delete");

        client.Get("/auth/").withBasicAuth("andrew", "12345").Expect().Body().Contains("andrew");

        client.Get("/json/").Expect().Body().Contains("\"name\":\"andrew\"");
    }
    return 0;
}