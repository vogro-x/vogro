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

namespace vogro {

class HeaderExpectation {
private:
    std::map<std::string, std::string> headers;

public:
    HeaderExpectation(const std::map<std::string, std::string>& hdrs)
        : headers(hdrs)
    {
    }

    const std::string Raw(const std::string & header_key){
        auto got = this->headers.find(header_key);
        return (got == this->headers.end()) ? "" : got->second;
    }

    HeaderExpectation& Contains(const std::string& header_key, const std::string& header_val)
    {
        auto h = this->headers.find(header_key);
        assert(h != this->headers.end() && h->second == header_val);
        return *this;
    }

    HeaderExpectation& NotContains(const std::string& header_key, const std::string& header_val)
    {
        auto h = this->headers.find(header_key);
        assert(h == this->headers.end() || h->second != header_val);
        return *this;
    }
};

class BodyExpectation {
private:
    std::string body;

public:
    BodyExpectation(const std::string bd)
        : body(bd)
    {
    }

    const std::string Raw(){
        return this->body;
    }

    BodyExpectation& Contains(const std::string& x)
    {
        size_t found = this->body.find(x);
        assert(found != std::string::npos);
        return *this;
    }

    BodyExpectation& NotContains(const std::string& x)
    {
        std::size_t found = this->body.find(x);
        assert(found == std::string::npos);
        return *this;
    }

    BodyExpectation& Equal(const std::string& bd)
    {
        assert(this->body == bd);
        return *this;
    }

    BodyExpectation& NotEqual(const std::string& bd)
    {
        assert(this->body != bd);
        return *this;
    }
};

class Response {
private:
    int code;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    friend class Request;

    Response() {}

    Response& Status(int c)
    {
        assert(this->code == c);
        return *this;
    }

    HeaderExpectation& Header()
    {
        auto hp = std::make_shared<HeaderExpectation>(this->headers);
        return *hp;
    }

    BodyExpectation& Body()
    {
        auto bd = std::make_shared<BodyExpectation>(this->body);
        return *bd;
    }
};
} //namespace vogro

#endif