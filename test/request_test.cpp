// the test file of Request class.
#include "../request.hpp"
#include <string>

int main(void) {
    auto request = std::make_shared<vogro::Request>();
    request->setMethod("GET");
    std::cout << request->getMethod() << std::endl;

    request->setHost("120.77.220.239");
    std::cout << request->getHost() << std::endl;

    request->setFormParam("a=1&b=2&name=Andrewpqc");
    std::cout << request->getFormParam("name") << std::endl;

    request->setPath("/abc/def/2/gg");
    std::cout << request->getPath() << std::endl;

    request->setProtocol("http");
    std::cout << request->getProtocol() << std::endl;

    request->setRemoteIP("119.23.154.25");
    std::cout << request->getRemoteIP() << std::endl;

    request->setRemotePort(80);
    std::cout << request->getRemotePort() << std::endl;

    request->setQueryParam("id=2&name=andrewpqc");
    std::cout << request->getQueryParam("id") << std::endl;

    request->setVersion("1.1");
    std::cout << request->getVersion() << std::endl;

    request->addPathParam("id", "1000");
    std::cout << request->getPathParam("id");

    request->addHeader("Host", "127.0.0.1");
    request->addHeader("Accept", "text/html,application/xhtml+xm…plication/xml;q=0.9,*/*;q=0.8");

    std::cout << request->getHeader("Accept") << std::endl;

    auto headers = request->getHeaders();

    for (auto header : headers) {
        std::cout << header.first << ":" << header.second << std::endl;
    }

    return 0;
}