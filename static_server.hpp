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
#include <vector>
// https://github.com/golang/go/blob/42257a262c94d839364113f2dbf4057731971fc1/src/net/http/fs.go#L713
// https://www.rfc-editor.org/rfc/rfc7233.txt

// Ranges : bytes = xxx -xxx

enum Cond {
    CondNone = 1,
    CondTrue,
    CondFalse,
};

struct range {
    unsigned long start;
    unsigned long len;
};

// parseRange to parse ranges. get the start position and the range length and
// store them to the range struct, if there are multi ranges in Ranges, them
// store each range struct to a range vector,and return the range vector.
std::vector<range> parseRange(std::string Ranges, const size_t total_size) {
    std::vector<range> vr;

    // if startwith 'bytes '
    if (!Ranges.rfind("bytes=", 0) == 0) return vr;

    Ranges = Ranges.substr(6);

    std::vector<std::string> splitResult;
    splitResult = split(Ranges, ',');

    for (auto s : splitResult) {
        // parse s
        s = trim(s);

        auto pos = s.find_first_of('-');
        std::string s_start = s.substr(0, pos);
        std::string s_end = s.substr(pos + 1);

        long long n_start = (s_start == "") ? -1 : std::stoll(s_start);
        long long n_end = (s_end == "") ? -1 : std::stoll(s_end);

        unsigned long start = 0;
        unsigned long len = 0;

        range r;
        if (n_start == -1 && n_end != -1) {
            if (n_end > total_size) n_end = total_size;
            r.start = total_size - n_end;
            r.len = total_size - r.start;
        } else if (n_start != -1 && n_end == -1) {
            if (n_start >= total_size) throw "RequestedRangeNotSatisfiable";
            r.start = n_start;
            r.len = total_size - r.start;
        } else if (n_start != -1 && n_end != -1) {
            if (n_start > n_end) throw "RequestedRangeNotSatisfiable";
            if (n_end >= total_size) n_end = total_size - 1;
            r.start = n_start;
            r.len = n_end - r.start + 1;
        } else {
            // n_start ==-1 && n_end== -1
            throw "RequestedRangeNotSatisfiable";
        }
        vr.push_back(r);
    }

    return vr;
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
// of the filename,and return (GmtTime-ModifiyTime) in seconds. that is to say,
// if Gmt time string is smaller than(earlier) modifiy time. it returns a
// negative number, otherwise returns a positive number. if the two time is
// equal, it returns zero.
double compareTime(const std::string &GmtString,
                   const struct timespec &lastModifiedTime) {
    tm tm_;
    time_t t_;
    char buf[128] = {0};

    strcpy(buf, GmtString.c_str());
    strptime(buf, "%a, %d %b %Y %H:%M:%S GMT", &tm_);
    tm_.tm_isdst = -1;
    t_ = mktime(&tm_);
    t_ += 3600;

    return difftime(t_, lastModifiedTime.tv_sec);  //这里只精确到秒
}

// ETag值匹配才发送资源
Cond checkIfMatch(vogro::Request &request, const std::string &filename) {
    auto IfMatchHeadString = request.getHeader("If-Match");
    auto im = trim(IfMatchHeadString);
    if (im == "") return CondNone;
    if (im == "*") return CondTrue;
    if (im == getEtag(filename)) return CondTrue;
    return CondFalse;
}

Cond checkIfUnmodifiedSince(vogro::Request &request,
                            const struct timespec &lastModifiedTime) {
    auto IfUnmodifiedSinceString = request.getHeader("If-Unmodified-Since");
    auto iums = trim(IfUnmodifiedSinceString);
    if (iums == "") return CondNone;
    if (compareTime(iums, lastModifiedTime) > 0) return CondTrue;
    return CondFalse;
}

Cond checkIfNoneMatch(vogro::Request &request, const std::string &filename) {
    auto IfNoneMatchString = request.getHeader("If-None-Match");
    auto inm = trim(IfNoneMatchString);
    if (inm == "") return CondNone;
    if (inm == "*" || inm == getEtag(filename)) return CondFalse;
    return CondTrue;
}

Cond checkIfModifiedSince(vogro::Request &request,
                          const struct timespec &lastModifiedTime) {
    if (request.getMethod() != "GET" && request.getMethod() != "HEAD")
        return CondNone;
    auto ims = request.getHeader("If-Modified-Since");
    if (ims == "") return CondNone;
    if (compareTime(ims, lastModifiedTime) > 0) return CondFalse;
    return CondTrue;
}

// If-Range's Value is ETag or Date
// if mathced, a range request,otherwise to return all the content to the client
// more infomation, see
// https://developer.mozilla.org/zh-CN/docs/Web/HTTP/Headers/If-Range
Cond checkIfRange(vogro::Request &request, const std::string &filename,
                  const struct timespec &lastModifiedTime) {
    if (request.getMethod() != "GET" && request.getMethod() != "HEAD")
        return CondNone;

    auto ir = request.getHeader("If-Range");
    if (ir == "") return CondNone;

    // ir value is endwith 'GMT'
    std::string const ending = "GMT";
    if (std::equal(ending.rbegin(), ending.rend(), ir.rbegin())) {
        // datetime
        return (compareTime(ir, lastModifiedTime) == 0) ? CondTrue : CondFalse;
    } else {
        // etag
        if (ir == "*") return CondTrue;
        if (ir == getEtag(filename)) return CondTrue;
        return CondFalse;
    }
}

std::pair<bool, std::string> CheckPreconditons(
    vogro::Response &response, vogro::Request &request,
    const struct timespec &lastModifiedTime, const std::string &filename) {
    auto checkResult = checkIfMatch(request, filename);
    if (checkResult == CondNone) {
        checkResult = checkIfUnmodifiedSince(request, lastModifiedTime);
    }

    if (checkResult == CondFalse) {
        // return StatusPreconditionFailed to client
        response.setCode(412, "Precondition Failed");
        return std::make_pair(true, "");
    }

    switch (checkIfNoneMatch(request, filename)) {
        case CondFalse:
            if (request.getMethod() == "GET" || request.getMethod() == "HEAD") {
                response.setCode(304, "Not Modified");
            } else {
                response.setCode(412, "Precondition Failed");
            }
            return std::make_pair(true, "");
            break;
        case CondNone:
            if (checkIfModifiedSince(request, lastModifiedTime) == CondFalse) {
                response.setCode(304, "Not Modified");
                return std::make_pair(true, "");
            }
            break;
    }

    auto rangeHeader = request.getHeader("Range");
    if (rangeHeader != "" &&
        checkIfRange(request, filename, lastModifiedTime) == CondFalse) {
        rangeHeader = "";
    }
    return make_pair(false, rangeHeader);
}

/*****
 *
 * int main()
{
     fstream _file;
     _file.open(FILENAME,ios::in);
     if(!_file)
     {
         cout<<FILENAME<<"没有被创建";
      }
      else
      {
          cout<<FILENAME<<"已经存在";
      }
      return 0;
}
 *
 *
 *
 *
 *
 *
 *
 *
 */

template <typename socket_type>
void ServeStatic(vogro::Response &response, vogro::Request &request,
                 std::ostream &responseStream,
                 std::shared_ptr<boost::asio::streambuf> &write_buffer,
                 socket_type socket) {
    // get current work dir
    char *buffer;
    if ((buffer = getcwd(NULL, 0)) == NULL) {

        // return 500 to client
        perror("getcwd error");
        exit(EXIT_FAILURE);
    }

    // get filename
    auto cwd = std::string(buffer);
    auto path = request.getPath();
    auto filepath = cwd + path;

    std::ifstream _file;
    _file.open(filepath, std::iso::in);
    if (!_file) {
        // file not found. return 404 to client
        return
    }

    // get filesize
    _file.seekg(0, std::ios::end);
    int64_t filesize = ifs.tellg();
    _file.seekg(0, std::ios::beg);

    // get last modified time of the request file
    struct stat statbuf;
    stat(file, &statbuf);
    struct timespec lastModifiedTime = statbuf.st_mtim;

    // get file type
    auto ext = getFileExtension(filepath);
    MimeTypeMap &mime = MimeTypeMap::GetInstance();
    auto type = mime.getMimeTypeByExt(ext);

    if (type == "") {
        response.setCode(415);

        // media type not support, return 415 to client
    }

    response.addHeader("Connection", "Keep-Alive");
    response.addHeader("Content-Type", type);
    response.addHeader("Accept-Ranges", "bytes");

   

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
                boost::asio::async_write(*socket, *write_buffer,
                                         [](const boost::system::error_code &ec,
                                            size_t bytes_transferred) {});

            } else {
                response.setCode(206);

                std::stringstream contentRange;
                contentRange << "bytes " << fileLength - rangeResult.second
                             << "-" << fileLength - 1 << '/' << fileLength;
                response.addHeader("Content-Range", contentRange.str());

                ifs.seekg(-rangeResult.second, std::ios::end);

                response.getResponseBodyStrem() << ifs.rdbuf();
                responseStream << response.makeResponseMsg();
                boost::asio::async_write(*socket, *write_buffer,
                                         [](const boost::system::error_code &ec,
                                            size_t bytes_transferred) {});
            }
        } else if (rangeResult.first != -1 && rangeResult.second == -1) {
            //请求从rangeResult.first到文件结尾的数据

            if (rangeResult.first >= fileLength) {
                response.setCode(416);
                responseStream << response.makeResponseMsg();
                boost::asio::async_write(*socket, *write_buffer,
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
                boost::asio::async_write(*socket, *write_buffer,
                                         [](const boost::system::error_code &ec,
                                            size_t bytes_transferred) {});
            } else {
                ifs.seekg(rangeResult.first, std::ios::beg);

                auto readLength = rangeResult.second - rangeResult.first + 1;

                char readbuf[readLength];
                ifs.read(readbuf, readLength);
                response.getResponseBodyStrem() << readbuf;

                responseStream << response.makeResponseMsg();
                boost::asio::async_write(*socket, *write_buffer,
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
