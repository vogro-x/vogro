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

// https://github.com/golang/go/blob/42257a262c94d839364113f2dbf4057731971fc1/src/net/http/fs.go#L713

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
    } else {
        if (request.getHeader("Range")!=""){
            response.setCode(206);
        }
        response.addHeader("Connection", "Keep-Alive");
        response.addHeader("Content-Type", type);
        response.addHeader
        // send header first

        std::ifstream ifs;
        ifs.open(filepath, std::ifstream::in);
        if (ifs) {
            ifs.seekg(0, std::ios::end);
            size_t length = ifs.tellg();

            ifs.seekg(0, std::ios::beg);
            std::stringstream ss;
            ss << ifs.rdbuf();
            std::string s;
            ss >> s;
            response.addBody(s);
            ifs.close();
        }
    }
}

#endif