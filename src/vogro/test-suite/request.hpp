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

#include "utils.hpp"
#include "response.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

using namespace boost::asio;

namespace vogro {

    class Request {
    private:
        const std::string ip;
        const std::string port;

        const std::string method;
        const std::string &pathTpl; // MUST reference
        std::string final_path;


        std::map <std::string, std::string> headers;
        std::map <std::string, std::string> resHeaders;
        std::map <std::string, std::string> queryParams;
        std::map <std::string, std::string> pathParams;

        // std::stringstream body_stream;
        std::string body;
        std::shared_ptr <ip::tcp::socket> &socket;

        std::string getFinalPath() {
            std::stringstream pathStream;
            auto realPath = get_real_path(this->pathTpl, this->pathParams);
            pathStream << realPath;

            if (this->queryParams.size() != 0) {
                pathStream << "?";
            }

            for (auto it : queryParams)
                pathStream << it.first << "=" << it.second << "&";

            auto finalPath = pathStream.str();
            if (finalPath.back() == '&')
                finalPath.pop_back();

            return finalPath;
        }

        std::string get_real_path(const std::string &originPath,
                                  const std::map <std::string, std::string> &pathParam) {
            auto length = originPath.length();
            std::string realPath;
            for (int i = 0; i < length; i++) {
                if (originPath[i] == '{') {
                    std::string key;
                    for (int j = i + 1; j < length; j++) {
                        if (originPath[j] == '}') {
                            auto got = pathParam.find(key);
                            if (got == pathParam.end()) {
                                std::cout << "Fatal: Lock of path Parameter " << key << std::endl;
                                exit(1);
                            } else {
                                realPath += got->second;
                                i = j;
                                break;
                            }
                        } else {
                            key += originPath[j];
                        }
                    }
                } else {
                    realPath += originPath[i];
                }
            }

            if (realPath.back() != '/')
                realPath += '/';
            return realPath;
        }

    public:
        Request(const std::string &mthd, const std::string &p,
                std::shared_ptr <boost::asio::ip::tcp::socket> &sock,
                const std::string &serverIP, const std::string &serverPort)
                : method(mthd), pathTpl(p), socket(sock), ip(serverIP), port(serverPort) {
        }

        Request &withQuery(const std::string &key, const std::string &val) {
            this->queryParams[key] = val;
            return *this;
        }

        Request &withBasicAuth(const std::string &username, const std::string &password) {
            auto basicAuthString = username + ":" + password;
            auto encodedString =
                    "Basic " + base64_encode((unsigned char *) (basicAuthString.c_str()), basicAuthString.length());
            return this->withHeader("Authorization", encodedString);
        }

        Request &withPath(const std::string &key, const std::string &val) {
            this->pathParams[key] = val;
            return *this;
        }

        Request &withJSON(const std::string &bd) {
            this->body = bd;
            this->headers["Content-Type"] = "application/json";
            return *this;
        }

        Request &withHeader(const std::string &key, const std::string &val) {
            this->headers[key] = val;
            return *this;
        }

        std::string makeRequestMessage() {
            // HTTP request must contain the Host header
            auto host_str = this->ip + ":" + this->port;
            this->headers["Host"] = host_str;

            auto fp = this->getFinalPath();
            this->final_path = fp;

            std::stringstream ss;
            ss << this->method << " " << url_encode(fp) << " HTTP/1.1\r\n";

            for (auto h : this->headers) {
                ss << h.first << ": " << h.second << "\r\n";
            }


            ss << "Content-Length: " << this->body.length() << "\r\n";
            ss << "\r\n";
            ss << this->body;
            return ss.str();
        }

        Response &Expect() {

            boost::asio::streambuf request_buffer;
            std::ostream request_stream(&request_buffer);
            request_stream << this->makeRequestMessage();
            boost::asio::write(*socket, request_buffer);

            boost::asio::streambuf response_buffer;
            boost::asio::read_until(*socket, response_buffer, "\r\n");
            std::istream response_stream(&response_buffer);

            std::string http_version;
            int status_code;
            std::string status_message;

            response_stream >> http_version;
            response_stream >> status_code;
            std::getline(response_stream, status_message);

            auto bytes_transfered = boost::asio::read_until(*socket, response_buffer, "\r\n\r\n");

            auto total = response_buffer.size();

            std::string header;
            while (std::getline(response_stream, header) && header != "\r") {
                auto parse_result = parse_header(header);
                resHeaders[parse_result.first] = parse_result.second;
            }


            auto bodyLength = (resHeaders.find("Content-Length") == resHeaders.end()) ? "0" : resHeaders["Content-Length"];
            auto Length = std::stoull(bodyLength);
            auto additional_bytes_num = total - bytes_transfered;
            auto remain = Length - additional_bytes_num;
            if (remain > 0) {
                boost::system::error_code error;
                boost::asio::read(*socket, response_buffer, boost::asio::transfer_exactly(remain), error);
            }

            std::stringstream mybody;
            mybody << &response_buffer;
            std::string bodyString = mybody.str();

            auto res = std::make_shared<Response>(bodyString, resHeaders, this->method, this->final_path);
            res->code = status_code;
            return *res;
        }
    };
} // namespace vogro
#endif
