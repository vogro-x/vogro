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

#ifndef _RESPONSE_HPP_
#define _RESPONSE_HPP_

#include <ctime>
#include <iostream>
#include <sstream>  // std::stringstream
#include <unordered_map>

#include "common.hpp"
#include "status_code.hpp"

namespace vogro {
class Response {
   private:
    int code;

    std::string version;

    std::string phrase;

    std::unordered_map<std::string, std::string> headers;

    std::stringstream body;

    std::string getCurrentCMTTime() {
        time_t rawTime;
        struct tm *timeInfo;
        char szTemp[30] = {0};
        time(&rawTime);
        timeInfo = gmtime(&rawTime);
        strftime(szTemp, sizeof(szTemp), "%a, %d %b %Y %H:%M:%S GMT", timeInfo);
        return std::string(szTemp);
    }

   public:
    //构造函数
    Response() {
        // set default code,version,phare
        this->code = CodeOK_200;
        this->version = "1.1";
        this->phrase = "OK";

        // add Server header
        std::string server = "vorgo/";
        server += __VOGRO_VERSION__;
        server += ", https://github.com/Andrewpqc/vogro";
        this->addHeader("Server", server);

        // add Date header
        this->addHeader("Date", this->getCurrentCMTTime());
        // add Content-Type header
        this->addHeader("Content-Type", "text/html; charset=utf-8");
    }

    int getCode() { return this->code; }

    void setCode(int code) {
        this->code = code;
        vogro::StatusCodeMap &codeMap = vogro::StatusCodeMap::GetInstance();
        this->phrase = codeMap.getPharseByCode(code);
    }

    void setVersion(std::string vsn) { this->version = vsn; }

    void setPhrase(std::string phrase) {}

    void addHeader(std::string key, std::string val) { this->headers[key] = val;}

    void addBody(std::string cnt) { this->body << cnt; }

   
    std::stringstream & getResponseBodyStrem(){return body; }

    std::string  makeResponseMsg() {
        std::stringstream responseMsg;

        // add  response line
        responseMsg << "HTTP/" << this->version << " " << this->code << " "
                    << this->phrase << "\r\n";

        // add Content-Length header
        this->body.seekp(0, std::ios::end);

        // add Content-Length header
        std::stringstream ss;
        std::string s;
        ss << this->body.tellp();
        ss >> s;
        this->addHeader("Content-Length", s);

        // add other headers
        for (auto header : this->headers) {
            responseMsg << header.first << ": " << header.second << "\r\n";
        }

        // empty line
        responseMsg << "\r\n";
        responseMsg << this->body.rdbuf();
        return responseMsg.str();
    }
};
}  // namespace vogro

#endif
