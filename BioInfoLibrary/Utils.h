#pragma once
#include <vector>
#include <string>

using namespace std;

class Utils
{
private:
    inline Utils() {};
public:
    static vector<string> split(const string& line, string delimiter);

};

