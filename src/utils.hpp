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

#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <locale>
#include <codecvt>

#define INITCOLOR(color)  std::string("\033[1;") + std::string(color) + std::string("m")
#define RED_COLOR "31"
#define GREEN_COLOR "32"
#define YELLOW_COLOR "33"
#define BLUE_COLOR "34"
#define ZERO_COLOR "0"

std::string ltrim(std::string str)
{
    auto length = str.length();
    while (str.length()) {
        if (str[0] == ' ')
            str.erase(0, 1);
        else
            return str;
    }
    return str;
}

std::string rtrim(std::string str)
{
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

inline static std::string url_encode(const std::string &value) noexcept {
    static auto hex_chars = "0123456789ABCDEF";

    std::string result;
    result.reserve(value.size()); // Minimum size of result

    for (auto &chr : value) {
        if (!((chr >= '0' && chr <= '9') || (chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z') || chr == '-' || chr == '.' || chr == '_' || chr == '~'))
            result += std::string("%") + hex_chars[static_cast<unsigned char>(chr) >> 4] + hex_chars[static_cast<unsigned char>(chr) & 15];
        else
            result += chr;
    }

    return result;
}

inline static std::string url_decode(const std::string &value) noexcept {
    std::string result;
    result.reserve(value.size() / 3 + (value.size() % 3)); // Minimum size of result

    for (std::size_t i = 0; i < value.size(); ++i) {
        auto &chr = value[i];
        if (chr == '%' && i + 2 < value.size()) {
            auto hex = value.substr(i + 1, 2);
            auto decoded_chr = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
            result += decoded_chr;
            i += 2;
        }
        else if (chr == '+')
            result += ' ';
        else
            result += chr;
    }

    return result;
}

inline static  std::string u8wstring_to_string(const std::wstring& wstr)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(wstr);
}

inline static std::wstring u8string_to_wstring(const std::string& str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
    return conv.from_bytes(str);
}




std::pair<bool, bool> urlMatch(std::string requestUrl, std::string handlerUrl,
    std::map<std::string, std::string>& storeMap)
{
    if (requestUrl.find("/static") == 0)
        return std::make_pair(false, true);

    if (handlerUrl.back() != '/')
        handlerUrl += '/';
    if (requestUrl.back() != '/')
        requestUrl += '/';

    std::string type, name, dynamicParam;

    auto handlerUrlLength = handlerUrl.length();
    auto requestUrlLength = requestUrl.length();
    auto max_length = (handlerUrlLength > requestUrlLength) ? handlerUrlLength
                                                            : requestUrlLength;
    size_t i = 0, j = 0; 
    for ( ; (i < max_length) && (j < max_length); ++i, ++j) {
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
                type = "int";

            do {
                // if type is int, every char in dynamicParam should be in
                // [48,57]
                if ((type == "int") && (requestUrl[j] < 48 || requestUrl[j] > 57)) {
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

        if ((i >= handlerUrlLength) || (j >= requestUrlLength) || (handlerUrl[i] != requestUrl[j])) {
            return std::make_pair(false, false);
        }
    }
    if(j < requestUrlLength ) return std::make_pair(false,false);
    return std::make_pair(true, false);
}

std::map<std::string, std::string> split_query_string(std::string str)
{
    std::map<std::string, std::string> results;
    std::string key, val;
    auto flag = true;
    if (str.length() == 0)
        return results;

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

std::pair<std::string, std::string> parse_header(std::string& header)
{
    std::string header_key, header_val;

    header_val.reserve(50); // pre alloc space

    auto pos = header.find(":");
    header_key = trim(header.substr(0, pos));
    header_val = trim(header.substr(pos + 1));

    return std::make_pair(header_key, header_val);
}

std::pair<std::string, std::pair<std::string, std::string>> parse_request_line(
    std::string request_line)
{
    std::string method, url, version;

    url.reserve(50); // pre alloc 50 byte

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
            if (versionNumberStart)
                version += request_line[i];
        }
    }

    
    // for return three value
    auto url_version_pair = std::make_pair(url_decode(url), version);
    return std::make_pair(method, url_version_pair);
}

std::string getFileExtension(std::string path)
{
    auto pos = path.find_last_of('.');
    std::string ext = path.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), [](char in) -> char {
        if (in <= 'Z' && in >= 'A')
            return in - ('Z' - 'z');
        return in;
    });
    return ext;
}

int is_file_exist(const char* path)
{
    if (path == NULL)
        return -1;
    if (access(path, F_OK) == 0) {
        return 0;
    }
    return -1;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}



static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = ( char_array_4[0] << 2       ) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +   char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = 0; j < i; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

#endif