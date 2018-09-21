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
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <cassert>
#include <memory>
namespace vogro {

    class HeaderExpectation {
        private:
            std::map<std::string,std::string> headers;
        public:
        HeaderExpectation(std::map<std::string,std::string> & hdrs):headers(hdrs){}
        HeaderExpectation& Contains(const std::string & header_key, const std::string& header_val){
            auto h = this->headers.find(header_key);
            assert( h!=this->headers.end() && h->second == header_val);
            return *this;
        }

        HeaderExpectation& NotContains(const std::string& header_key, const std::string& header_val) {
            auto h= this->headers.find(header_key);
            assert( h==this->headers.end() ||h->second != header_val );
            return *this;
        }

    };

    class BodyExpectation {
        private:
            std::string body;
        public:
        BodyExpectation(std::string& bd):body(bd) {}
        BodyExpectation& Contains(const std::string& x) {
            std::size_t found = this->body.find(x);
            std::cout<<"hhhh"<<std::endl;

            assert(found != std::string::npos);
            return *this;
        }

        BodyExpectation& NotContains(const std::string& x) {
            std::size_t found = this->body.find(x);
            assert(found == std::string::npos);
            return *this;
        }

        BodyExpectation& Equal(const std::string& bd){
            assert(this->body == bd);
            return *this;
        }

        BodyExpectation& NotEqual(const std::string& bd){
            assert(this->body != bd);
            return *this;
        }
    };

    class Response {
    private:
        int code;
        std::map<std::string,std::string> headers;
        std::string body;
        void setCode(const int code){ this->code =code; }
        void addHeader(const std::string& key,const std::string& val){this->headers[key]=val; }
        void setBody(const std::string& bd){this->body = bd;}
    public:
        friend class Request;
        Response(){}
        Response& Status(int code){
            std::cout<<"status"<<std::endl;
            assert(this->code == code);
            return *this;
        }
        HeaderExpectation& Header(){
            auto hp = std::shared_ptr<HeaderExpectation>( new HeaderExpectation(this->headers));
            return *hp;
        }

        BodyExpectation& Body() {
            auto bd =std::shared_ptr<BodyExpectation>(new BodyExpectation(this->body));
            return *bd;
        }
    };
}