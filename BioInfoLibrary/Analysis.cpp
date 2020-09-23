#include "Analysis.h"
#include "Utils.h"

/*

    const fs::path indir = "D:/perflingens/4_blast/analyzedAgain";
    const fs::path out = "out.txt";
    Fmeasure fm(indir, out);
    fm.run();
*/


void Fmeasure::run(){

    if (!fs::exists(indir)) {
        std::cout << "No such Directory!" << std::endl;
        exit(1);
    }
    
    std::string line;
    std::map<std::string, std::map<std::string, int> > idCounterMap; /**< map[idA]={idA:count,idB:count,...} */
    std::vector<std::string> idCountPairVec;  /**< (id:counter),(id:counter),... */
    std::vector<std::string> idCountVec;      /**< id:counter */
    for (const fs::directory_entry& x : fs::directory_iterator(indir)) {
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
    int hit = 0;
    float fmeasure = 0.00f;
    std::map<std::string, float> fmeasureMap;
    for (const auto& [selfidA, stCountA] : idCounterMap) {
        for (const auto& [selfidB, stCountB] : idCounterMap) {
            //cal
            if (selfidA == selfidB) {
                continue;
            }
            else if (memo.find(selfidB + "\t" + selfidA) != memo.end()) {
                //裏返しはskip
                continue;
            }
            else {
                denominator = int(stCountA.at(selfidA)) + int(stCountB.at(selfidB));
                hit = stCountA.at(selfidB) + stCountB.at(selfidA);
                fmeasure = (float)hit / denominator;
                fmeasureMap[selfidA + "\t" + selfidB] = fmeasure;
                memo.insert(selfidA + "\t" + selfidB);
            }
        }
    }
    
    //format
    std::ofstream out{outfile};
    for (const auto& [conbi, fmeasure] : fmeasureMap) {
        out << conbi << "\t" << fmeasure << "\n";
    }
    
}
