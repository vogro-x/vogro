#ifndef __REQUEST_HPP__
#define __REQUEST_HPP__

#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

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
  // std::shared_ptr<std::istream> content;
  pt::ptree jsonTree;

  // request haeaders are stored in an unordered_map
  std::unordered_map<std::string, std::string> headers;

  // the parameters in the url query string(urlencoded)
  std::map<std::string, std::string> queryParam;

  // the parameters in the form data(POST, urlencodeed)
  std::map<std::string, std::string> formParam;

  // splitQueryString devide query string such as "id=123&name=andrew" into key
  // value pair and store them into storeMap
  void splitQueryString(std::string queryString,
                        std::map<std::string, std::string> &storeMap) {
    std::string key = "";
    std::string val = "";
    auto flag = true;
    for (auto i = 0; i <= queryString.length(); i++) {
      if (i == queryString.length() || queryString[i] == '&') {
        flag = true;

        storeMap[key] = val;

        key.clear();
        val.clear();

        continue;

      } else if (queryString[i] == '=') {

        flag = false;

        continue;
      }

      if (flag) {
        key += queryString[i];
      } else {
        val += queryString[i];
      }
    }
  }

public:
  // the parameters in path
  std::map<std::string, std::string> pathParam;

  std::string getMethod() { return this->method; }

  void setMethod(const std::string &mhd) { this->method = mhd; }

  std::string getPath() { return this->path; }

  void setPath(const std::string &ph) { this->path = ph; }

  std::string getVersion() { return this->version; }

  void setVersion(const std::string &vsn) { this->version = vsn; }

  std::string getHost() { return this->host; }

  void setHost(const std::string &hst) { this->host = hst; }

  std::string getRemoteIP() { return this->remote_ip; }

  void setRemoteIP(const std::string &r_ip) { this->remote_ip = r_ip; }

  unsigned short getRemotePort() { return this->remote_port; }

  void setRemotePort(const unsigned short port) { this->remote_port = port; }

  std::string getProtocol() { return this->protocol; }

  void setProtocol(const std::string &protoc) { this->protocol = protoc; }

  std::string getHeader(std::string key) {
    auto got = this->headers.find(key);
    return (got == this->headers.end()) ? "" : got->second;
  }

  std::unordered_map<std::string, std::string> getHeaders() {
    return this->headers;
  }

  void addHeader(std::string key, std::string val) { this->headers[key] = val; }

  void addPathParam(std::string key, std::string val) {
    this->pathParam[key] = val;
  }

  std::string getPathParam(std::string key) {
    auto got = this->pathParam.find(key);
    return (got == this->pathParam.end()) ? "" : got->second;
  }

  void setQueryParam(std::string queryString) {
    this->splitQueryString(queryString, this->queryParam);
  }

  std::string getQueryParam(std::string key) {
    auto got = this->queryParam.find(key);
    return (got == this->queryParam.end()) ? "" : got->second;
  }

  void setFormParam(std::string formData) {
    this->splitQueryString(formData, this->formParam);
  }

  std::string getFormParam(std::string key) {
    auto got = this->formParam.find(key);
    return (got == this->formParam.end()) ? "" : got->second;
  }

  void ReadJSON(std::shared_ptr<std::istream> jsonPtr) {
    pt::read_json(*jsonPtr, jsonTree);
  }

  const pt::ptree &GetJsonTree() { return this->jsonTree; }
};

} // namespace vogro

#endif