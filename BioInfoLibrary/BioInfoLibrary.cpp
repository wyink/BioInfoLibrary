
#pragma once
#include "Utils.h"
#include "Formatter.h"
#include "PajekHelper.h"
#include "BlastParser.h"
#include <filesystem>



int main() {
    namespace fs = std::filesystem;

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
    /*
    //reference_idの変換用
    std::ifstream in{ "D:/perflingens/new_resources/5_prid_strain.txt" };
    std::string line;
    std::vector<std::string> vec;
    std::map<std::string, std::string> map;
    while (std::getline(in, line) ){
        vec = Utils::split(line, "\t");
        map[vec[0].substr(1)] = vec[1];
    }
    in.close();

    //header用（各入力ファイルの属するid）
    std::vector<std::string> genFtpPath;
    std::map<std::string, std::string> header; //filename->strain
    std::ifstream in2{ "D:/perflingens/resources/prokaryotes_changed.txt" };
    std::getline(in2, line); //skip header
    while (std::getline(in2, line)) {
        vec = Utils::split(line, "\t");
        genFtpPath = Utils::split(vec[14], "/");
        header[genFtpPath.back()] = vec[2];//strain
    }
    in2.close();
    
    BlastParserPt1Imple bp1(map);

    std::string infile;
    std::string outfile;
    std::string headert;
    std::regex re{ "(.+).fasta_re.txt$" };
    std::smatch sm;


    // dir_aディレクトリ直下に含まれる全ファイルを出力
    for (const fs::directory_entry& x : fs::directory_iterator("D:/perflingens/4_blast/result/")) {

        infile = x.path().string();
        BlastParser bp(infile, bp1);

        outfile =  "D:/perflingens/4_blast/analyzedAgain/" + x.path().filename().string();
        std::cout << outfile << "\n" ;

        //header用
        headert = x.path().filename().string();
        regex_match(headert, sm, re);
        std::cout << header[sm.str(1)] << std::endl;
        bp.run(outfile,header[sm.str(1)]);
    }
  */

/*testCase2 */
    
    const fs::path indir = "D:/perflingens/4_blast/analyzedAgain";

    if(!fs::exists(indir)){
        std::cout << "No such Directory!"<< std::endl;
        exit(1);
    }

    std::string line;
    std::map<std::string, std::map<std::string, int> > idCounterMap; /**< map[idA]={idA:count,idB:count,...} */
    std::vector<std::string> idCountPairVec;  /**< (id:counter),(id:counter),... */
    std::vector<std::string> idCountVec;      /**< id:counter */
    for (const fs::directory_entry& x : fs::directory_iterator(indir)){
        fs::path path = x.path();
        std::ifstream in{ path };


        //一行目でファイル自身のidを取得
        std::getline(in, line);
        std::string selfid = line;

        int i = 1;
        while (std::getline(in, line)) {
            ++i;
            if (i % 2 == 0) { //start with '>';
                // Not used;
                continue;
            }
            else {
                idCountPairVec = Utils::split(line, ",");
                for (const auto& idCountPair_ : idCountPairVec) {
                    idCountVec = Utils::split(idCountPair_, ":");

                    /* 
                       複数個所ヒットした配列を総合する，
                   　  いわゆるmaximumに対応する場合
                    */
                    //idCounterMap[selfid][idCountVec[0]] += std::stoi(idCountVec[1]);

                    /*各配列ヒットしたかどうかを判定(ヒットで1）*/
                    //今回の場合はno-hitの場合はstrainに登録されない．
                    idCounterMap[selfid][idCountVec[0]] += 1;
                }
                
            }
        }
    }

    //fmeasureの計算
    std::set<std::string> memo;
    int denominator = 0;
    int fraction = 0;
    int hit  =0;
    float fmeasure = 0.00f;
    std::map<std::string, float> fmeasureMap;
    for (const auto& [selfidA, stCountA] :  idCounterMap) {
        for (const auto& [selfidB, stCountB] : idCounterMap) {
            //cal
            if (selfidA == selfidB) {
                continue;
            }
            else if (memo.find(selfidB +"\t"+ selfidA) != memo.end()){
                //裏返しはskip
                continue;
            }
            else {
                denominator = int(stCountA.at(selfidA)) + int(stCountB.at(selfidB));
                hit = stCountA.at(selfidB) + stCountB.at(selfidA);
                fmeasure = (float) hit / denominator ;
                fmeasureMap[selfidA +"\t"+ selfidB] = fmeasure;
                memo.insert(selfidA +"\t"+ selfidB);
            }
        }
    }

    //format
    std::ofstream out{ "out.txt" };
    for (const auto& [conbi, fmeasure] : fmeasureMap) {
        out << conbi << "\t" << fmeasure << "\n";
    }
    
}

