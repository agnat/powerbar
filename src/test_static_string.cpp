#include <iostream>

#include "static_string.hpp"

int main() {
    etl::static_string<char,128> str;
    if ( str.capacity() != 128) {
        std::cout << "unexpected capacity" << std::endl;
        throw 23;
    }

    if ( ! str.empty() ) {
        std::cout << "not empty" << std::endl;
        throw 23;
    }
    if ( str.size() != 0 ) {
        std::cout << "size not 0" << std::endl;
        throw 23;
    }
    str.append('q');
    if ( str.empty() ) {
        std::cout << "string claims to be empty" << std::endl;
        throw 23;
    }
    if ( str.size() != 1 ) {
        std::cout << "size not 1" << std::endl;
        throw 23;
    }
    str.append('u');
    if ( str.size() != 2 ) {
        std::cout << "size not 2" << std::endl;
        throw 23;
    }
    str.append('i');
    if ( str.size() != 3 ) {
        std::cout << "size not 3" << std::endl;
        throw 23;
    }
    str.append('t');
    if ( str.size() != 4 ) {
        std::cout << "size not 4" << std::endl;
        throw 23;
    }
    str.append('\n');
    if ( str.size() != 5 ) {
        std::cout << "size not 5" << std::endl;
        throw 23;
    }

    std::cout << str.c_str();

    if (str == "quit\n") {
        std::cout << "equal" << std::endl;
    } else {
        std::cout << "FAILED: strings are not equal." << std::endl;
        throw 23;
    }

    str.clear();
    if ( ! str.empty() ) {
        std::cout << "not empty after clear" << std::endl;
        throw 23;
    }

    return 0;
}
