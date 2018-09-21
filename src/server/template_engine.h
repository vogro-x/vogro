//
// Created by xvvx on 18-9-2.
//

#ifndef VOGRO_TPL_TEMPLATE_ENGINE_H

#define VOGRO_TPL_TEMPLATE_ENGINE_H

#include <algorithm>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

namespace vstring {

    std::vector<std::string> split(const std::string &str, const std::string &delim) {
        std::vector<std::string> result;
        std::string::size_type pos1, pos2;

        pos1 = 0;
        pos2 = str.find(delim);

        while(std::string::npos != pos2) {
            result.push_back(str.substr(pos1, pos2-pos1));

            pos1 = pos2 + delim.size();
            pos2 = str.find(delim, pos1);
        }
        if(pos1 != str.length())
            result.push_back(str.substr(pos1));
        return result;
    }

    std::vector<std::string> split(const std::string &str) {
        return split(str, " ");
    }

    std::string join(const std::vector<std::string> &strs, const std::string &seq) {
        std::string result;
        for (size_t i = 0; i < strs.size()-1; ++i) {
            result += (strs[i] + seq);
        }
        result += strs.back();
        return result;
    }

    inline std::string lstrip(const std::string &str, const std::string &rm = "\r\t ") {
        auto beg = str.find_first_not_of(rm);
        if (beg == std::string::npos) {
            return "";
        } else {
            return str.substr(beg);
        }
    }

    inline std::string rstrip(const std::string &str, const std::string &rm = "\r\t ") {
        auto end = str.find_last_not_of(rm);
        if (end == std::string::npos) {
            return "";
        } else {
            return str.substr(0, end+1);
        }
    }

    inline std::string strip(const std::string &str, const std::string &rm = "\n\t ") {
        return rstrip(lstrip(str, rm), rm);
    }

    inline bool start_with(const std::string &str, const std::string &prefix) {
        return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
    }

    inline bool end_with(const std::string &str, const std::string &suffix) {
        return str.size() >= suffix.size() && str.substr(str.size() - suffix.size(), suffix.size()) == suffix;
    }

    std::string upper(const std::string &str) {
        std::string result;
        for (auto c : str) {
            result += toupper(c);
        }
        return result;
    }

    std::string lower(const std::string &str) {
        std::string result;
        for (auto c : str) {
            result += tolower(c);
        }
        return result;
    }
};

namespace vtpl {

    using Json = nlohmann::json;

    using std::string;
    using std::vector;
    using std::unordered_map;

    using vstring::split;
    using vstring::rstrip;
    using vstring::lstrip;
    using vstring::strip;
    using vstring::lower;
    using vstring::upper;
    using vstring::end_with;
    using vstring::start_with;
    using vstring::join;

    vector<string> ParseToLines(string html_tpl) {
        std::vector<std::string> result;
        size_t n = html_tpl.size(),  last_begin = 0;
        unordered_map<string, string> open_close_m = { {"{{", "}}"},  {"{%", "%}"}, {"{#", "#}"} };

        for (size_t i = 0; i < n; ++i) {
            auto open = html_tpl.substr(i, 2);
            if (open == "{{" || open == "{%" || open == "{#") {
                auto token = strip(html_tpl.substr(last_begin, i - last_begin));
                if (not token.empty()) {
                    result.push_back(token);
                }
                string close_str = open_close_m[open];
                auto end = html_tpl.find(close_str, i);
                if (end == std::string::npos) {
//                    throw std::exception();
                    return result;
                } else {
                    result.push_back(html_tpl.substr(i, end - i + 2));
                    i = end + 1;
                    last_begin = i + 1;
                }
            }
        }
        result.push_back(html_tpl.substr(last_begin + 1, n - last_begin));
        return result;
    }

    class Environment: public Json {

    private:
        template <typename T>
        auto GetData(const T &data, size_t index, const vector<string> &keys) {
            if (keys.size() - 1 == index) {
                return data[keys[index]];
            } else {
                return GetData(data[keys[index]], index+1, keys);
            }
        }

    public:

        auto Get(const string &key) {
            auto keys = split(key, ".");
            return GetData((*this), 0, keys);
        }
    };

    class Template {
    public:
        Template(string &html_tpl):
                html_tpl_(std::move(html_tpl)) {
        }

        Template(string &&html_tpl):
                html_tpl_(html_tpl) {
        }

        string render(Environment &env) {
            auto items = ParseToLines(html_tpl_);
            return render(items, env);
        }

    public:
        string render(vector<string> &items, Environment &env) {
            vector<string> stk, result;
            bool is_if = false, is_for = false;
            size_t times = 0, match_times = 0;

            int i = 0;
            for (auto item : items) {
                ++i;
                if (start_with(item, "{{")) {
                    if (not is_for and not is_if) {
                        auto key = GetTokens(item).front();
                        result.push_back(JsonToString(env.Get(key)));
                    } else {
                        stk.push_back(item);
                    }
                } else if (start_with(item, "{%")) {
                    auto tokens = GetTokens(item);
                    if (tokens.front() == "if") {
                        stk.push_back(item);
                        if (not is_for) {
                            is_if = true;
                            ++times;
                        }
                    } else if (tokens.front() == "for") {
                        stk.push_back(item);
                        if (not is_if) {
                            is_for = true;
                            ++times;
                        }
                    }

                    if (tokens.front() == "endif") {
                        stk.push_back(item);
                        if (not is_for) {
                            ++match_times;
                        }
                        if (match_times == times) {
                            result.push_back(IfBlock(stk, env));
                            stk.clear();
                            is_if = false, is_for = false;
                            times = 0, match_times = 0;
                        }

                    } else if (tokens.front() == "endfor") {
                        stk.push_back(item);
                        if (not is_if) {
                            ++match_times;
                        }
                        if (match_times == times) {
                            result.push_back(ForBlock(stk, env));
                            stk.clear();
                            is_if = false, is_for = false;
                            times = 0, match_times = 0;
                        }
                    }
                } else {
                    if (not is_for and not is_if) {
                        result.push_back(item);
                    } else {
                        stk.push_back(item);
                    }
                }
            }
            return join(result, " ");
        }

        vector<string> GetTokens(const string &item) {
            return split(strip(item.substr(2, item.size()-4)));
        }

        string IfBlock(vector<string> &stk, Environment &env) {
            auto tokens = GetTokens(stk.front());
            tokens.erase(tokens.begin());
            stk.erase(stk.begin());
            stk.pop_back();

            if (CalculateCondition(tokens, env)) {
                return (render(stk, env));
            } else {
                return (string());
            }
        }

        string ForBlock(vector<string> &stk, Environment &env) {
            string result;
            auto tokens = GetTokens(stk.front());
            stk.erase(stk.begin());
            stk.pop_back();

            if (tokens[0] != "for" or tokens[2] != "in") {
                throw std::exception();
            }

            string iter_name = tokens[1], list_name = tokens[3];
            auto env_copy = env;
            for (auto it : env.Get(list_name)) {
                env_copy[iter_name] = it;
                result += render(stk, env_copy);
            }

            return result;
        }

        bool CalculateCondition(const vector<string> &cond, Environment &env) {
            return env.Get(cond.front());
        }

        template <typename JsonObj>
        string JsonToString(const JsonObj &obj) {
            return strip(obj.dump(), "\"\n");
        }

    private:
        string html_tpl_;
    };

    string TemplateRender(string html_tpl, Environment data) {
        Template tpl = Template(html_tpl);
        return tpl.render(data);
    }
};

#endif //VOGRO_TPL_TEMPLATE_ENGINE_H
