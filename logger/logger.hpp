/************************************************************************ 
 * Copyright (c) 2018- , anderwpqc|Andrewpqc <andrewpqc@mails.ccnu.edu> *
 * All rights reserved.                                                 * 
 *                                                                      *
 * Apache License, Version 2.0. See http://www.apache.org/licenses/     *
 ************************************************************************/

#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <cstdio>
#include "severity.hpp"

#define INITCOLOR(color) std::string("\033[") + std::string(color) + std::string("m")
#define RED_COLOR "31"
#define GREEN_COLOR "32"
#define YELLOW_COLOR "33"
#define BLUE_COLOR "34"
#define ZERO_COLOR "0"

namespace vogro {
namespace logger{

template <typename policy_type>
class Logger {

  public:
    static Logger &getLoggerInstance(std::string filename_or_addr) {
        static Logger logger(filename_or_addr);
        return logger;
    }

    std::string getCurrentTime() {
        time_t t;
        time(&t);
        struct tm *tmp_time = localtime(&t);
        char s[100];
        strftime(s, sizeof(s), "%04Y/%02m/%02d %H:%M:%S", tmp_time);
        return static_cast<std::string>(s);
    }

    // template<typename...Args>
    template <severity_type severity, typename... Args>
    void PrintLog(Args& ... args) {
        // init p and ssp only once
        if (!p) {
            p = std::shared_ptr<policy_type>(new policy_type(this->filename_or_addr));
            std::string t = typeid(decltype(*p)).name();
            if (t == "14TerminalPolicy") {
                this->is_terminal = true;
            }
        }
        if (!ssp) {
            ssp = std::shared_ptr<std::stringstream>(new std::stringstream);
        }

        (*ssp) << "[" << getCurrentTime() << "] ";
        switch (severity) {

        case severity_type::info:
            if (this->is_terminal) 
                (*ssp) << INITCOLOR(GREEN_COLOR) << "<INFO>" << INITCOLOR(ZERO_COLOR) << ": "; //with color
            else
                (*ssp) << "<INFO>: "; // no color
            break;

        case severity_type::debug:
            if (this->is_terminal) 
                (*ssp) << INITCOLOR(BLUE_COLOR) << "<DEBUG>" << INITCOLOR(ZERO_COLOR) << ": ";
            else
                (*ssp) << "<DEBUG>: ";
            break;

        case severity_type::warn:
            if (this->is_terminal) 
                (*ssp) << INITCOLOR(YELLOW_COLOR) << "<WARN>" << INITCOLOR(ZERO_COLOR) << ": ";
            else
                (*ssp) << "<WARN>: ";
            break;

        case severity_type::error:
            if (this->is_terminal) 
                (*ssp) << INITCOLOR(RED_COLOR) << "<ERROR>" << INITCOLOR(ZERO_COLOR) << ": ";
            else
                (*ssp) << "<ERROR>: ";
            break;

        };
        this->print_impl(args...);
    }

    // void LOG_ERROR()

  private:
    // policy_type policy;
    std::shared_ptr<policy_type> p;

    std::string filename_or_addr;

    std::shared_ptr<std::stringstream> ssp;

    bool is_terminal = false;

    void print_impl() { // Recursive termination function
        p->write(ssp->str());
        ssp->str(""); //ssp->empty(),ssp->clear() can not clean ssp
    }

    template <typename First, typename... Rest>
    void print_impl(First &parm1,  Rest &... parm) {
        (*ssp) << parm1 << " ";
        print_impl(parm...);
    }

    Logger(std::string &f) {
        this->filename_or_addr = f;
    }

    Logger & operator = (const Logger &);
};
}
}



#endif
