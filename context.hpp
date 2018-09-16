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

#include "request.hpp"
#include "response.hpp"

namespace vogro {

class Context {

private:
    std::vector<std::function<void(vogro::Context&)>>::const_iterator g;
    std::vector<std::function<void(vogro::Context&)>>::const_iterator ge;
    std::vector<std::function<void(vogro::Context&)>>::iterator l;
    std::vector<std::function<void(vogro::Context&)>>::iterator le;

    std::vector<std::function<void(vogro::Context&)>>::const_iterator currentHandler = g;

public:
    std::shared_ptr<Request> request;
    std::shared_ptr<Response> response;

    Context(std::shared_ptr<Request> req, std::shared_ptr<Response> res,
        std::vector<std::function<void(vogro::Context&)>>::const_iterator&& globalfirst,
        std::vector<std::function<void(vogro::Context&)>>::const_iterator&& globalend,
        std::vector<std::function<void(vogro::Context&)>>::iterator&& localfirst,
        std::vector<std::function<void(vogro::Context&)>>::iterator&& localend)
        : request(req)
        , response(res)
        , g(globalfirst)
        , ge(globalend)
        , l(localfirst)
        , le(localend)
    {
    }

    void Next()
    {
        currentHandler++;
        if (currentHandler != ge) {
            (*currentHandler)(*this);
            return;
        } else {
            currentHandler = l;
            if (currentHandler != le) {
                (*currentHandler)(*this);
                return;
            } else {
                throw "No Next";
            }
        }
    }
};
}

#endif