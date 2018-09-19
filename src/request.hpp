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

#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include "utils.hpp"
#include "json.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

namespace vogro {

class Request {
private:
    std::string method;

    std::string path;

    // 1.1,1.0,2.0
    std::string version;

    std::string host;

    std::string remote_ip;

    unsigned short remote_port;

    // http or https
    std::string protocol;

    // the request body
    std::shared_ptr<std::istream> body;
    // pt::ptree jsonTree;

    // request haeaders are stored in an unordered_map
    std::unordered_map<std::string, std::string> headers;

    // the parameters in the url query string(urlencoded)
    std::map<std::string, std::string> queryParam;

    // the parameters in the form data(POST, urlencodeed)
    std::map<std::string, std::string> formParam;

    // the parameters in path
    std::map<std::string, std::string> pathParam;

public:
    std::string getMethod() { return this->method; }

    void setMethod(const std::string& mhd) { this->method = mhd; }

    std::string getPath() { return this->path; }

    void setPath(const std::string& ph) { this->path = ph; }

    std::string getVersion() { return this->version; }

    void setVersion(const std::string& vsn) { this->version = vsn; }

    std::string getHost() { return this->host; }

    void setHost(const std::string& hst) { this->host = hst; }

    std::string getRemoteIP() { return this->remote_ip; }

    void setRemoteIP(const std::string& r_ip) { this->remote_ip = r_ip; }

    unsigned short getRemotePort() { return this->remote_port; }

    void setRemotePort(const unsigned short port) { this->remote_port = port; }

    std::string getProtocol() { return this->protocol; }

    void setProtocol(const std::string& protoc) { this->protocol = protoc; }

    std::string getHeader(std::string key)
    {
        auto got = this->headers.find(key);
        return (got == this->headers.end()) ? "" : got->second;
    }

    std::unordered_map<std::string, std::string> getHeaders() { return this->headers; }

    void addHeader(std::string key, std::string val) { this->headers[key] = val; }

    void setPathParam(std::map<std::string, std::string>& pathParam) { this->pathParam = pathParam; }

    std::string getPathParam(std::string key)
    {
        auto got = this->pathParam.find(key);
        return (got == this->pathParam.end()) ? "" : got->second;
    }

    void setQueryParam(std::string queryString) { this->queryParam = split_query_string(queryString); }

    std::string getQueryParam(std::string key)
    {
        auto got = this->queryParam.find(key);
        return (got == this->queryParam.end()) ? "" : got->second;
    }

    void setFormParam(std::string formData) { this->formParam = split_query_string(formData); }

    std::string getFormParam(std::string key)
    {
        auto got = this->formParam.find(key);
        return (got == this->formParam.end()) ? "" : got->second;
    }

     std::shared_ptr<std::istream> getBody()
     {
            return this->body;
    }

    void setBody(std::shared_ptr<std::istream> p)
    {
            this->body = p;
    }


    // void ReadJSON(std::shared_ptr<std::istream> jsonPtr) { pt::read_json(*jsonPtr, jsonTree); }

    nlohmann::json ReadJSON(){
        std::string s;
        *body >> s;
        auto j = nlohmann::json::parse(s);
        return j;
    }

    // const pt::ptree& GetJsonTree() { return this->jsonTree; }

    std::string getCookie(std::string key = "")
    {
        auto cookieString = this->getHeader("Cookie");
        if (key == "" || cookieString == "")
            return cookieString;

        auto tokens = split(cookieString, ',');
        for (auto tk : tokens) {
            auto kv = split(tk, '=');
            if (kv.size() == 2 && trim(kv[0]) == key)
                return kv[1];
        }

        return "";
    }

    std::pair<std::string,std::string> basicAuth() {
        auto auth = this->getHeader("Authorization");
        if (auth.rfind("Basic ", 0) != 0) 
            return std::make_pair("","");

        auth =auth.substr(6);

        auto decoded_auth = base64_decode(auth);

        auto result = split(decoded_auth,':');
        if(result.size()==1)
            return std::make_pair(result[0],"");
        
        return std::make_pair(result[0],result[1]);
    }
};

} // namespace vogro

#endif