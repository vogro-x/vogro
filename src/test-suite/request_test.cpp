#include <iostream>
#include "client.hpp"
int main(void){
    vogro::VogroTestClient client("127.0.0.1","8080");

    client.Get("/").withQuery("id","123")
             .withQuery("name","andrewpqc")
             .withHeader("Host","localhost")
             .withHeader("VVV","JJJ")
             .withBody("{\"id\":\"123\",\"name\":\"andrewpqc\"}")
             .withBasicAuth("andrew","12345")
             .Expect().Status(200);

    

   return 0;
}