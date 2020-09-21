
#pragma once
#include "Utils.h"
#include "Formatter.h"
#include "PajekHelper.h"
#include "BlastParser.h"

using namespace std;


//test
int main() {

    //Cstreamは使わない
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);

    /*
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
    std::ifstream in{ "D:/perflingens/new_resources/5_prid_strain.txt" };
    std::string line;
    std::vector<std::string> vec;
    std::map<std::string, std::string> map;
    while (std::getline(in, line) ){
        vec = Utils::split(line, "\t");
        
        map.insert(std::make_pair(vec[0].substr(1), vec[1]));
    }



    //const std::string infile = "D:/perflingens/4_blast/result/GCA_000009685.1_ASM968v1.fasta_re.txt" ;
    const std::string infile = "D:/perflingens/new_resources/in.txt";
    BlastParser bp(infile);
    bp.setRefConvertMap(map);
    bp.run("D:/perflingens/new_resources/otameshi.txt");

}
