#include "Utils.h"


std::vector<std::string> Utils::split(const std::string& line, const char* delimiter) {
    std::vector<std::string> vec;
    std::string::size_type len = line.length();

    for (std::string::size_type i = 0 , n = 0; i < len; i = n + 1) {
        n = line.find_first_of(delimiter, i);
        if (n == std::string::npos) {
            n = len;
        }
        vec.emplace_back(line.substr(i, n - i));
    }

    return vec;
}

std::vector<std::string> Utils::split(const std::string& line, const char* delimiter, const int maxSplitCount) {
    std::vector<std::string> vec;
    std::string::size_type len = line.length();

    std::string::size_type i = 0;
    for (std::string::size_type n = 0, c = 0; c < maxSplitCount; ++c, i = n + 1) {
        n = line.find_first_of(delimiter, i);
        if (n == std::string::npos) {
            std::cout << "Žw’è•¶Žš—ñ‚É" << maxSplitCount << "ŒÂ‚Ì[" << delimiter << "]‚Í‘¶Ý‚µ‚Ü‚¹‚ñ\n";
            return vec;
        }
        vec.emplace_back(line.substr(i, n - i));
    }

    vec.push_back(line.substr(i, len - i));

    return vec;
}


 std::string* Utils::splitf(const std::string& line, const char* delimiter, std::string* arr,const int maxSplitCount) 
{
    std::string::size_type len = line.length();
    std::string::size_type i = 0;
    for (std::string::size_type n = 0, c = 0; c < maxSplitCount; ++c, i = n + 1) {
        n = line.find_first_of(delimiter, i);
        if (n == std::string::npos) {
            std::cout << "Žw’è•¶Žš—ñ‚É" << maxSplitCount << "ŒÂ‚Ì[" << delimiter << "]‚Í‘¶Ý‚µ‚Ü‚¹‚ñ\n";
            return arr;
        }
        arr[c] = line.substr(i, n - 1);
    }

    arr[maxSplitCount] = line.substr(i, len - i);
    
    return arr;
}


std::map<std::string, std::string> Utils::keyValMakeFromFile(
    const fs::path& infile,
    const char* del,
    std::function<std::vector<std::string>&(std::vector<std::string>&)> func) {

    std::ifstream in{ infile };
    std::string line;
    std::vector<std::string> vec;
    std::map<std::string, std::string> keyValMap;
    while (std::getline(in, line)) {
        vec = Utils::split(line, del);
        vec = func(vec);
        keyValMap[vec[0]] = vec[1];
    }
    in.close();
    return keyValMap;
}


bool Utils::isExistFile(const std::string& file)
{
    std::ifstream ifs(file);
    return ifs.is_open();
}