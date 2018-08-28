#include "http_server.hpp"
// #include "handler.hpp"

int main() {
    // HTTP 服务运行在 12345 端口，并启用四个线程
    vogro::Server server(12345,4);
    // server.user_resource["/"]["GET"]=[](std::ostream& response, vogro::Request& request){
    //      response << "HTTP/1.1 200 OK\r\nContent-Length: " << "\r\n\r\n";
    // };
    server.addRoute("/","GET",[](vogro::Response& response, vogro::Request& request){
        
        // std::cout<<"hello,vogro!"<<std::endl;
        response.addBody("ff");
        // std::cout<<"in handler"<<std::endl;
        return;
    });

    server.customErrorHandler(404,[](vogro::Response& response, vogro::Request& request){
        response.addBody("not found ....");
        return;
    });
    
    server.runServer();
    return 0;
}