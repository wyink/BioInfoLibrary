
#pragma once
#include "Utils.h"
#include "Formatter.h"
#include "PajekHelper.h"
#include "BlastParser.h"
#include <filesystem>



int main() {

/* testCase 1
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

/*textCase2 */

    std::ifstream in{ "G:/perflingens/new_resources/5_prid_strain.txt" };
    std::string line;
    std::vector<std::string> vec;
    std::map<std::string, std::string> map;
    while (std::getline(in, line) ){
        vec = Utils::split(line, "\t");
        map[vec[0].substr(1)] = vec[1];
    }
    in.close();

    
    namespace fs = std::filesystem;
    BlastParserPt1Imple bp1(map);

    std::string infile;
    std::string outfile;

    // dir_aディレクトリ直下に含まれる全ファイルを出力
    for (const fs::directory_entry& x : fs::directory_iterator("G:/perflingens/4_blast/result/")) {
        std::cout << x.path() << std::endl;

        infile = x.path().string();
        BlastParser bp(infile, bp1);

        std::cout << x.path().filename().string() << std::endl;

        outfile =  "G:/perflingens/4_blast/analyzedAgain/" + x.path().filename().string();
        std::cout << outfile << std::endl;
        bp.run(outfile);

        exit(0);
    }

    
    
   
}

