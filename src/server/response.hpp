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
#include <sstream> // std::stringstream
#include <string>
#include <fstream>
#include <unordered_map>

#include "common.hpp"
#include "status.hpp"
#include "json.hpp"
#include "template_engine.hpp"

namespace vogro {

    class Response {
    private:
        int code = 200;

        std::string version= "1.1";

        std::string phrase="OK";

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
            // add Server header
            std::string server = "vorgo/";
            server += __VOGRO_VERSION__;
            server += ", https://github.com/Andrewpqc/vogro";
            this->headers["Server"] = server;

            // add Date header
            this->headers["Date"] = this->getCurrentCMTTime();
            // add Content-Type header
            this->headers["Content-Type"] = "text/html; charset=utf-8";
        }

        int getCode() { return this->code; }

        void setCode(int code) {
            this->code = code;
            vogro::StatusCodeMap &codeMap = vogro::StatusCodeMap::GetInstance();
            this->phrase = codeMap.getPharseByCode(code);
        }

        void setCode(int code, const std::string & phrase) {
            this->code = code;
            this->phrase = phrase;
        }

        void setPhrase(const std::string & phrase) { this->phrase = phrase; }

        void addHeader(const std::string & key, const std::string & val) { this->headers[key] = val; }

        std::string getHeader(std::string key) {
            auto got = this->headers.find(key);
            return (got == this->headers.end()) ? "" : got->second;
        }

        /************************************************************************
        Cookie Syntax

            Set-Cookie: <cookie-name>=<cookie-value>
            Set-Cookie: <cookie-name>=<cookie-value>; Expires=<date>
            Set-Cookie: <cookie-name>=<cookie-value>; Max-Age=<non-zero-digit>
            Set-Cookie: <cookie-name>=<cookie-value>; Domain=<domain-value>
            Set-Cookie: <cookie-name>=<cookie-value>; Path=<path-value>
            Set-Cookie: <cookie-name>=<cookie-value>; Secure
            Set-Cookie: <cookie-name>=<cookie-value>; HttpOnly
            Set-Cookie: <cookie-name>=<cookie-value>; SameSite=Strict
            Set-Cookie: <cookie-name>=<cookie-value>; SameSite=Lax

            // Multiple directives are also possible, for example:
            Set-Cookie: <cookie-name>=<cookie-value>; Domain=<domain-value>; Secure; HttpOnly

            to view more, see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Set-Cookie
         **************************************************************************/


        void setCookie(const std::string & k, const std::string & v, const std::string & path = "/",
                bool httpOnly = true, const std::string & maxAge = "", const std::string & domain = "", const std::string &expires = "") {
            std::stringstream ss;
            ss << k << "=" << v;

            if (path != "/")
                ss << "; Path=" << path;  // if path not set, the default path is "/"

            if (httpOnly)
                ss << "; HttpOnly";

            if (maxAge != "")
                ss << "; Max-Age=" << maxAge;

            if (domain != "")
                ss << "; Domain=" << domain;

            if (expires != "")
                ss << "; Expires=" << expires;

            this->addHeader("Set-Cookie", ss.str());
        }


        void redirect(const std::string & location, int code = 307) {
            this->setCode(code);
            this->addHeader("Location", location);
        }

        void writeJSON(const nlohmann::json &j) {
            this->addHeader("Content-Type", "application/json");
            this->addBody(j.dump());
        }

        void addBody(const std::string& bodyString) { this->body << bodyString; }

        void renderStringTemplate(const std::string &html, const vtpl::Environment &env) {
            auto rendred = vtpl::TemplateRender(html, env);
            this->addBody(rendred);
        }

        void renderFileTemplate(const std::string & filename,const vtpl::Environment& env) {
            std::ifstream f(filename);
            if (!f) {
                std::cout<<filename << " not found"<<std::endl;
                exit(1);
            }
            std::string html((std::istreambuf_iterator<char>(f)),std::istreambuf_iterator<char>());
            auto rendered = vtpl::TemplateRender(html,env);
            this->addBody(rendered);
        }

        std::stringstream &getResponseBodyStrem() { return body; }

        std::string makeResponseMsgWithoutBody() {
            std::stringstream responseMsg;

            // add  response line
            responseMsg << "HTTP/" << this->version << " " << this->code << " "
                        << this->phrase << "\r\n";
            for (auto header : this->headers) {
                responseMsg << header.first << ": " << header.second << "\r\n";
            }

            // empty line
            responseMsg << "\r\n";
            return responseMsg.str();
        }

        std::string makeResponseMsg() {
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
} // namespace vogro

#endif
