
#pragma once
#include "Utils.h"
#include "PajekHelper.h"
#include <vector>
#include <string>


using namespace std;


//test
int main() {
    string p = "abc";
    const char d = '\t';
    vector<string> abc =Utils::split(p, &d);
    return 0;

}
