#include <iostream>
#include "ini_parser.h"

int main() {
    std::ifstream file("example.ini");
    ini_parser parser(file);
    std::cout << parser.get_value<std::string>("Section1", "var1");
    return 0;
}