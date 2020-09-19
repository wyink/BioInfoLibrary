
#pragma once
#include "Utils.h"
#include "Formatter.h"
#include "PajekHelper.h"
#include <vector>
#include <string>


using namespace std;


//test
int main() {
    /*
    string p = "abc";
    const char d = '\t';
    vector<string> abc =Utils::split(p, &d);
    return 0;
    */

    std::string faafile = "G:/perflingens/2_protein/GCA_000009685.1_ASM968v1_protein.faa";
    std::string out = "G:/perflingens/otameshi.fasta";

    Formatter::FaaToFasta(faafile, out);


}
