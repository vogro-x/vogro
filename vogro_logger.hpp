/************************************************************************
 * Copyright (c) 2018- , Andrewpqc <andrewpqc@mails.ccnu.edu>           *
 * All rights reserved.                                                 *
 *                                                                      *
 * Apache License, Version 2.0                                          *
 * See http://www.apache.org/licenses/ for terms and conditions for use,*
 * reproduction, and distribution.                                      *
 *                                                                      *
 ************************************************************************/

#ifndef __VOGRO_LOGGER_HPP__
#define __VOGRO_LOGGER_HPP__

enum severity_type {
  info = 1,
  debug,
  error,
  warn,
};

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <cstdio>
#include <sstream>
#include <typeinfo>

class BasePolicy {
public:
  virtual void open_ostream() = 0;
  virtual void close_ostream() = 0;
  virtual void write(const std::string &msg) = 0;
};

class FilePolicy : public BasePolicy {
private:
  std::string filename;
  std::unique_ptr<std::ofstream> out_stream;

public:
  FilePolicy(std::string &f) : filename(f), out_stream(new std::ofstream) {
    this->open_ostream();
  }

  void open_ostream() {
    this->out_stream->open(filename, std::ofstream::out | std::ofstream::app);
  }

  void close_ostream() { this->out_stream->close(); }

  void write(const std::string &msg) {
    *(this->out_stream) << msg << std::endl;
  }
  ~FilePolicy() { this->close_ostream(); }
};

class TerminalPolicy : public BasePolicy {
public:
  // placehold 保持接口一致
  TerminalPolicy(std::string &placehold){};

  void open_ostream() {
    // do nothing
  }

  void close_ostream() {
    // do nothing
  }

  void write(const std::string &msg) { std::cout << msg << std::endl; }
};

// remaining impl
class RemotePolicy : public BasePolicy {
private:
  std::string remote_host;
  unsigned short remote_port;

public:
  RemotePolicy(std::string &addr) {
    auto pos = addr.find_first_of(":");
    this->remote_host = addr.substr(0, pos);
    this->remote_port = atoi(addr.substr(pos + 1).c_str());

    this->open_ostream();
  }

  ~RemotePolicy() { this->close_ostream(); }

  void open_ostream() {
    // do nothing
  }

  void close_ostream() {
    // do nothing
  }

  void write(const std::string &msg) {}
};

// color mocro
// #define	CLEARSCREEN() printf("\033[H\033[2J")
#define INITCOLOR(color)                                                       \
  std::string("\033[") + std::string(color) + std::string("m")
#define RED_COLOR "31"
#define GREEN_COLOR "32"
#define YELLOW_COLOR "33"
#define BLUE_COLOR "34"
#define ZERO_COLOR "0"

template <typename policy_type> class Logger {
public:
  static Logger &getLoggerInstance(std::string filename_or_addr) {
    static Logger logger(filename_or_addr);
    return logger;
  }

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  // template<typename...Args>
  template <severity_type severity, typename... Args>
  void PrintLog(const Args &... args) {
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
        (*ssp) << INITCOLOR(GREEN_COLOR) << "<INFO>" << INITCOLOR(ZERO_COLOR)
               << ": "; // with color
      else
        (*ssp) << "<INFO>: "; // no color
      break;

    case severity_type::debug:
      if (this->is_terminal)
        (*ssp) << INITCOLOR(BLUE_COLOR) << "<DEBUG>" << INITCOLOR(ZERO_COLOR)
               << ": ";
      else
        (*ssp) << "<DEBUG>: ";
      break;

    case severity_type::warn:
      if (this->is_terminal)
        (*ssp) << INITCOLOR(YELLOW_COLOR) << "<WARN>" << INITCOLOR(ZERO_COLOR)
               << ": ";
      else
        (*ssp) << "<WARN>: ";
      break;

    case severity_type::error:
      if (this->is_terminal)
        (*ssp) << INITCOLOR(RED_COLOR) << "<ERROR>" << INITCOLOR(ZERO_COLOR)
               << ": ";
      else
        (*ssp) << "<ERROR>: ";
      break;
    };
    this->print_impl(args...);
  }

  template <typename... Args> void LOG_INFO(const Args &... args) {
    this->PrintLog<severity_type::info>(args...);
  }

  template <typename... Args> void LOG_DEBUG(const Args &... args) {
    this->PrintLog<severity_type::debug>(args...);
  }

  template <typename... Args> void LOG_WARN(const Args &... args) {
    this->PrintLog<severity_type::warn>(args...);
  }

  template <typename... Args> void LOG_ERROR(const Args &... args) {
    this->PrintLog<severity_type::error>(args...);
  }

private:
  // policy_type policy;
  std::shared_ptr<policy_type> p;

  std::string filename_or_addr;

  std::shared_ptr<std::stringstream> ssp;

  bool is_terminal = false;

  void print_impl() { // Recursive termination function
    p->write(ssp->str());
    ssp->str(""); // ssp->empty(),ssp->clear() can not clean ssp
  }

  template <typename First, typename... Rest>
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

  Logger(std::string &f) { this->filename_or_addr = f; }
};

#endif