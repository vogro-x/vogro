#include <iostream>
#include "request.hpp"
int main(void){
    vogro::Request r("GET","/a/b/c/");
    // test Status
    r.withQuery("id","123")
             .withQuery("name","andrewpqc")
             .withHeader("Host","localhost")
             .withHeader("VVV","JJJ")
             .withBody("{\"id\":\"123\",\"name\":\"andrewpqc\"}")
             .withBasicAuth("andrew","12345")
             .Expect().Status(200);

    // test Header
    r.withQuery("id","123")
             .withQuery("name","andrewpqc")
             .withHeader("Host","localhost")
             .withHeader("VVV","JJJ")
             .withBody("{\"id\":\"123\",\"name\":\"andrewpqc\"}")
             .withBasicAuth("andrew","12345")
             .Expect().Header().Contains("A","aaaaa").NotContains("A","aaaa");
    
    // test body
    r.withQuery("id","123")
             .withQuery("name","andrewpqc")
             .withHeader("Host","localhost")
             .withHeader("VVV","JJJ")
             .withBody("{\"id\":\"123\",\"name\":\"andrewpqc\"}")
             .withBasicAuth("andrew","12345")
             .Expect().Status(200).Body().Contains("test body");
//    std::cout<<s<<std::endl;
   return 0; 
}