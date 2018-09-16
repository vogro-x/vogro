/************************************************************************
 Copyright 2018 andrewpqc@mails.ccnu.edu.cn
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 ************************************************************************/

#ifndef _HTTP_SERVER_HPP
#define _HTTP_SERVER_HPP

#include <iostream>
#include "base.hpp"
#include "common.hpp"
#include "status.hpp"

namespace vogro {

typedef boost::asio::ip::tcp::socket HTTP;

//    template <>
class Server : public ServerBase<HTTP> {
   public:
    Server(unsigned short port = 8080, size_t num_threads = 4)
        : ServerBase<HTTP>::ServerBase(port, num_threads) {
        default_error_handler = [](vogro::Context& ctx) {
            auto code = ctx.response->getCode();
            vogro::StatusCodeMap& codeMap = vogro::StatusCodeMap::GetInstance();
            auto pharse = codeMap.getPharseByCode(code);
            std::stringstream ss;
            ss << "<center><h1>" << code << " " << pharse << "</h1><br/>";
            ss << "<a href='https:://github.com/Andrewpqc/vogro'>vogro(" << 0.1
               << ")</a></center>";
            ctx.response->addBody(ss.str());
            return;
        };
    }

   private:
    void accept() {
        std::cout << "accept a new connection............." << std::endl;
        auto socket = std::make_shared<HTTP>(io_svc);
        acceptor.async_accept(
            *socket, [this, socket](const boost::system::error_code& ec) {
                accept();  //递归调用accept
                if (!ec) process_request_and_respond(socket);
            });
    }
};
}  // namespace vogro
#endif