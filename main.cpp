#include "http_server.hpp"

int main() {
  vogro::Server server(12345, 4);
  
  server.addRoute("/", "GET",[](vogro::Response &response, vogro::Request &request) {
            response.addBody("ff");
            return;
        });

  server.customErrorHandler(404, [](vogro::Response &response, vogro::Request &request) {
            response.addBody("not found ....");
            return;
      });

  server.runServer();
  return 0;
}
