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

#ifndef _VOGRO_GROUP_HPP_
#define _VOGRO_GROUP_HPP_

#include "context.hpp"
#include "logger.hpp"

#include <map>
#include <functional>
#include <unordered_map>

namespace vogro {

    using RegistrationCenter =  std::map<std::string, std::unordered_map<std::string,
            std::vector<std::function<void(vogro::Context &)>>>>;

    class Group {
    private:
        std::string prefix_;
        RegistrationCenter &rc_;
        const std::function<void(vogro::Context &)> groupGlobalHandler_ = nullptr;

        Logger<TerminalPolicy> &logger = Logger<TerminalPolicy>::getLoggerInstance("vogro.log");


        void addHandler(const std::string &path, const std::string &method) {
            logger.LOG_INFO(path, method, "handlers register ok");
        }

        template<typename First, typename... Rest>
        void addHandler(const std::string &path, const std::string &method, const First &parm1,
                        const Rest &... parm) {
            this->rc_[path][method].push_back(parm1);
            addHandler(path, method, parm...);
        }

    public:
        Group(
                const std::string &prefix,
                RegistrationCenter &rc,
                const std::function<void(vogro::Context &)> groupGlobalHandler
        ) : prefix_(prefix), rc_(rc), groupGlobalHandler_(groupGlobalHandler) {}

        Group(
                const std::string &prefix,
                RegistrationCenter &rc
        ) : prefix_(prefix), rc_(rc) {}

        template<typename... Args>
        void Get(const std::string &userPath, const Args &... args) {
            auto path = prefix_ + userPath;

            if (this->groupGlobalHandler_ != nullptr)
                this->rc_[path]["GET"].push_back(groupGlobalHandler_);

            this->addHandler(path, "GET", args...);
        }

        template<typename... Args>
        void Post(const std::string &userPath, const Args &... args) {
            auto path = prefix_ + userPath;

            if (this->groupGlobalHandler_ != nullptr)
                this->rc_[path]["POST"].push_back(groupGlobalHandler_);

            this->addHandler(path, "POST", args...);
        }

        template<typename... Args>
        void Put(const std::string &userPath, const Args &... args) {
            auto path = prefix_ + userPath;

            if (this->groupGlobalHandler_ != nullptr)
                this->rc_[path]["PUT"].push_back(groupGlobalHandler_);

            this->addHandler(path, "PUT", args...);
        }

        template<typename... Args>
        void Delete(const std::string &userPath, const Args &... args) {
            auto path = prefix_ + userPath;

            if (this->groupGlobalHandler_ != nullptr)
                this->rc_[path]["DELETE"].push_back(groupGlobalHandler_);

            this->addHandler(path, "DELETE", args...);
        }

    }; // class Group
} // namespace vogro
#endif