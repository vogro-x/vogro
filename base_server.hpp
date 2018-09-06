#ifndef _BASE_SERVER_HPP_
#define _BASE_SERVER_HPP_

#include <boost/asio.hpp>

#include <iostream>
#include <regex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "request.hpp"
#include "response.hpp"
#include "utils.hpp"
#include "vogro_logger.hpp"

namespace vogro {

typedef std::map<
    std::string,
    std::unordered_map<
        std::string, std::function<void(vogro::Response &, vogro::Request &)>>>
    RegistrationCenter;

template <typename socket_type> class ServerBase {
protected:
  boost::asio::io_service io_svc;
  boost::asio::ip::tcp::endpoint endpoint;
  boost::asio::ip::tcp::acceptor acceptor;

  size_t thread_num;
  std::vector<std::thread> threads;

  std::vector<RegistrationCenter::iterator> all_resources;

  RegistrationCenter user_resource;
  RegistrationCenter vogro_resource;

  std::function<void(vogro::Response &, vogro::Request &)>
      default_error_handler;
  std::unordered_map<unsigned short,
                     std::function<void(vogro::Response &, vogro::Request &)>>
      error_handlers;

  Logger<TerminalPolicy> &logger =
      Logger<TerminalPolicy>::getLoggerInstance("vogro.log");

  virtual void accept() {}

public:
  ServerBase(unsigned short port, size_t num_threads)
      : endpoint(boost::asio::ip::tcp::v4(), port), acceptor(io_svc, endpoint),
        thread_num(num_threads) {}

  void runServer() {
    for (auto it = user_resource.begin(); it != user_resource.end(); it++) {
      all_resources.push_back(it);
    }
    for (auto it = vogro_resource.begin(); it != vogro_resource.end(); it++) {
      all_resources.push_back(it);
    }
    logger.LOG_INFO("vogro server is linstening on port:",12345);
    accept();

    // n-1 thread pool

    for (size_t c = 1; c < thread_num; c++) {
      threads.emplace_back([this]() { io_svc.run(); });
    }

    // main thread
    io_svc.run();

    for (auto &t : threads)
      t.join();
  }

  void process_request_and_respond(std::shared_ptr<socket_type> socket) const {

    auto read_buffer = std::make_shared<boost::asio::streambuf>();
    boost::asio::async_read_until(
        *socket, *read_buffer, "\r\n\r\n",
        [this, socket, read_buffer](const boost::system::error_code &ec,
                                    size_t bytes_transferred) {
          if (!ec) {
            size_t total = read_buffer->size();

            std::istream stream(read_buffer.get());

            auto request = std::make_shared<vogro::Request>();
            *request = parse_request(stream);

            size_t num_additional_bytes = total - bytes_transferred;
            if (request->getHeader("Content-Length") != "") {
              boost::asio::async_read(
                  *socket, *read_buffer,
                  boost::asio::transfer_exactly(
                      std::stoull(request->getHeader("Content-Length")) -
                      num_additional_bytes),
                  [this, socket, read_buffer,
                   request](const boost::system::error_code &ec,
                            size_t bytes_transferred) {
                    if (!ec) {
                      // 将指针作为 istream 对象存储到 read_buffer 中
                      request->ReadJSON(std::shared_ptr<std::istream>(
                          new std::istream(read_buffer.get())));
                      
                      respond(socket, request);
                    }
                  });
            } else {
              respond(socket, request);
            }
          }
        });
  }

  vogro::Request parse_request(std::istream &stream) const {
    vogro::Request req;

    // parse request line
    std::string line;
    getline(stream, line);
    line.pop_back(); //去掉换行符
    auto threesome = parse_request_line(line);
    req.setMethod(threesome.first);
    req.setVersion(threesome.second.second);

    std::string url = threesome.second.first;

    std::size_t pos = url.find_first_of('?');
    if (pos == std::string::npos) {
      req.setPath(url);
    } else {
      std::string path = url.substr(0, pos);
      req.setPath(path);

      std::string query_string = url.substr(pos + 1);
      req.setQueryParam(query_string);
    }

    // parse request headers
    while (true) {
      getline(stream, line);
      line.pop_back();

      if (line == "") {
        break;
      }

      auto headerpair = parse_header(line);
      req.addHeader(headerpair.first, headerpair.second);
    }

    return req;
  }

  void respond(std::shared_ptr<socket_type> socket,
               std::shared_ptr<vogro::Request> request) const {
    // 对请求路径和方法进行匹配查找，并生成响应
    vogro::Response response;
    auto write_buffer = std::make_shared<boost::asio::streambuf>();
    std::ostream responseStream(write_buffer.get());
    for (auto res_it : all_resources) {
      if (urlMatch(request->getPath(), res_it->first, request->pathParam)) {
        if (res_it->second.count(request->getMethod()) > 0) {
          // request->pathParam = move(what);

          res_it->second[request->getMethod()](response, *request);

          responseStream << response.makeResponseMsg();

          boost::asio::async_write(
              *socket, *write_buffer,
              [this, socket, request, write_buffer,
               &response](const boost::system::error_code &ec,
                          size_t bytes_transferred) {

                logger.LOG_INFO(request->getMethod(), request->getPath(),
                                response.getCode());

                if (!ec && std::stof(request->getVersion()) > 1.05)
                  process_request_and_respond(socket);
              });
          return;
        }
        response.setCode(405);
        auto got = error_handlers.find(405);
        if (got == error_handlers.end())
          default_error_handler(response, *request);
        else
          got->second(response, *request);

        responseStream << response.makeResponseMsg();
        boost::asio::async_write(
            *socket, *write_buffer,
            [this, socket, request, write_buffer](
                const boost::system::error_code &ec, size_t bytes_transferred) {
              if (!ec && std::stof(request->getVersion()) > 1.05)
                process_request_and_respond(socket);
            });

        logger.LOG_INFO(request->getMethod(), request->getPath(),
                        response.getCode());
        return;
      }
    }
    response.setCode(404);
    auto got = error_handlers.find(404);
    if (got == error_handlers.end()) {
      default_error_handler(response, *request);
    } else {
      got->second(response, *request);
    }
    responseStream << response.makeResponseMsg();
    boost::asio::async_write(
        *socket, *write_buffer,
        [this, socket, request, write_buffer](
            const boost::system::error_code &ec, size_t bytes_transferred) {
          if (!ec && std::stof(request->getVersion()) > 1.05)
            process_request_and_respond(socket);
          return;
        });

    logger.LOG_INFO(request->getMethod(), request->getPath(),
                    response.getCode());

    return;
  }

  void addRoute(std::string userPath, std::string method,
           std::function<void(vogro::Response &, vogro::Request &)> handler) {
    this->user_resource[userPath][method] = handler;
  }

  void customErrorHandler(
      unsigned short code,
      std::function<void(vogro::Response &, vogro::Request &)> handler) {
    this->error_handlers[code] = handler;
  }
};

} // namespace vogro

#endif
