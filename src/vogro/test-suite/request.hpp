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
        std::string ip;
        std::string port;

        std::string method;
        const std::string &pathTpl;
        std::string requestURL;


        std::map <std::string, std::string> headers;
        std::map <std::string, std::string> resHeaders;
        std::map <std::string, std::string> queryParams;
        std::map <std::string, std::string> pathParams;

        std::string body;
        std::shared_ptr <ip::tcp::socket> &socket;

        std::string getBaseURL(std::string tplURL,
                               std::map <std::string, std::string> pathParam) {
            auto length = tplURL.length();
            std::string realPath;
            for (int i = 0; i < length; i++) {
                if (tplURL[i] == '{') {
                    std::string key;
                    for (int j = i + 1; j < length; j++)
                        if (tplURL[j] == '}') {
                            auto got = pathParam.find(key);
                            if (got == pathParam.end()) throw "unknow key" + key;
                            else { realPath += got->second; i = j; break; }
                        } else key += tplURL[j];
                } else realPath += tplURL[i];
            }

            if (realPath.back() != '/') realPath += '/';

            return realPath;
        }

        std::string getRequestURL(std::map <std::string, std::string> queryParameters) {
            std::stringstream pathStream;

            pathStream << getBaseURL(this->pathTpl, this->pathParams);

            if (!queryParameters.empty()) pathStream << "?";

            for (auto it : queryParameters)
                pathStream << it.first << "=" << it.second << "&";

            auto finalPath = pathStream.str();
            if (finalPath.back() == '&') finalPath.pop_back();

            return finalPath;
        }


        std::string makeRequestMessage() {
            // HTTP request must contain the Host header
            this->headers["Host"] = this->ip + ":" + this->port;;

            auto reqURL = this->getRequestURL(this->queryParams);
            this->requestURL = reqURL;

            std::stringstream ss;
            ss << this->method << " " << url_encode(reqURL) << " HTTP/1.1\r\n";

            for (auto header : this->headers)
                ss << header.first << ": " << header.second << "\r\n";


            ss << "Content-Length: " << this->body.length() << "\r\n";
            ss << "\r\n";
            ss << this->body;
            return ss.str();
        }


    public:
        Request(const std::string mthd, const std::string &path,
                std::shared_ptr <boost::asio::ip::tcp::socket> &sock,
                const std::string serverIP, const std::string serverPort)
                : method(mthd), pathTpl(path), socket(sock),
                ip(serverIP), port(serverPort) {}

        Request &withQuery(const std::string &key, const std::string &val) {
            this->queryParams[key] = val;
            return *this;
        }

        Request &withBasicAuth(const std::string &username, const std::string &password) {
            auto basicAuthString = username + ":" + password;
            auto encodedString = "Basic " + base64_encode((unsigned char *) (basicAuthString.c_str()),
                    basicAuthString.length());
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

        Response &Expect() {

            boost::asio::streambuf requestBuffer;
            std::ostream requestStream(&requestBuffer);
            requestStream << this->makeRequestMessage();
            boost::asio::write(*socket, requestBuffer);

            boost::asio::streambuf responseBuffer;
            boost::asio::read_until(*socket, responseBuffer, "\r\n");
            std::istream responseStream(&responseBuffer);

            std::string httpVersion, statusMessage;
            int statusCode;

            responseStream >> httpVersion;
            responseStream >> statusCode;
            std::getline(responseStream, statusMessage);

            auto bytesTransferred = boost::asio::read_until(*socket, responseBuffer, "\r\n\r\n");

            auto total = responseBuffer.size();

            std::string header;
            while (std::getline(responseStream, header) && header != "\r") {
                auto kvPairHeader = parse_header(header);
                resHeaders[kvPairHeader.first] = kvPairHeader.second;
            }


            auto bodyLength = (resHeaders.find("Content-Length") == resHeaders.end())
                                ? "0" : resHeaders["Content-Length"];
            auto Length = std::stoull(bodyLength);
            auto additionalBytesNum = total - bytesTransferred;
            auto remain = Length - additionalBytesNum;
            if (remain > 0) {
                boost::system::error_code error;
                boost::asio::read(*socket, responseBuffer, boost::asio::transfer_exactly(remain), error);
            }

            std::stringstream mybody;
            mybody << &responseBuffer;
            std::string bodyString = mybody.str();

            auto res = std::make_shared<Response>(bodyString, resHeaders, this->method, this->requestURL);
            res->code = statusCode;
            return *res;
        }
    };
} // namespace vogro
#endif
