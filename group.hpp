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

#ifndef __GROUP_HPP__
#define __GROUP_HPP__

#include "base.hpp"
#include <iostream>
#include <string>
namespace vogro {

class Group {
private:
    std::string prefix_;
    std::map<std::string, std::unordered_map<std::string, std::vector<std::function<void(vogro::Context&)>>>>& rc_;

public:
    Group(std::string prefix, std::map<std::string, std::unordered_map<std::string, std::vector<std::function<void(vogro::Context&)>>>>& rc)
        : prefix_(prefix)
        , rc_(rc)
    {
    }

    void GET(std::string userPath, std::function<void(vogro::Context&)> handler)
    {
        this->rc_[prefix_ + userPath]["GET"].push_back(handler);
    }

    void POST(std::string userPath, std::function<void(vogro::Context&)> handler)
    {
        this->rc_[prefix_ + userPath]["POST"].push_back(handler);
    }

    void PUT(std::string userPath, std::function<void(vogro::Context&)> handler)
    {
        this->rc_[prefix_ + userPath]["PUT"].push_back(handler);
    }

    void DELETE(std::string userPath, std::function<void(vogro::Context&)> handler)
    {
        this->rc_[prefix_ + userPath]["DELETE"].push_back(handler);
    }
};
} // vogro

#endif
