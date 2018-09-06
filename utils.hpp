#include <string>
#include "common.hpp"
#ifndef __UTILS_HPP__
#define __UTILS_HPP__

namespace vogro{

std::pair<std::string,std::string> parse_header(std::string header) {
    std::string header_key, header_val;
    bool headerValStart = false;
    for (int i = 0; i < header.length(); i++) {
        if (header[i] == ':') {
            headerValStart = true;
            continue;
        }
        if (!headerValStart) {
            header_key += header[i];
        }
        else{
            header_val += header[i];
        }
    }

    header_key = trim(header_key);
    header_val = trim(header_val);
    return std::make_pair(header_key,header_val);    
}


std::pair<std::string,std::pair<std::string,std::string>> parse_request_line(std::string request_line) {
    std::string method, url, version;
    char flag = 'm';
    bool versionNumberStart = false;
    for (auto i = 0; i < request_line.length(); i++) {
        if (flag == 'm'){
            if (request_line[i] == ' ') {
                flag = 'u';
                continue;
            }
            method += request_line[i];
        }
        if (flag == 'u'){
            if (request_line[i] == ' ') {
                flag = 'v';
                continue;
            }
            url += request_line[i];
        }
        if (flag == 'v'){
            if (request_line[i] == '/') {
                versionNumberStart = true;
                ++i;
            }
            if (versionNumberStart)
                version += request_line[i];
        }
    }

    // for return three value
    auto url_version_pair=std::make_pair(url,version);
    return std::make_pair(method,url_version_pair); 
}

bool urlMatch(std::string requestUrl, std::string handlerUrl,
              std::map<std::string, std::string> &storeMap) {
  if (handlerUrl.back() != '/')
    handlerUrl += '/';
  if (requestUrl.back() != '/')
    requestUrl += '/';

  std::string type, name, dynamicParam;
  auto handlerUrlLength = handlerUrl.length();
  auto requestUrlLength = requestUrl.length();
  auto max_length = (handlerUrlLength > requestUrlLength) ? handlerUrlLength
                                                          : requestUrlLength;
  for (auto i = 0, j = 0; (i < max_length) && (j < max_length); i++, j++) {
    if (handlerUrl[i] == '{') {
      auto tempIndex = i + 1;
      bool flag = true; // true代表当前在type域中
      do {
        if (handlerUrl[tempIndex] == ':') {
          flag = false;
          ++tempIndex;
        }

        if (!flag)
          name += handlerUrl[tempIndex];
        else
          type += handlerUrl[tempIndex];

        ++tempIndex;
      } while (handlerUrl[tempIndex] != '}');

      i = tempIndex + 1;

      if (flag == true)
        type = "int"; // default type is int
      do {
        // if type is int, every char in dynamicParam should be in [48,57]
        if ((type == "int") && (requestUrl[j] < 48 || requestUrl[j] > 57))
          return false;
        dynamicParam += requestUrl[j];
        ++j;
      } while (requestUrl[j] != '/');

      // store the dynamic parameters to storeMap
      storeMap[name] = dynamicParam;

      // clear name, type, dynamicParam
      name.clear();
      type.clear();
      dynamicParam.clear();
    }

    if ((i >= handlerUrlLength) || (j >= requestUrlLength) ||
        (handlerUrl[i] != requestUrl[j])) {
      return false;
    }
  }
  return true;
}

}


#endif