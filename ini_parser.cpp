#include "ini_parser.h"
#include <algorithm>
#include <regex>

ini_parser::ini_parser(std::ifstream &file)
        : file_(file) {
    if (!file.is_open()) {
        throw std::ios_base::failure("error opening file");
    }
    parse_file();
}

bool ini_parser::isValidSectionHeader(const std::string &line) {
    std::regex pattern(R"(\s*\[[a-zA-Z0-9]+]\s*)");
    return std::regex_match(line, pattern);
}

bool ini_parser::isValidExpression(const std::string &line) {
    std::regex pattern(R"(\s*([A-Za-z0-9]+)\s*=\s*(.*)\s*)");
    return std::regex_match(line, pattern);
}

bool ini_parser::isLineEmpty(const std::string &line) {
    return line.empty() || std::all_of(line.begin(), line.end(), [](const char c) { return isspace(c); });
}

std::string ini_parser::GetSectionName(const std::string &line) {
    size_t start = line.find('[');
    size_t end = line.find(']');
    std::string result = line.substr(start + 1, end - start - 1);
    return result;
}

std::pair<std::string, std::string> ini_parser::GetKeyValue(const std::string &line) {
    size_t equal_pos = line.find('=');
    std::string left = line.substr(0, equal_pos);
    std::string right = line.substr(equal_pos + 1);

    left.erase(0, left.find_first_not_of(' '));
    left.erase(left.find_last_not_of(' ') + 1);
    right.erase(0, right.find_first_not_of(' '));
    right.erase(right.find_last_not_of(' ') + 1);

    return {left, right};
}

void ini_parser::parse_file() {
    std::string line;
    int line_number = 1;
    bool is_first = true;
    std::string current_section_name;
    while (std::getline(file_, line)) {
        size_t semicolon_pos = line.find(';');
        if (semicolon_pos != std::string::npos) {
            line = line.substr(0, semicolon_pos);
        }

        if (isValidSectionHeader(line)) {
            is_first = false;
            current_section_name = GetSectionName(line);
            sections_[current_section_name];
            ++line_number;
        } else if (isValidExpression(line)) {
            if (is_first) {
                throw std::invalid_argument(
                        "incorrect file format: syntax error on line " + std::to_string(line_number));
            }
            auto [key, value] = GetKeyValue(line);
            sections_[current_section_name][key] = value;
            ++line_number;

        } else if (isLineEmpty(line)) {
            ++line_number;
        } else {
            throw std::invalid_argument(
                    "incorrect file format: syntax error on line " + std::to_string(line_number));
        }
    }
}

std::string ini_parser::get_string_value(const std::string &section, const std::string &value) const {
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

    return sections_.at(section).at(value);
}

template<>
std::string ini_parser::get_value<std::string>(const std::string &section, const std::string &value) const {
    return get_string_value(section, value);
}

template<>
int ini_parser::get_value<int>(const std::string &section, const std::string &value) const {
    return std::stoi(get_string_value(section, value));
}

template<>
float ini_parser::get_value<float>(const std::string &section, const std::string &value) const {
    return std::stof(get_string_value(section, value));
}

template<>
double ini_parser::get_value<double>(const std::string &section, const std::string &value) const {
    return std::stod(get_string_value(section, value));
}