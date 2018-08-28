/***********************************
[DEPRECATED] this file is deprecated now.

at the very begining, I want to convert the handler path (that provided by this 
framework's users in the handler registration function) into regular expression 
format of Named Captures, such as bellow:

handler path                 named capture regular expression

/abc/{int:id}/bala/  ---- > ^/abc/(?P<id>\\d+)/bala/?$
/abc/{str:name}/bala/  ---- > ^/abc/(?P<name>\\w+)/bala/?$

then store the named capture regular expression into the the global map.

when client request is comming, use boost::xpressive::regex_search() to match the request url 
and the regex expression and capture the named group.

The method stated above relies on the boost's regular expression library, which make things 
complicated, and the performance of the method does not reach the limit. As a geek, I 
implemented a amazing function to do all the work. Want to know more about the amazing function?
See https://github.com/Andrewpqc/vogro/blob/master/test/url_match.cpp

So, the content of this file is deprecated now.
**********************************/

#include <iostream>
#include <boost/xpressive/xpressive.hpp>

//old version of transformUserReqextoBstRegex

std::string transformUserRegexToBstRegex(std::string userRegex) {
    using namespace std;
    using namespace boost::xpressive;

    sregex pathParam = sregex::compile("\\{(int|str)?:?(\\w+)\\}");
    smatch what;
    while (regex_search(userRegex, what, pathParam)) {

        string pattern;

        if (what[1] == "int" || what[1] == "") {
            pattern = "\\d+";
        }
        else if (what[1] == "str") {
            pattern = "\\w+";
        }
        else {
            cout << "error" << endl;
        }

        stringstream s;
        string format;
        s << "(?P<" << what[2] << ">" << pattern << ")";
        s >> format;
        s.clear();

        userRegex = regex_replace(userRegex, pathParam, format, regex_constants::format_first_only);
    }

    if (userRegex.back() != '/') {
        userRegex += "/?$";
    }
    else {
        userRegex += "?$";
    }

    userRegex = '^' + userRegex;
    return userRegex;
}

void testUrlMatch(std::string regString, std::string path, std::string name) {
    boost::xpressive::sregex e = boost::xpressive::sregex::compile(regString);
    boost::xpressive::smatch what;
    if (boost::xpressive::regex_search(path, what, e)) {
        try {
            std::cout << what[name] << std::endl;
        }
        catch (boost::exception_detail::clone_impl<boost::xpressive::regex_error> e) {
            std::cout << "参数" << name << "不存在" << std::endl;
        }
    }
    else {
        std::cout << "not match" << std::endl;
    }
}

int main(void) {
    std::string testUserRegex1 = "/abc/{id}";
    std::cout << transformUserRegexToBstRegex(testUserRegex1) << std::endl;
    std::string testUserRegex2 = "/abc/{int:id}";
    std::cout << transformUserRegexToBstRegex(testUserRegex2) << std::endl;
    std::string testUserRegex3 = "/abc/{id}/{str:name}/";
    std::cout << transformUserRegexToBstRegex(testUserRegex3) << std::endl;
    std::string testUserRegex4 = "/abc/{int:id}/{str:name}/hhh/";
    std::string regString = transformUserRegexToBstRegex(testUserRegex4);

    boost::xpressive::sregex e = boost::xpressive::sregex::compile(regString);
    testUrlMatch(regString, "/abc/2/andrewpqc/hhh", "id");
    testUrlMatch(regString, "/abc/2/andrewpqc/hhh", "name");
    testUrlMatch(regString, "/abc/2/andrewpqc/hhh/", "name");
    testUrlMatch(regString, "/ab/2/andrewpqc/hhh/", "name");

    return 0;
}