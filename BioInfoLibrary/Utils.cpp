#include "Utils.h"

vector<string> Utils::split(const string& line, string delimiter) {
    vector<string> vec;
    string::size_type len = line.length();
    //a,b
    for (string::size_type i = 0 , n = 0; i < len; i = n + 1) {
        n = line.find_first_of(delimiter, i);
        if (n == string::npos) {
            n = len;
        }
        vec.push_back(line.substr(i, n - i));
    }

    return vec;
}

