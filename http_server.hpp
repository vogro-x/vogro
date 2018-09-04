#ifndef _HTTP_SERVER_HPP
#define	_HTTP_SERVER_HPP

#include <iostream>
#include "base_server.hpp"
#include "common.hpp"
#include "status_code.hpp"

namespace vogro {

    typedef boost::asio::ip::tcp::socket HTTP;

//    template <>
    class Server : public ServerBase<HTTP> {
    public:
        Server(unsigned short port=8080, size_t num_threads=4):
        ServerBase<HTTP>::ServerBase(port, num_threads) {

            default_error_handler = [](vogro::Response& response, vogro::Request& request){
                auto code = response.getCode();
                vogro::StatusCodeMap& codeMap = vogro::StatusCodeMap::GetInstance();
                auto pharse = codeMap.getPharseByCode(code);
                std::stringstream ss;
                ss << "<center><h1>"<<code<<" "<<pharse<<"</h1><br/>";
                ss << "<a href='https:://github.com/Andrewpqc/vogro'>vogro("
                   << 0.1<<")</a></center>";
                response.addBody(ss.str());
                return;
            };
        }
    private:
        void accept() {
            // std::cout<<"accept"<<std::endl;
            auto socket = std::make_shared<HTTP>(io_svc);
            acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& ec) {
                accept();//递归调用accept
                if(!ec) process_request_and_respond(socket);
            });
        }
    };
}
#endif