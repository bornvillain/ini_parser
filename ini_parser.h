#pragma once

#include <string>
#include <variant>
#include <fstream>
#include <map>

class ini_parser final {
public:
    explicit ini_parser(std::ifstream &file);

    template<typename Type>
    Type get_value(const std::string &section, const std::string &value) const;

private:

    std::map<std::string, std::map<std::string, std::string>> sections_;
    std::ifstream &file_;

    static bool isValidSectionHeader(const std::string &line);

    static bool isValidExpression(const std::string &line);

    static bool isLineEmpty(const std::string &line);

    static std::string GetSectionName(const std::string &line);

    static std::pair<std::string, std::string> GetKeyValue(const std::string &line);

    void parse_file();

    std::string get_string_value(const std::string &section, const std::string &value) const;
};

template<typename Type>
Type ini_parser::get_value(const std::string &section, const std::string &value) const {
    static_assert(sizeof(Type) == -1,
                  "not implemented type for get_value");  //sizeof cannot return a positive value for an incomplete type
}

template<>
std::string ini_parser::get_value<std::string>(const std::string &section, const std::string &value) const;

template<>
int ini_parser::get_value<int>(const std::string &section, const std::string &value) const;

template<>
float ini_parser::get_value<float>(const std::string &section, const std::string &value) const;

template<>
double ini_parser::get_value<double>(const std::string &section, const std::string &value) const;