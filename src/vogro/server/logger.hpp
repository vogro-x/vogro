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

#ifndef __VOGRO_LOGGER_HPP__
#define __VOGRO_LOGGER_HPP__


#include <mutex>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>
#include <typeinfo>
#include "utils.hpp"

namespace vogro {

    enum severity_type {
        info = 1,
        debug,
        error,
        warn,
    };


    class BasePolicy {
    public:
        virtual void open_ostream() = 0;

        virtual void close_ostream() = 0;

        virtual void write(const std::string &msg) = 0;

        virtual ~BasePolicy() {};
    }; // class BasePolicy

    class FilePolicy : public BasePolicy {
    private:
        std::string filename;
        std::unique_ptr<std::ofstream> out_stream;

    public:
        explicit FilePolicy(const std::string &f) : filename(f), out_stream(new std::ofstream) {
            this->open_ostream();
        }

        void open_ostream() override {
            this->out_stream->open(filename,
                                   std::ofstream::out | std::ofstream::app);
        }

        void close_ostream() override { this->out_stream->close(); }

        void write(const std::string &msg) override {
            *(this->out_stream) << msg << std::endl;
        }

        ~FilePolicy() override { this->close_ostream(); }
    }; // class FilePolicy

    class TerminalPolicy : public BasePolicy {
    public:
        // placehold 保持接口一致
        explicit TerminalPolicy(std::string &placehold) {};

        void open_ostream() override {
            // do nothing
        }

        void close_ostream() override {
            // do nothing
        }

        void write(const std::string &msg) override {
            std::cout << msg << std::endl;
        }
    }; //class TerminalPolicy

// remaining impl
    class RemotePolicy : public BasePolicy {
    private:
        std::string remote_host;
        unsigned short remote_port;

    public:
        explicit RemotePolicy(const std::string &addr) {
            auto pos = addr.find_first_of(':');
            this->remote_host = addr.substr(0, pos);
            this->remote_port = (unsigned short) atoi(addr.substr(pos + 1).c_str());

            this->open_ostream();
        }

        ~RemotePolicy() override { this->close_ostream(); }

        void open_ostream() override {
            // do nothing
        }

        void close_ostream() override {
            // do nothing
        }

        void write(const std::string &msg) override {}
    }; //class RemotePolicy


    template<typename policy_type>
    class Logger {
    public:
        static Logger &getLoggerInstance(std::string filename_or_addr) {
            static Logger logger(filename_or_addr);
            return logger;
        }

        Logger(const Logger &) = delete;

        Logger &operator=(const Logger &) = delete;

        // template<typename...Args>
        template<severity_type severity, typename... Args>
        void PrintLog(const Args &... args) {
            // init p and ssp only once
            if (!p) {
                p = std::shared_ptr<policy_type>(
                        new policy_type(this->filename_or_addr));
                if (typeid(TerminalPolicy) == typeid(*p)) {
                    this->is_terminal = true;
                }
            }
            if (!ssp) {
                ssp = std::make_shared<std::stringstream>();
            }

            write_mutex.lock();
            (*ssp) << "[" << getCurrentTime() << "] ";
            switch (severity) {
                case severity_type::info:
                    if (this->is_terminal)
                        (*ssp) << INITCOLOR(GREEN_COLOR) << "<INFO>"
                               << INITCOLOR(ZERO_COLOR) << ": ";  // with color
                    else
                        (*ssp) << "<INFO>: ";  // no color
                    break;

                case severity_type::debug:
                    if (this->is_terminal)
                        (*ssp) << INITCOLOR(BLUE_COLOR) << "<DEBUG>"
                               << INITCOLOR(ZERO_COLOR) << ": ";
                    else
                        (*ssp) << "<DEBUG>: ";
                    break;

                case severity_type::warn:
                    if (this->is_terminal)
                        (*ssp) << INITCOLOR(YELLOW_COLOR) << "<WARN>"
                               << INITCOLOR(ZERO_COLOR) << ": ";
                    else
                        (*ssp) << "<WARN>: ";
                    break;

                case severity_type::error:
                    if (this->is_terminal)
                        (*ssp) << INITCOLOR(RED_COLOR) << "<ERROR>"
                               << INITCOLOR(ZERO_COLOR) << ": ";
                    else
                        (*ssp) << "<ERROR>: ";
                    break;
            };
            this->print_impl(args...);
            write_mutex.unlock();
        }

        template<typename... Args>
        void LOG_INFO(const Args &... args) {
            this->PrintLog<severity_type::info>(args...);
        }

        template<typename... Args>
        void LOG_DEBUG(const Args &... args) {
            this->PrintLog<severity_type::debug>(args...);
        }

        template<typename... Args>
        void LOG_WARN(const Args &... args) {
            this->PrintLog<severity_type::warn>(args...);
        }

        template<typename... Args>
        void LOG_ERROR(const Args &... args) {
            this->PrintLog<severity_type::error>(args...);
        }

    private:
        std::mutex write_mutex;
        // policy_type policy;
        std::shared_ptr<policy_type> p;

        std::string filename_or_addr;

        std::shared_ptr<std::stringstream> ssp;

        bool is_terminal = false;

        void print_impl() {  // Recursive termination function
            p->write(ssp->str());
            ssp->str("");  // ssp->empty(),ssp->clear() can not clean ssp
        }

        template<typename First, typename... Rest>
        void print_impl(const First &parm1, const Rest &... parm) {
            (*ssp) << parm1 << " ";
            print_impl(parm...);
        }

        std::string getCurrentTime() {
            time_t t;
            time(&t);
            struct tm *tmp_time = localtime(&t);
            char s[100];
            strftime(s, sizeof(s), "%04Y/%02m/%02d %H:%M:%S", tmp_time);
            return static_cast<std::string>(s);
        }

        explicit Logger(std::string &f) { this->filename_or_addr = f; }
    }; // class Logger
} // namespace vogro
#endif