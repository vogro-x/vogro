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
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace vogro {

class VogroTestClient {
private:
    std::string serverIP;
    std::string serverPort;
    boost::asio::io_service ios;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket = 
        std::make_shared<boost::asio::ip::tcp::socket>(ios);
   

public:
    VogroTestClient(const std::string& ip, const::std::string& port):serverIP(ip),serverPort(port){

        boost::asio::ip::tcp::resolver::query resolver_query(this->serverIP, this->serverPort,
            boost::asio::ip::tcp::resolver::query::numeric_service);

        boost::asio::ip::tcp::resolver resolver(ios);

        try {
            boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(resolver_query);
            boost::asio::connect(*socket, it);
        }catch (boost::system::system_error &e) {
            std::cout<<e.what()<<std::endl;
        }

    }

    Request& Get(std::string path){
        auto p = std::make_shared<Request>("GET",path, this->socket,this->serverIP,this->serverPort);
        return *p;
    }

    Request& Post(std::string path){
        auto p = std::make_shared<Request>("POST",path,this->socket,this->serverIP,this->serverPort);
        return *p;
    }

    Request& Put(std::string path){
        auto p = std::make_shared<Request>("PUT",path,this->socket, this->serverIP, this->serverPort);
        return *p;
    }
    
    Request& Delete(std::string path){
        auto p = std::make_shared<Request>("DELETE",path,this->socket, this->serverIP, this->serverPort);
        return *p;
    }


};
}  //namespace vogro

#endif