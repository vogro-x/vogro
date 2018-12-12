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

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#ifndef __COMMOM_HPP__
#define __COMMOM_HPP__

namespace vogro {

// see https://zh.wikipedia.org/wiki/HTTP状态码

// 1xx
#define CodeContinue_100 100
#define CodeSwitchingProtocols_101 101
#define CodeProcessing 102

// 2xx
#define CodeOK_200 200
#define CodeCreated_201 201
#define CodeAccepted_202 202
#define CodeNonAuthoritativeInformation_203 203
#define CodeNoContent_204 204
#define CodeResetContent_205 205
#define CodePartialContent_206 206
#define CodeMultiStatus_207 207
#define CodeAlreadyReported_208 208
#define CodeIMUsed_226 226

// 3xx
#define CodeMultipleChoices_300 300
#define CodeMovedPermanently_301 301
#define CodeFound_302 302
#define CodeSeeOther_303 303
#define CodeNotModified_304 304
#define CodeUseProxy_305 305
#define CodeSwitchProxy_306 306
#define CodeTemporaryRedirect_307 307
#define CodePermanentRedirect_308 308

// 4xx
#define CodeBadRequest_400 400
#define CodeUnauthorized_401 401
#define CodePaymentRequired_402 402
#define CodeForbidden_403 403
#define CodeNotFound_404 404
#define CodeMethodNotAllowed_405 405
#define CodeNotAcceptable_406 406
#define CodeProxyAuthenticationRequired_407 407
#define CodeRequestTimeout_408 408
#define CodeConflict_409 409
#define CodeGone_410 410
#define CodeLengthRequired_411 411
#define CodePreconditionFailed_412 412
#define CodeRequestEntityTooLarge_413 413
#define CodeRequestURITooLong_414 414
#define CodeUnsupportedMediaType_415 415
#define CodeRequestedRangeNotSatisfiable_416 416
#define CodeExpectationFailed_417 417
#define CodeIMATeapot_418 418
#define CodeEnhanceYourCaim_420 420
#define CodeMisdirectedRequest_421 421
#define CodeUnprocessableEntity_422 422
#define CodeLocked_423 423
#define CodeFailedDependency_424 424
#define CodeUnorderedCollection_425 425
#define CodeUpgradeRequired_426 426
#define CodePreconditionRequired_428 428
#define CodeTooManyRequests_429 429
#define CodeRequestHeaderFieldsTooLarge_431 431
#define CodeNoResponse_444 444
#define CodeBlockedbyWindowsParentalControls_450 450
#define CodeUnavailableForLegalReasons_451 451
#define CodeRequestHeaderTooLarge_494 494

// 5xx
#define CodeInternalServerError_500 500
#define CodeNotImplemented_501 501
#define CodeBadGateway_502 502
#define CodeServiceUnavailable_503 503
#define CodeGatewayTimeout_504 504
#define CodeHTTPVersionNotSupported_505 505
#define CodeHTTPVariantAlsoNegotiates_506 506
#define CodeHTTPInsufficientStorage_507 507
#define CodeHTTPLoopDetected_508 508
#define CodeHTTPNotExtended_510 510
#define CodeHTTPNetworkAuthenticationRequired_511 511

    class StatusCodeMap {
    private:
        StatusCodeMap() {
            codePhrase[100] = "Continue";
            codePhrase[101] = "Switching Protocols";
            codePhrase[102] = "Processing";

            codePhrase[200] = "OK";
            codePhrase[201] = "Created";
            codePhrase[202] = "Accepted";
            codePhrase[203] = "Non-Authoritative Information";
            codePhrase[204] = "No Content";
            codePhrase[205] = "Reset Content";
            codePhrase[206] = "Partial Content";
            codePhrase[207] = "Multi-Status";
            codePhrase[208] = "Already Reported";
            codePhrase[226] = "IM Used";

            codePhrase[300] = "Multiple Choices";
            codePhrase[301] = "Moved Permanently";
            codePhrase[302] = "Found";
            codePhrase[303] = "See Other";
            codePhrase[304] = "Not Modified";
            codePhrase[305] = "Use Proxy";
            codePhrase[306] = "Switch Proxy";
            codePhrase[307] = "Temporary Redirect";
            codePhrase[308] = "Permanent Redirect";

            codePhrase[400] = "Bad Request";
            codePhrase[401] = "Unauthorized";
            codePhrase[402] = "Payment Required";
            codePhrase[403] = "Forbidden";
            codePhrase[404] = "Not Found";
            codePhrase[405] = "Method Not Allowed";
            codePhrase[406] = "Not Acceptable";
            codePhrase[407] = "Proxy Authentication Required";
            codePhrase[408] = "Request Timeout";
            codePhrase[409] = "Conflict";
            codePhrase[410] = "Gone";
            codePhrase[411] = "Length Required";
            codePhrase[412] = "Precondition Failed";
            codePhrase[413] = "Request Entity Too Large";
            codePhrase[414] = "Request-URI Too Long";
            codePhrase[415] = "Unsupported Media Type";
            codePhrase[416] = "Requested Range Not Satisfiable";
            codePhrase[417] = "Expectation Failed";
            codePhrase[418] = "I'm a teapot";
            codePhrase[420] = "Enhance Your Caim";
            codePhrase[421] = "Misdirected Request";
            codePhrase[422] = "Unprocessable Entity";
            codePhrase[423] = "Locked";
            codePhrase[424] = "Failed Dependency";
            codePhrase[425] = "Unordered Collection";
            codePhrase[426] = "Upgrade Required";
            codePhrase[428] = "Precondition Required";
            codePhrase[429] = "Too Many Requests";
            codePhrase[431] = "Request Header Fields Too Large";
            codePhrase[444] = "No Response";
            codePhrase[450] = "Blocked by Windows Parental Controls";
            codePhrase[451] = "Unavailable For Legal Reasons";
            codePhrase[494] = "Request Header Too Large";

            codePhrase[500] = "Internal Server Error";
            codePhrase[501] = "Not Implemented";
            codePhrase[502] = "Bad Gateway";
            codePhrase[503] = "Service Unavailable";
            codePhrase[504] = "Gateway Timeout";
            codePhrase[505] = "HTTP Version Not Supported";
            codePhrase[506] = "Variant Also Negotiates";
            codePhrase[507] = "Insufficient Storage";
            codePhrase[508] = "Loop Detected";
            codePhrase[510] = "Not Extended";
            codePhrase[511] = "Network Authentication Required";
        }

        std::unordered_map<unsigned long, std::string> codePhrase;

    public:
        StatusCodeMap(const StatusCodeMap &) = delete;

        StatusCodeMap &operator=(const StatusCodeMap &) = delete;

        std::string getPharseByCode(unsigned long code) {
            std::unordered_map<unsigned long, std::string>::const_iterator got =
                    codePhrase.find(code);
            return (got == codePhrase.end()) ? "" : got->second;
        }

        static StatusCodeMap &GetInstance() {
            static StatusCodeMap instance;  //局部静态变量
            return instance;
        }
    };
}

#endif