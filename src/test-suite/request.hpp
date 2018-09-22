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
#ifndef __VOGRO_TEST_REQUEST_HPP__
#define __VOGRO_TEST_REQUEST_HPP__
#include "../utils.hpp"
#include "response.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
namespace vogro {
class Request {
private:
    std::string method;
    std::string path;
    std::map<std::string, std::string> queryParams;
    std::vector<std::pair<std::string, std::string>> headers;
    std::map<std::string, std::string> pathParams;
    std::stringstream body;

    std::shared_ptr<boost::asio::ip::tcp::socket>& socket;

public:
    Request(const std::string& mtd, const std::string& p,
            std::shared_ptr<boost::asio::ip::tcp::socket>& sock)
        : method(mtd)
        , path(p)
        , socket(sock)
        {
        }

    Request& withQuery(const std::string& key, const std::string& val)
    {
        this->queryParams[key] = val;
        return *this;
    }

    Request& withBasicAuth(const std::string& username, const std::string& password)
    {
        auto basicAuthString = username + ":" + password;
        auto encodedString = "Basic " + base64_encode((unsigned char*)(basicAuthString.c_str()), basicAuthString.length());
        return this->withHeader("Authorization", encodedString);
    }

    Request& withPath(const std::string& key, const std::string& val)
    {
        this->pathParams[key] = val;
        return *this;
    }

    Request& withBody(const std::string& bd)
    {
        this->body << bd;
        return *this;
    }

    Request& withHeader(const std::string& key, const std::string& val)
    {
        auto header = std::make_pair(key, val);
        this->headers.push_back(header);
        return *this;
    }

    std::string getFinalPath()
    {
        std::stringstream pathStream;
        if (this->path.back() != '/')
            this->path += '/';

        pathStream << this->path;

        if (this->queryParams.size() != 0){
            pathStream << "?";
        }

        for (auto it = queryParams.begin(); it != queryParams.end(); it++)
            pathStream << it->first << "=" << it->second << "&";

        auto finalPath = pathStream.str();
        if (finalPath.back() == '&')
            finalPath.pop_back();
        return finalPath;
    }

    std::string makeRequestMessage()
    {
        std::stringstream ss;
        ss << this->method << " " << this->getFinalPath() << " HTTP/1.1\r\n";
        for (auto h : this->headers) {
            ss << h.first << ": " << h.second << "\r\n";
        }
        ss << "\r\n";
        ss << this->body.str();
        return ss.str();
    }

    Response& Expect()
    {   
        boost::asio::streambuf request_buffer;
        std::ostream request_stream(&request_buffer);
        request_stream << this->makeRequestMessage();
        boost::asio::write(*socket, request_buffer);
         
        boost::asio::streambuf response_buffer;
        boost::asio::read_until(*socket, response_buffer, "\r\n");
        std::istream response_stream(&response_buffer);

        auto res = std::make_shared<Response>();

        std::string http_version;
        int status_code;
        std::string status_message;

        response_stream >> http_version;
        response_stream >> status_code;
        std::getline(response_stream, status_message);
        res->code = status_code;


        auto bytes_transfered = boost::asio::read_until(*socket, response_buffer, "\r\n\r\n");

        auto total = response_buffer.size();

        std::string header;
        while (std::getline(response_stream, header) && header != "\r"){
            auto parse_result = parse_header(header);
            res->headers[parse_result.first] = parse_result.second;
        }
        
        auto bodyLength = (res->headers.find("Content-Length")== res->headers.end()) ? "0" :res->headers["Content-Length"];
        auto Length = std::stoull(bodyLength);
        auto additional_bytes_num = total -bytes_transfered;
        auto remain =Length - additional_bytes_num;
        std::cout<<remain<<std::endl;
        if(remain>0) {
             boost::system::error_code error;
            boost::asio::read(*socket, response_buffer,boost::asio::transfer_exactly(remain), error);
        }
            std::stringstream mybody;
            mybody << &response_buffer;
            
            res->body = mybody.str();
            std::cout<<res->body<<std::endl<<std::endl;
        return *res;
    }
};
} // namespace vogro
#endif
