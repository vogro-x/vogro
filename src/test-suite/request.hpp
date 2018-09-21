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
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <iostream>

namespace vogro{
    class Request{
    private:
        std::string method;
        std::string path;
        std::map<std::string,std::string> queryParams;
        std::vector<std::pair<std::string,std::string>> headers;
        std::map<std::string,std::string> pathParams;
    public:
        Request(const std::string& mtd,const std::string& p):method(mtd),path(p){}
        Request& withQuery(const std::string& key,const std::string& val){
            this->queryParams[key]=val;
            return *this;
        }
        Request& withBasicAuth();
        Request& withPath(const std::string& key,const std::string& val){
            this->pathParams[key]=val;
            return *this;
        }

        Request& withHeader(const std::string& key,const std::string& val){
            auto header = std::make_pair(key,val);
            this->headers.push_back(header);
            return *this;
        }

        std::string getFinalPath(){
            std::stringstream pathStream;
            if(this->path.back()!='/')
                this->path += '/';
            
            pathStream <<this->path;
            
            if(this->pathParams.size()!=0)
                pathStream <<"?";
                
            for(auto it=pathParams.begin();it!=pathParams.end();it++){
                pathStream <<it->first<<"="<<it->second<<"&";
            }

            return pathStream.str();
        }

        std::string makeRequestMessage(){
            std::stringstream ss;
            ss << this->method << " " << this->getFinalPath() << " HTTP/1.1\r\n";
            for(auto h: this->headers){
                ss << h.first<<": "<<h.second<<"\r\n";
            }
            ss<<"\r\n";
            return ss.str();
        }

    };
}
#endif


