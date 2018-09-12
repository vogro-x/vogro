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
#include <sys/stat.h>
#include <time.h>
#include <boost/asio.hpp>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

// https://github.com/golang/go/blob/42257a262c94d839364113f2dbf4057731971fc1/src/net/http/fs.go#L713
// https://www.rfc-editor.org/rfc/rfc7233.txt

// Ranges : bytes = xxx -xxx

enum Cond {
    CondNone = 1,
    CondTrue,
    CondFalse,
};

std::pair<long, long> parseRange(const std::string &Ranges) {
    auto pos1 = Ranges.find_first_of('=');
    auto pos2 = Ranges.find_last_of('-');
    auto start = Ranges.substr(pos1 + 1, pos2 - pos1 - 1);
    auto end = Ranges.substr(pos2 + 1);
    auto int_start = (start == "") ? -1 : std::stol(start);
    auto int_end = (end == "") ? -1 : std::stol(end);
    return std::make_pair(int_start, int_end);
}

// getEtag to get the ETag of a static file, the Etag's format is
// A-B-C
// A---> i-node number(serial number) of the file
// B---> time of last modification of the file
// C---> size of bytes of the file
const std::string getEtag(const std::string &filename) {
    struct stat statbuf;
    stat(filename.c_str(), &statbuf);
    std::stringstream ss;
    ss << statbuf.st_ino << "-" << statbuf.st_mtime << "-" << statbuf.st_size;
    return ss.str();
}

// compareTime to compare a GMT time string and the time of last modification
// of the filename. If the giving GMT time is bigger than the file's last
// modification time, it return true, otherwise return false.
bool compareTime(const std::string &GmtString, const std::string &filename) {
    tm tm_;
    time_t t_;
    char buf[128] = {0};

    strcpy(buf, GmtString.c_str());
    strptime(buf, "%a, %d %b %Y %H:%M:%S GMT", &tm_);
    tm_.tm_isdst = -1;
    t_ = mktime(&tm_);
    t_ += 3600;
    struct stat statbuf;
    stat(filename.c_str(), &statbuf);
    return difftime(t_, statbuf.st_mtime) > 0;
}

// ETag值匹配才发送资源
Cond checkIfMatch(std::string &IfMatchHeadString, const std::string &filename) {
    auto im = trim(IfMatchHeadString);
    if (im == "") return CondNone;
    if (im == "*") return CondTrue;
    if (im == getEtag(filename)) return CondTrue;
    return CondFalse;
}

//
Cond checkIfUnmodifiedSince(std::string &IfUnmodifiedSinceString,
                            const std::string &filename) {
    auto iums = trim(IfUnmodifiedSinceString);
    if (iums == "") return CondNone;
    if (compareTime(iums, filename)) {
        return CondTrue;
    }
    return CondFalse;
}

Cond checkIfNoneMatch(std::string IfNoneMatchString,
                      const std::string &filename) {
    auto inm = trim(IfNoneMatchString);
    if (inm == "") return CondNone;
    if (inm == "*") return CondFalse;
    if (inm == "*" || inm == getEtag(filename)) return CondFalse;
    return CondTrue;
}

Cond checkIfModifiedSince(vogro::Request &request,
                          const std::string &filename) {
    if (request.getMethod() != "GET" && request.getMethod() != "HEAD")
        return CondNone;
    auto ims = request.getHeader("If-Modified-Since");
    if (ims == "") return CondNone;
    if (compareTime(ims, filename)) {
        return CondFalse
    }
    return CondTrue;
}

// If-Range is ETag or Date
Cond checkIfRange(vogro::Request &request, const std::string &filename) {
    if (request.getMethod() != "GET" && request.getMethod() != "HEAD")
        return CondNone;

    auto ir = request.getHeader("If-Range");
    if (ir == "") return CondNone;
}

void CheckPreconditons() {}

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
                auto etag = getEtag(filepath);
                response.addHeader("ETag", etag);
                response.getResponseBodyStrem() << ifs.rdbuf();

                responseStream << response.makeResponseMsg();
                boost::asio::async_write(*socket, *write_buffer,
                                         [](const boost::system::error_code &ec,
                                            size_t bytes_transferred) {});

            } else {
                //范围请求
                auto rangeResult = parseRange(rangeValue);
                if (rangeResult.first == -1 && rangeResult.second != -1) {
                    //请求最后的rangeResult.secode字节的数据

                    if (rangeResult.first >= fileLength) {
                        response.setCode(416);
                        responseStream << response.makeResponseMsg();
                        boost::asio::async_write(
                            *socket, *write_buffer,
                            [](const boost::system::error_code &ec,
                               size_t bytes_transferred) {});

                    } else {
                        response.setCode(206);

                        std::stringstream contentRange;
                        contentRange << "bytes "
                                     << fileLength - rangeResult.second << "-"
                                     << fileLength - 1 << '/' << fileLength;
                        response.addHeader("Content-Range", contentRange.str());

                        ifs.seekg(-rangeResult.second, std::ios::end);

                        response.getResponseBodyStrem() << ifs.rdbuf();
                        responseStream << response.makeResponseMsg();
                        boost::asio::async_write(
                            *socket, *write_buffer,
                            [](const boost::system::error_code &ec,
                               size_t bytes_transferred) {});
                    }
                } else if (rangeResult.first != -1 &&
                           rangeResult.second == -1) {
                    //请求从rangeResult.first到文件结尾的数据

                    if (rangeResult.first >= fileLength) {
                        response.setCode(416);
                        responseStream << response.makeResponseMsg();
                        boost::asio::async_write(
                            *socket, *write_buffer,
                            [](const boost::system::error_code &ec,
                               size_t bytes_transferred) {});
                    } else {
                        std::stringstream contentRange;
                        contentRange << "bytes " << rangeResult.first << "-"
                                     << fileLength - 1 << "/" << fileLength;
                        response.addHeader("Content-Range", contentRange.str());

                        response.setCode(206);
                        // responseStream <<
                        // response.makeResponseMsgWithoutBody();
                        ifs.seekg(rangeResult.first, std::ios::beg);
                        // char buf[4096];
                        // // // ssize_t n = 0;
                        // while (!ifs.eof()) {
                        //     ifs.read(buf, 4096);
                        //     // response.getResponseBodyStrem() <<buf;
                        //     responseStream << buf;
                        //     std::cout << "read buf" << std::endl;
                        //     // responseStream.flush();
                        //     // boost::asio::write(*socket,*write_buffer);
                        response.getResponseBodyStrem() << ifs.rdbuf();
                        responseStream << response.makeResponseMsg();
                        boost::asio::async_write(
                            *socket, *write_buffer,
                            [socket, request, write_buffer, &response](
                                const boost::system::error_code &ec,
                                size_t bytes_transferred) {
                                std::cout << bytes_transferred << std::endl;
                            });
                        // }
                        // unsigned char buf[3*1024];
                        // ifs.read(buf,3072);
                        // responseStream<<buf;
                        // responseStream.flush();
                        // response.getResponseBodyStrem() << ifs.rdbuf();
                    }
                } else {
                    //请求从rangeResult.first到rangeResult.secode字节范围的数据
                    if (rangeResult.first > rangeResult.second) {
                        response.setCode(416);
                        responseStream << response.makeResponseMsg();
                        boost::asio::async_write(
                            *socket, *write_buffer,
                            [](const boost::system::error_code &ec,
                               size_t bytes_transferred) {});
                    } else {
                        ifs.seekg(rangeResult.first, std::ios::beg);

                        auto readLength =
                            rangeResult.second - rangeResult.first + 1;

                        char readbuf[readLength];
                        ifs.read(readbuf, readLength);
                        response.getResponseBodyStrem() << readbuf;

                        responseStream << response.makeResponseMsg();
                        boost::asio::async_write(
                            *socket, *write_buffer,
                            [](const boost::system::error_code &ec,
                               size_t bytes_transferred) {});
                    }
                }
            }
        }

        return;
    }
}

#endif
