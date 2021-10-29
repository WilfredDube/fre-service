#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <regex>

class URL
{
    void parse(const std::string &url_s);
    std::string protocol_, host_, path_, query_;
    std::vector<std::string> split(const std::string str, const std::string regex_str);

public:
    explicit URL(const std::string &url_s);
    std::string extractBlobName();
};