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

//TODO
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
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <unistd.h>

// https://github.com/golang/go/blob/42257a262c94d839364113f2dbf4057731971fc1/src/net/http/fs.go#L713

int is_file_exist(const char* path){
    if(path == NULL)
        return -1;
    if(access(path,F_OK)==0){
        return 0;
    }
    return -1;
}

template <typename socket_type>
void ServeStatic(vogro::Response &response, vogro::Request &request,
                 std::ostream &responseStream,
                 std::shared_ptr<boost::asio::streambuf> &write_buffer,
                 socket_type socket) {
    char *buffer;
    if ((buffer = getcwd(NULL, 0)) == NULL) {
        perror("getcwd error");
    }

    auto cwd = std::string(buffer);
    auto path = request.getPath();
    std::cout << "cwd:" << cwd << std::endl;
    std::cout << "path" << path << std::endl;

    auto filepath = cwd + path;

    auto ext = getFileExtension(filepath);
    MimeTypeMap &mime = MimeTypeMap::GetInstance();
    auto type = mime.getMimeTypeByExt(ext);
    if (type == "") {
        response.setCode(415);
        responseStream << response.makeResponseMsg();
        boost::asio::async_write(
            *socket, *write_buffer,
            [](const boost::system::error_code &ec, size_t bytes_transferred) {
                //
            });
            return ;
    } else {
        if(is_file_exist(filepath.c_str())){
            response.setCode(404);
            responseStream << response.makeResponseMsg();
            boost::asio::async_write(*socket, *write_buffer,
                                     [](const boost::system::error_code &ec,
                                        size_t bytes_transferred) {
                                         //
                                     });
            return;
        }
        
        response.addHeader("Connection", "Keep-Alive");
        response.addHeader("Content-Type", type);
       
        
        std::ifstream ifs(filepath, std::ifstream::binary);
        
        response.getResponseBodyStrem() <<ifs.rdbuf();
        
        responseStream << response.makeResponseMsg();
        boost::asio::async_write(
            *socket, *write_buffer,
            [](const boost::system::error_code &ec, size_t bytes_transferred) {
                //
            });       
        return;
    }
    
}

#endif