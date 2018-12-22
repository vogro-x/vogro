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

#ifndef __VOGRO_TEST_CLIENT_HPP__
#define __VOGRO_TEST_CLIENT_HPP__

#include "request.hpp"
#include "utils.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <string>

using namespace boost::asio;

namespace vogro {

    class VogroTestClient {
    private:
        std::string serverIP;
        std::string serverPort;
        io_service ios;
        std::shared_ptr <ip::tcp::socket> socket = std::make_shared<ip::tcp::socket>(ios);

    public:
        VogroTestClient(const std::string &ip, const ::std::string &port) : serverIP(ip), serverPort(port) {

            ip::tcp::resolver::query resolver_query(this->serverIP, this->serverPort,
                    ip::tcp::resolver::query::numeric_service);

            ip::tcp::resolver resolver(ios);

            try {
                ip::tcp::resolver::iterator it = resolver.resolve(resolver_query);
                connect(*socket, it);
            } catch (const boost::system::system_error &e) {
                std::cout << e.what() << std::endl;
            }
        }

        ~VogroTestClient() {
            std::cout << INITCOLOR(BLUE_COLOR) << "Successed" << INITCOLOR(ZERO_COLOR) << std::endl;
        }


        std::shared_ptr<Request> Get(const std::string &path) {
            return std::make_shared<Request>("GET", path, this->socket, this->serverIP, this->serverPort);
        }
        // Request &Get(const std::string &path) {
        //     auto p = 
        //     return *p;
        // }

        std::shared_ptr<Request> Post(const std::string &path) {
            return std::make_shared<Request>("POST", path, this->socket, this->serverIP, this->serverPort);
        }
        // Request &Post(const std::string &path) {
        //     auto p = 
        //     return *p;
        // }


        std::shared_ptr<Request> Put(const std::string &path) {
            return std::make_shared<Request>("PUT", path, this->socket, this->serverIP, this->serverPort);
        }
        // Request &Put(const std::string &path) {
        //     auto p = 
        //     return *p;
        // }

        std::shared_ptr<Request> Delete(const std::string &path) {
            return std::make_shared<Request>("DELETE", path, this->socket, this->serverIP, this->serverPort);
        }
        // Request &Delete(const std::string &path) {
        //     auto p = std::make_shared<Request>("DELETE", path, this->socket, this->serverIP, this->serverPort);
        //     return *p;
        // }
    };
} //namespace vogro

#endif