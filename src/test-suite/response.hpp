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
    const std::string method_;
    const std::string& path_;
    std::map<std::string, std::string> headers;

public:
    HeaderExpectation(const std::map<std::string, std::string>& hdrs,const std::string& method, const std::string& path)
        : headers(hdrs)
        , method_(method)
        , path_(path)
    {
    }

    const std::string Raw(const std::string & header_key){
        auto got = this->headers.find(header_key);
        return (got == this->headers.end()) ? "" : got->second;
    }

    HeaderExpectation& Contains(const std::string& header_key, const std::string& header_val)
    {
        auto h = this->headers.find(header_key);
        if(!(h != this->headers.end() && h->second == header_val)){
            std::cout<<"["<<this->method_<<"] "<<this->path_<<" Failed"<<std::endl;
            exit(1);
        }
        return *this;
    }

    HeaderExpectation& NotContains(const std::string& header_key, const std::string& header_val)
    {
        auto h = this->headers.find(header_key);
         if(!(h == this->headers.end() || h->second != header_val)){
            std::cout<<"["<<this->method_<<"] "<<this->path_<<" Failed"<<std::endl;
            exit(1);
        }
        return *this;
    }
};

class BodyExpectation {
private:
    std::string body;
    const std::string method_;
    const std::string& path_;

public:
    BodyExpectation(const std::string bd,const std::string& method, const std::string& path)
        : body(bd)
        , method_(method)
        , path_(path)
    {
    }

    const std::string Raw(){
        return this->body;
    }

    BodyExpectation& Contains(const std::string& x)
    {
        size_t found = this->body.find(x);
        // assert(found != std::string::npos);
        if(found == std::string::npos){
            std::cout<<"["<<this->method_<<"] "<<this->path_<<" Failed"<<std::endl;
            exit(1);
        }
        return *this;
    }

    BodyExpectation& NotContains(const std::string& x)
    {
        std::size_t found = this->body.find(x);
        // assert(found == std::string::npos);
         if(found != std::string::npos){
            std::cout<<"["<<this->method_<<"] "<<this->path_<<" Failed"<<std::endl;
            exit(1);
        }
        return *this;
    }

    BodyExpectation& Equal(const std::string& bd)
    {
        // assert(this->body == bd);
        if(this->body != bd){
            std::cout<<"["<<this->method_<<"] "<<this->path_<<" Failed"<<std::endl;
            exit(1);
        }
        return *this;
    }

    BodyExpectation& NotEqual(const std::string& bd)
    {
        // assert(this->body != bd);
        if(this->body == bd){
            std::cout<<"["<<this->method_<<"] "<<this->path_<<" Failed"<<std::endl;
            exit(1);
        }
        return *this;
    }
};

class Response {
private:
    const std::string& req_method_;
    const std::string& req_path_;
    int code;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    friend class Request;

    Response(const std::string & req_method,const std::string & req_path)
        : req_method_(req_method)
        , req_path_(req_path)
        {
        }

    Response& Status(int c)
    {
        assert(this->code == c);
        return *this;
    }

    HeaderExpectation& Header()
    {
        auto hp = std::make_shared<HeaderExpectation>(this->headers,this->req_method_,this->req_path_);
        return *hp;
    }

    BodyExpectation& Body()
    {
        auto bd = std::make_shared<BodyExpectation>(this->body,this->req_method_,this->req_path_);
        return *bd;
    }
};
} //namespace vogro

#endif