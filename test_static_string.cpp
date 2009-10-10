#include <iostream>

#include "static_string.hpp"

int main() {
    etl::static_string<char,128> str;

    str.append('q');
    str.append('u');
    str.append('i');
    str.append('t');
    str.append('\n');

    std::cout << str.c_str();

    if (str == "quit\n") {
        std::cout << "equal" << std::endl;
    } else {
        std::cout << "FAILED: strings are not equal." << std::endl;
        throw 23;
    }

    return 0;
}
