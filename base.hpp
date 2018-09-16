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

#ifndef _BASE_SERVER_HPP_
#define _BASE_SERVER_HPP_

#include <boost/asio.hpp>

#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "context.hpp"
#include "logger.hpp"
#include "request.hpp"
#include "response.hpp"
#include "static.hpp"
#include "utils.hpp"

namespace vogro {

typedef std::map<std::string, std::unordered_map<std::string, std::vector<std::function<void(vogro::Context&)>>>> RegistrationCenter;

class Group {
private:
    std::string prefix_;
    RegistrationCenter& rc_;

    Logger<TerminalPolicy>& logger = Logger<TerminalPolicy>::getLoggerInstance("vogro.log");

    void addHandler(std::string path, std::string method) {
        logger.LOG_INFO(path,method,"handlers register ok");
    }

    template <typename First, typename... Rest>
    void addHandler(std::string path, std::string method, const First &parm1, const Rest &... parm){
        this->rc_[path][method].push_back(parm1);
        addHandler(path,method,parm...);
    }

public:
    Group(std::string prefix, RegistrationCenter& rc)
        : prefix_(prefix)
        , rc_(rc){}

    template <typename... Args>
    void GET(std::string userPath, const Args &... args)
    {
        if(prefix_.back() == '/') prefix_.pop_back();
        auto path = prefix_+ userPath;
        this->addHandler(path,"GET", args...);
    }

    template <typename... Args>
    void POST(std::string userPath, const Args &... args)
    {   
        if(prefix_.back() == '/') prefix_.pop_back();
        auto path = prefix_+userPath;
         this->addHandler(path,"POST", args...);
    }

    template <typename... Args>
    void PUT(std::string userPath, const Args &... args)
    {   
        if(prefix_.back() == '/') prefix_.pop_back();
        auto path = prefix_+userPath;
         this->addHandler(path,"PUT", args...);
    }

    template <typename... Args>
    void DELETE(std::string userPath, const Args &... args)
    {   
        if(prefix_.back() == '/') prefix_.pop_back();
        auto path = prefix_+userPath;
         this->addHandler(path,"DELETE", args...);
    }

};


template <typename socket_type>
class ServerBase {
protected:
    boost::asio::io_service io_svc;
    boost::asio::ip::tcp::endpoint endpoint;
    boost::asio::ip::tcp::acceptor acceptor;

    size_t thread_num;
    std::vector<std::thread> threads;

    std::vector<RegistrationCenter::iterator> all_resources;

    RegistrationCenter user_resource;
    RegistrationCenter vogro_resource;

    std::function<void(vogro::Request&, vogro::Response&)> default_error_handler;
    std::unordered_map<unsigned short, std::function<void(vogro::Request&, vogro::Response&)>> error_handlers;

    std::vector<std::function<void(vogro::Context&)>> globalMiddlewares;

    Logger<TerminalPolicy>& logger = Logger<TerminalPolicy>::getLoggerInstance("vogro.log");

    virtual void accept() {}

public:
    ServerBase(unsigned short port, size_t num_threads)
        : endpoint(boost::asio::ip::tcp::v4(), port)
        , acceptor(io_svc, endpoint)
        , thread_num(num_threads)
    {
    }

    void runServer()
    {
        for (auto it = user_resource.begin(); it != user_resource.end(); it++)
            all_resources.push_back(it);

        for (auto it = vogro_resource.begin(); it != vogro_resource.end(); it++)
            all_resources.push_back(it);

        logger.LOG_INFO("vogro server is linstening on port:", 12345);

        accept();

        // n-1 thread pool
        for (size_t c = 1; c < thread_num; c++) {
            threads.emplace_back([this]() { io_svc.run(); });
        }

        // main thread
        io_svc.run();

        for (auto& t : threads)
            t.join();
    }

    void process_request_and_respond(std::shared_ptr<socket_type> socket) const
    {
        auto read_buffer = std::make_shared<boost::asio::streambuf>();
        boost::asio::async_read_until(*socket, *read_buffer, "\r\n\r\n",
            [this, socket, read_buffer](const boost::system::error_code& ec,
                size_t bytes_transferred) {
                if (!ec) {
                    size_t total = read_buffer->size();

                    std::istream stream(read_buffer.get());

                    auto request = std::make_shared<vogro::Request>();
                    *request = parse_request(stream);

                    request->setRemoteIP(socket->remote_endpoint().address().to_string());
                    request->setRemotePort(socket->remote_endpoint().port());

                    size_t num_additional_bytes = total - bytes_transferred;
                    if (request->getHeader("Content-Length") != "") {
                        boost::asio::async_read(*socket, *read_buffer,
                            boost::asio::transfer_exactly(
                                std::stoull(request->getHeader("Content-Length")) - num_additional_bytes),
                            [this, socket, read_buffer, request](const boost::system::error_code& ec,
                                size_t bytes_transferred) {
                                if (!ec) {
                                    request->ReadJSON(std::shared_ptr<std::istream>(new std::istream(read_buffer.get())));
                                    respond(socket, request);
                                }
                            });
                    } else {
                        respond(socket, request);
                    }
                }
            });
    }

    vogro::Request parse_request(std::istream& stream) const
    {
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

    void respond(std::shared_ptr<socket_type> socket, std::shared_ptr<vogro::Request> request) const
    {
        auto write_buffer = std::make_shared<boost::asio::streambuf>();
        std::ostream responseStream(write_buffer.get());

        auto response = std::make_shared<vogro::Response>();

        bool matchedOne = false;
        for (auto res_it : all_resources) {
            std::map<std::string, std::string> tempPathParamStoreMap;
            auto matchResult = urlMatch(request->getPath(), res_it->first, tempPathParamStoreMap);
            if ((!matchResult.first) && (matchResult.second)) {
                ServeStatic(*response, *request, responseStream, write_buffer, socket);
                matchedOne = true;
                // break;
                return;
            } else if ((matchResult.first) && (!matchResult.second)) {
                if (res_it->second.count(request->getMethod()) > 0) {
                    request->setPathParam(tempPathParamStoreMap);

                    Context ctx(request, response,
                        globalMiddlewares,
                        res_it->second[request->getMethod()]);

                    if (globalMiddlewares.begin() != globalMiddlewares.end()) {
                        (*(globalMiddlewares.begin()))(ctx); 
                    } else {
                        if (res_it->second[request->getMethod()].begin() != res_it->second[request->getMethod()].end()) {
                            (*(res_it->second[request->getMethod()].begin()))(ctx);
                        }
                    }

                    matchedOne = true;
                    break;
                } else {
                    response->setCode(405);
                    auto got = error_handlers.find(405);
                    if (got == error_handlers.end())
                        default_error_handler(*request, *response);
                    else
                        got->second(*request, *response);
                    matchedOne = true;
                    break;
                }
            }
        }

        if (!matchedOne) {
            response->setCode(404);
            auto got = error_handlers.find(404);
            if (got == error_handlers.end())
                default_error_handler(*request, *response);
            else
                got->second(*request, *response);
        }
        responseStream << response->makeResponseMsg();

        boost::asio::async_write(
            *socket, *write_buffer,
            [this, socket, request, write_buffer, &response](
                const boost::system::error_code& ec, size_t bytes_transferred) {

                logger.LOG_DEBUG(request->getRemoteIP(), request->getMethod(), request->getPath(), response->getCode());

                if (!ec && std::stof(request->getVersion()) > 1.05)
                    process_request_and_respond(socket);
            });
        return;
    }

    void addRoute(std::string userPath, std::string method, std::function<void(vogro::Context&)> handler){
        this->user_resource[userPath][method].push_back(handler);
    }

    void Get(std::string userPath, std::function<void(vogro::Context&)> handler) {
        this->user_resource[userPath]["GET"].push_back(handler);
    }

    void POST(std::string userPath, std::function<void(vogro::Context&)> handler) {
        this->user_resource[userPath]["POST"].push_back(handler);
    }

    void PUT(std::string userPath, std::function<void(vogro::Context&)> handler) {
        this->user_resource[userPath]["PUT"].push_back(handler);
    }

    void DELETE(std::string userPath, std::function<void(vogro::Context&)> handler) {
        this->user_resource[userPath]["DELETE"].push_back(handler);
    }

    std::shared_ptr<vogro::Group> makeGroup(std::string prefix,std::function<void(vogro::Context&)> handler) {
        auto group = std::shared_ptr<vogro::Group>(new vogro::Group(prefix,user_resource));
        return group;
    }

    void customErrorHandler(unsigned short code, std::function<void(vogro::Request&, vogro::Response&)> handler) {
        this->error_handlers[code] = handler;
    }

    void Use(std::function<void(vogro::Context&)> middleware) {
        this->globalMiddlewares.push_back(middleware);
    }


};

} // namespace vogro

#endif
