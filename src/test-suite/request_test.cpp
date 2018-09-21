#include <iostream>
#include "request.hpp"
int main(void){
    vogro::Request r("GET","/a/b/c/");
   auto s = r.withQuery("id","123")
             .withQuery("name","andrewpqc")
             .withHeader("Host","localhost")
             .withHeader("VVV","JJJ")
             .withBody("{\"id\":\"123\",\"name\":\"andrewpqc\"}")
             .withBasicAuth("andrew","12345")
             .Expect().Header().Contains("A","aaaa");
//    std::cout<<s<<std::endl;
   return 0; 
}