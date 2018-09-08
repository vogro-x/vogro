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

#ifndef __VOGRO_UTILS_HPP__
#define __VOGRO_UTILS_HPP__
#include <unistd.h>
#include <algorithm>
#include <map>
#include <string>

std::string ltrim(std::string str) {
    auto length = str.length();
    while (str.length()) {
        if (str[0] == ' ')
            str.erase(0, 1);
        else
            return str;
    }
    return str;
}

std::string rtrim(std::string str) {
    auto length = str.length();
    for (auto i = length - 1; i >= 0; i--) {
        if (str[i] == ' ')
            str.pop_back();
        else
            return str;
    }
    return str;
}

std::string trim(std::string str) { return rtrim(ltrim(str)); }

std::pair<bool, bool> urlMatch(std::string requestUrl, std::string handlerUrl,
                               std::map<std::string, std::string> &storeMap) {
    if (requestUrl.find("/static") == 0) return std::make_pair(false, true);

    if (handlerUrl.back() != '/') handlerUrl += '/';
    if (requestUrl.back() != '/') requestUrl += '/';

    std::string type, name, dynamicParam;

    auto handlerUrlLength = handlerUrl.length();
    auto requestUrlLength = requestUrl.length();
    auto max_length = (handlerUrlLength > requestUrlLength) ? handlerUrlLength
                                                            : requestUrlLength;
    for (auto i = 0, j = 0; (i < max_length) && (j < max_length); ++i, ++j) {
        if (handlerUrl[i] == '{') {
            auto tempIndex = i + 1;
            bool flag = true;  // true代表当前在type域中
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

            if (flag == true) type = "int";

            do {
                // if type is int, every char in dynamicParam should be in
                // [48,57]
                if ((type == "int") &&
                    (requestUrl[j] < 48 || requestUrl[j] > 57)) {
                    return std::make_pair(false, false);
                }
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
            return std::make_pair(false, false);
        }
    }
    return std::make_pair(true, false);
}

std::map<std::string, std::string> split_query_string(std::string str) {
    std::map<std::string, std::string> results;
    std::string key, val;
    auto flag = true;
    if (str.length() == 0) return results;

    for (auto i = 0; i <= str.length(); i++) {
        if (i == str.length() || str[i] == '&') {
            flag = true;
            results[key] = val;
            key.clear();
            val.clear();
            continue;
        } else if (str[i] == '=') {
            flag = false;
            continue;
        }

        if (flag)
            key += str[i];
        else
            val += str[i];
    }
    return results;
}

std::pair<std::string, std::string> parse_header(std::string &header) {
    std::string header_key, header_val;

    header_val.reserve(50);  // pre alloc space

    auto pos = header.find(":");
    header_key = trim(header.substr(0, pos));
    header_val = trim(header.substr(pos + 1));

    return std::make_pair(header_key, header_val);
}

std::pair<std::string, std::pair<std::string, std::string>> parse_request_line(
    std::string request_line) {
    std::string method, url, version;

    url.reserve(50);  // pre alloc 50 byte

    char flag = 'm';
    bool versionNumberStart = false;
    for (auto i = 0; i < request_line.length(); i++) {
        if (flag == 'm') {
            if (request_line[i] == ' ') {
                flag = 'u';
                continue;
            }
            method += request_line[i];
        }
        if (flag == 'u') {
            if (request_line[i] == ' ') {
                flag = 'v';
                continue;
            }
            url += request_line[i];
        }
        if (flag == 'v') {
            if (request_line[i] == '/') {
                versionNumberStart = true;
                ++i;
            }
            if (versionNumberStart) version += request_line[i];
        }
    }

    // for return three value
    auto url_version_pair = std::make_pair(url, version);
    return std::make_pair(method, url_version_pair);
}

std::string getFileExtension(std::string path) {
    auto pos = path.find_last_of('.');
    std::string ext = path.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), [](char in) -> char {
        if (in <= 'Z' && in >= 'A') return in - ('Z' - 'z');
        return in;
    });
    return ext;
}

int is_file_exist(const char *path) {
    if (path == NULL) return -1;
    if (access(path, F_OK) == 0) {
        return 0;
    }
    return -1;
}

#endif