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
#ifndef __VOGRO_TEST_RESPONSE_HPP__
#define __VOGRO_TEST_RESPONSE_HPP__

#include <cassert>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#include "utils.hpp"

void TestFailedReportor(
        const std::string method,
        const std::string path,
        const std::string &entity,
        const std::string &action,
        const std::string &expect,
        const std::string &got
) {

    std::cout << "[" << method << "] "
              << path << INITCOLOR(RED_COLOR)
              << " Failed" << INITCOLOR(ZERO_COLOR)
              << std::endl << std::endl;

    std::cout << "expect " << entity
              << " " << action << " :"
              << std::endl;

    std::cout << "    " << expect
              << std::endl;

    std::cout << "but got :"
              << std::endl;

    if (got == "") std::cout << "    not found" << std::endl;
    else std::cout << "    " << got << std::endl;

    std::cout << std::endl;
    exit(1);
}


namespace vogro {

    class HeaderExpectation {
    private:
         std::string method_;
         std::string path_;
         std::map<std::string, std::string> headers;

    public:
        HeaderExpectation(
                const std::map<std::string, std::string> &hdrs,
                const std::string &method,
                const std::string &path)
                : headers(hdrs),
                  method_(method),
                  path_(path) {}

        const std::string Raw(const std::string &header_key) {
            auto got = this->headers.find(header_key);
            return (got == this->headers.end()) ? "" : got->second;
        }

        HeaderExpectation &Contains(
                const std::string &header_key,
                const std::string &header_val
        ) {
            auto h = this->headers.find(header_key);
            auto expect = header_key + ": " + header_val;
            if (h == this->headers.end()) {
                TestFailedReportor(
                        this->method_,
                        this->path_,
                        "Header",
                        "contains",
                        expect,
                        ""
                );
            }

            if (h->second != header_val) {
                auto got = h->first + ": " + h->second;
                TestFailedReportor(
                        this->method_,
                        this->path_,
                        "Header",
                        "contains",
                        expect,
                        got
                );
            }
            return *this;
        }

        HeaderExpectation &NotContains(
                const std::string &header_key,
                const std::string &header_val
        ) {
            auto h = this->headers.find(header_key);
            auto expect = header_key + ": " + header_val;
            if (h != this->headers.end() && h->second == header_val) {
                auto got = h->first + ": " + h->second;
                TestFailedReportor(
                        this->method_,
                        this->path_,
                        "Header",
                        "not contain",
                        expect,
                        got
                );
            }

            return *this;
        }
    }; // class HeaderExpectation

    class BodyExpectation {
    private:
         std::string body;
         std::string method_;
         std::string path_;

    public:
        BodyExpectation(
                const std::string &bd,
                const std::string &method,
                const std::string &path)
                : body(bd),
                  method_(method),
                  path_(path) {}

        const std::string Raw() {
            return this->body;
        }

        BodyExpectation &Contains(const std::string &x) {
            std::size_t found = this->body.find(x);
            if (found == std::string::npos) {
                TestFailedReportor(
                        this->method_,
                        this->path_,
                        "Body",
                        "contains",
                        x,
                        this->body
                );
            }
            return *this;
        }

        BodyExpectation &NotContains(const std::string &x) {
            std::size_t found = this->body.find(x);
            if (found != std::string::npos) {
                TestFailedReportor(
                        this->method_,
                        this->path_,
                        "Body",
                        "not contain",
                        x,
                        this->body
                );
            }
            return *this;
        }

        BodyExpectation &Equal(const std::string &bd) {
            if (this->body != bd) {
                TestFailedReportor(
                        this->method_,
                        this->path_,
                        "Body",
                        "equal",
                        bd,
                        this->body
                );
            }
            return *this;
        }

        BodyExpectation &NotEqual(const std::string &bd) {
            if (this->body == bd) {
                TestFailedReportor(
                        this->method_,
                        this->path_,
                        "Body",
                        "not equal",
                        bd,
                        this->body
                );
            }
            return *this;
        }
    }; // class BodyExpectation

    class Response: std::enable_shared_from_this<Response> {
    private:
         std::string req_method_;
         std::string req_path_;
        int code;
        std::map<std::string, std::string> headers;
        std::string body;

    public:
        friend class Request;

        // Responce(std::string &&bd): body(std::move(bd)) {}

        Response(const std::string &bd,
                 std::map<std::string, std::string> &hdrs,
                 const std::string &req_method,
                 const std::string &req_path
        ) : req_method_(req_method),
            req_path_(req_path),
            headers(hdrs),
            body(bd) {
              std::cout << body << " " << bd << std::endl;
            }

        Response *Status(int c) {
        // Response &Status(int c) {
            // assert(this->code == c);
            std::cout << body << std::endl;
            // return *this;
            return this;
            // return shared_from_this();
        }


        std::shared_ptr<HeaderExpectation> Header() {
        // HeaderExpectation &Header() {
            auto hp = std::make_shared<HeaderExpectation>(
                    this->headers,
                    this->req_method_,
                    this->req_path_
            );
            return hp;
            // return *hp;
        }

        std::shared_ptr<BodyExpectation> Body() {
        // BodyExpectation &Body() {
          std::cout << body  << std::endl;
            auto bd = std::make_shared<BodyExpectation>(
                    this->body,
                    this->req_method_,
                    this->req_path_
            );
            // return *bd;
            return bd;
        }
    }; //class Response
} //namespace vogro

#endif
