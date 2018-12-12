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

#ifndef __HTTPS_SERVER_HPP__
#define __HTTPS_SERVER_HPP__


// sudo apt-get install libssl-dev

#include "base.hpp"
#include <boost/asio/ssl.hpp>

namespace vogro {

    typedef boost::asio::ssl::stream <boost::asio::ip::tcp::socket> HTTPS;


    class Server : public ServerBase<HTTPS> {
    public:
        Server(unsigned short port, size_t num_threads,
               const std::string &cert_file, const std::string &private_key_file) : ServerBase<HTTPS>::ServerBase(port, num_threads),
                                                                                    context(boost::asio::ssl::context::sslv23) {
            context.use_certificate_chain_file(cert_file);
            context.use_private_key_file(private_key_file, boost::asio::ssl::context::pem);
        }

    private:
        boost::asio::ssl::context context;

        void accept() {

            auto socket = std::make_shared<HTTPS>(io_svc, context);

            acceptor.async_accept(
                    (*socket).lowest_layer(),
                    [this, socket](const boost::system::error_code &ec) {

                        accept();

                        if (!ec) {
                            (*socket).async_handshake(boost::asio::ssl::stream_base::server,
                                                      [this, socket](const boost::system::error_code &ec) {
                                                          if (!ec) process_request_and_respond(socket);
                                                      });
                        }
                    });
        }
    };
}


#endif