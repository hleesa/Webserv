#ifndef TOSTRING_HPP
#define TOSTRING_HPP

#include <string>

template <typename T>
std::string to_string(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

#endif