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

// TODO
// 数据压缩传输
// 断点续传
// 重构代码
// 文件上传处理
#ifndef __STATIC_SERVER_HPP__
#define __STATIC_SERVER_HPP__
#include "mime_type.hpp"
#include "request.hpp"
#include "response.hpp"
#include "vogro_utils.hpp"

#include <dirent.h>
#include <boost/asio.hpp>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

// https://github.com/golang/go/blob/42257a262c94d839364113f2dbf4057731971fc1/src/net/http/fs.go#L713

// Ranges : bytes = xxx -xxx
std::pair<long, long> parseRange(const std::string &Ranges) {
  auto pos1 = Ranges.find_first_of('=');
  auto pos2 = Ranges.find_last_of('-');
  auto start = Ranges.substr(pos1 + 1, pos2 - pos1 - 1);
  auto end = Ranges.substr(pos2 + 1);
  std::cout << "|" << start << "|" << end << "|" << std::endl;
  auto int_start = (start == "") ? -1 : std::stol(start);
  auto int_end = (end == "") ? -1 : std::stol(end);
  return std::make_pair(int_start, int_end);
}

template <typename socket_type>
void ServeStatic(vogro::Response &response, vogro::Request &request,
                 std::ostream &responseStream,
                 std::shared_ptr<boost::asio::streambuf> &write_buffer,
                 socket_type socket) {
  char *buffer;
  if ((buffer = getcwd(NULL, 0)) == NULL) {
    perror("getcwd error");
    exit(EXIT_FAILURE);
  }

  auto cwd = std::string(buffer);
  auto path = request.getPath();
  auto filepath = cwd + path;

  auto ext = getFileExtension(filepath);
  MimeTypeMap &mime = MimeTypeMap::GetInstance();
  auto type = mime.getMimeTypeByExt(ext);
  if (type == "") {
    response.setCode(415);

  } else {
    if (is_file_exist(filepath.c_str())) {
      response.setCode(404);

    } else {
      response.addHeader("Connection", "Keep-Alive");
      response.addHeader("Content-Type", type);
      response.addHeader("Accept-Ranges", "bytes");

      std::ifstream ifs(filepath, std::ifstream::binary);
      ifs.seekg(0, std::ios::end);
      int64_t fileLength = ifs.tellg();
      ifs.seekg(0, std::ios::beg);

      auto rangeValue = request.getHeader("Range");
      if (rangeValue == "") {
        //普通请求
        response.getResponseBodyStrem() << ifs.rdbuf();

      } else {
        //范围请求
        auto rangeResult = parseRange(rangeValue);
        if (rangeResult.first == -1 && rangeResult.second != -1) {
          //请求最后的rangeResult.secode字节的数据

          if (rangeResult.first >= fileLength) {
            response.setCode(416);
          } else {
            response.setCode(206);

            std::stringstream contentRange;
            contentRange << "bytes " << fileLength - rangeResult.second << "-"
                         << fileLength << '/' << fileLength;
            response.addHeader("Content-Range", contentRange.str());

            ifs.seekg(-rangeResult.second, std::ios::end);
            response.getResponseBodyStrem() << ifs.rdbuf();
          }
        } else if (rangeResult.first != -1 && rangeResult.second == -1) {
          //请求从rangeResult.first到文件结尾的数据

          if (rangeResult.first >= fileLength) {
            response.setCode(416);
          } else {
            std::stringstream contentRange;
            contentRange << "bytes " << rangeResult.first << "-" << fileLength
                         << "/" << fileLength;
            response.addHeader("Content-Range", contentRange.str());

            response.setCode(206);

            ifs.seekg(rangeResult.first, std::ios::beg);
            response.getResponseBodyStrem() << ifs.rdbuf();
          }
        } else {
          //请求从rangeResult.first到rangeResult.secode字节范围的数据
          if (rangeResult.first > rangeResult.second) {
            response.setCode(416);
          } else {
            //...
            response.getResponseBodyStrem() << ifs.rdbuf();
          }
        }
      }
    }

    return;
  }
}

#endif
