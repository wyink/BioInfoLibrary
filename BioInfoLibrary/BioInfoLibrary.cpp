
#pragma once
#include "Utils.h"
#include "Formatter.h"
#include "PajekHelper.h"
#include "BlastParser.h"



int main() {

/* TextCase 1
    *
    std::string faafile = "D:/perflingens/2_protein/GCA_000009685.1_ASM968v1_protein.faa";
    std::string out = "D:/perflingens/otameshi.fasta";

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
    */

    //対応テキスト読み込み
    std::ifstream in{ "G:/perflingens/new_resources/5_prid_strain.txt" };
    std::string line;
    std::vector<std::string> vec;
    std::map<std::string, std::string> map;
    while (std::getline(in, line) ){
        vec = Utils::split(line, "\t");
        map[vec[0].substr(1)] = vec[1];
    }

    
    //const std::string infile = "D:/perflingens/4_blast/result/GCA_000009685.1_ASM968v1.fasta_re.txt" ;
    const std::string infile = "G:/perflingens/new_resources/in.txt";

    BlastParserPt1Imple bp1(map);
    BlastParser bp(infile, bp1);

    bp.run("G:/perflingens/new_resources/otameshi.txt");
    
}

