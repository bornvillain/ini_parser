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
    std::map<std::string, std::map<std::string, std::variant<int, double, std::string>>> sections_;
    std::ifstream &file_;

    static bool isValidSectionHeader(const std::string &line);

    static bool isValidExpression(const std::string &line);

    static bool isLineEmpty(const std::string &line);

    static std::string GetSectionName(const std::string &line);

    using KeyValueType = std::pair<std::string, std::variant<int, double, std::string>>;

    static KeyValueType GetKeyValue(const std::string &line);

    void parse_file();
};

template<typename Type>
Type ini_parser::get_value(const std::string &section, const std::string &value) const {

    if (sections_.find(section) == sections_.end()) {
        throw std::out_of_range("section " + section + " not found");
    }

    if (sections_.at(section).find(value) == sections_.at(section).end()) {
        std::string available_values;
        bool is_first = true;
        if (sections_.at(section).empty()) {
            available_values = "no values available in this section";
        } else {
            for (const auto &[key, _]: sections_.at(section)) {
                if (is_first) {
                    available_values += key;
                    is_first = false;
                } else {
                    available_values += ", " + key;
                }
            }
        }
        throw std::out_of_range(
                "element in section " + section + " with value " + value + " not found, available values: " +
                available_values);
    }

    if (std::holds_alternative<int>(sections_.at(section).at(value))) {
        if constexpr (std::is_same<Type, double>::value) {
            return static_cast<double>(std::get<int>(sections_.at(section).at(value)));
        } else if constexpr (std::is_same<Type, std::string>::value) {
            return std::to_string(std::get<int>(sections_.at(section).at(value)));
        }
    } else if (std::holds_alternative<double>(sections_.at(section).at(value))) {
        if constexpr (std::is_same<Type, int>::value) {
            return static_cast<int>(std::get<double>(sections_.at(section).at(value)));
        } else if constexpr (std::is_same<Type, std::string>::value) {
            return std::to_string(std::get<double>(sections_.at(section).at(value)));
        }
    } else if (std::holds_alternative<std::string>(sections_.at(section).at(value))) {
        if constexpr (std::is_same<Type, int>::value) {
            return std::stoi(std::get<std::string>(sections_.at(section).at(value)));
        } else if constexpr (std::is_same<Type, double>::value) {
            return std::stod(std::get<std::string>(sections_.at(section).at(value)));
        }
    }

    return std::get<Type>(sections_.at(section).at(value));
}