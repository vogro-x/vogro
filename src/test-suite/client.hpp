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

#include "../request.hpp"
#include <iostream>
#include <string>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
namespace vogro {

class VogroTestClient {
private:
    std::string serverIP;
    std::string serverPort;
    boost::asio::io_service io_service;

    tcp::resolver resolver(io_service);
    tcp::resolver::query query;
    tcp::resolver::iterator endpoint_iterator;
    tcp::socket socket;

public:
    VogroTestClient(const std::string& ip,const::std::string port):serverIP(ip),serverPort(port){

        this->query = tcp::resolver::query(this->serverIP,this->serverPort);
        this->endpoint_iterator =this->resolver.resolve(this->query);
        this->socket =tcp::socket(this->io_service);
    }
    void Get(std::string path){
        boost::asio::connect(this->socket, this->endpoint_iterator);
        Request r;
        

    }
    void POST();
    void PUT();
    void DELETE();


};
}  //namespace vogro

#endif