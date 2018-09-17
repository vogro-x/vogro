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

#include "request.hpp"
#include "response.hpp"
#include "status.hpp"

void DefaultErrorHandler(vogro::Request& request, vogro::Response& response) {
    auto code = response.getCode();
    vogro::StatusCodeMap& codeMap = vogro::StatusCodeMap::GetInstance();
    auto pharse = codeMap.getPharseByCode(code);
    std::stringstream ss;
    ss << "<center><h1>" << code << " " << pharse << "</h1><br/>";
    ss << "<a href='https://github.com/Andrewpqc/vogro'>vogro(" << 0.1
       << ")</a></center>";
    response.addBody(ss.str());
    return;
};