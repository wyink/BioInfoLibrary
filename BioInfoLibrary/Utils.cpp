#include "Utils.h"

std::vector<std::string> Utils::split(const std::string& line, const char* delimiter) {
    std::vector<std::string> vec;
    std::string::size_type len = line.length();
    for (std::string::size_type i = 0 , n = 0; i < len; i = n + 1) {
        n = line.find_first_of(delimiter, i);
        if (n == std::string::npos) {
            n = len;
        }
        vec.push_back(line.substr(i, n - i));
    }

    return vec;
}

bool Utils::isExistFile(const std::string& file)
{
    std::ifstream ifs(file);
    return ifs.is_open();
}