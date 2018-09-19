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

#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include "../request.hpp"
#include "../response.hpp"

namespace vogro {

class Context {

private:
    const std::vector<std::function<void(vogro::Context&)>>& g;
    const std::vector<std::function<void(vogro::Context&)>>& l;

    std::vector<std::function<void(vogro::Context&)>>::const_iterator currentHandler = g.begin();

    std::map<std::string, std::string> values;

public:
    std::shared_ptr<Request> request;
    std::shared_ptr<Response> response;

    Context(std::shared_ptr<Request> req, std::shared_ptr<Response> res,
        const std::vector<std::function<void(vogro::Context&)>>& global,
        const std::vector<std::function<void(vogro::Context&)>>& local)
        : request(req)
        , response(res)
        , g(global)
        , l(local)
    {
    }

    void Next()
    {
        currentHandler++;
        if (currentHandler != g.end()) {
            (*currentHandler)(*this);
            return;
        } else {
            currentHandler = l.begin();
            if (currentHandler != l.end()) {
                (*currentHandler)(*this);
                return;
            } else {
                throw "No Next";
            }
        }
    }


    std::string getValue(const std::string key)
    {
        auto got = this->values.find(key);
        return (got == this->values.end()) ? "" : got->second;
    }

    void setValue(const std::string key, const std::string val)
    {
        this->values[key] = val;
    }
};
}

#endif