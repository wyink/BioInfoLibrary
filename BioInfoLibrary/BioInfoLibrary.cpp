
#pragma once
#include "Utils.h"
#include "Formatter.h"
#include "PajekHelper.h"
#include <vector>
#include <string>


using namespace std;


//test
int main() {

    std::string faafile = "G:/perflingens/2_protein/GCA_000009685.1_ASM968v1_protein.faa";
    std::string out = "G:/perflingens/otameshi.fasta";

    FaaToFasta fta(faafile, out);
    fta.setIdFilterCall(
            [](std::string idLine)->bool {
                std::string::size_type n = idLine.find("(plasmid)");
                if (n == std::string::npos) {// 核ゲノム
                    return true;
                }
                else {//見つかったー＞プラスミド
                    return false;
                }
            }
        )
        .run();



}
