#include <string>

bool isVisibleString(const std::string& str) {
    for (std::string::const_iterator ch = str.begin(); ch != str.end(); ++ch) {
        if (!isprint(*ch))
            return false;
    }
    return true;
}

