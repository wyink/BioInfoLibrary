#include "Analysis.h"
#include "Utils.h"

void Fmeasure::run(){

    if (!fs::exists(indir)) {
        std::cout << indir << std::endl;
        std::cout << "No such Directory!" << std::endl;
        exit(1);
    }
    
    // ＜入力＞
    std::map<std::string, std::map<std::string, int> > idCounterMap; /**< map[id_A]={id_A:fma,id_B:fmb,...} */
    idCounterMap = fh.informat(indir);
    
    // f-measureの計算
    std::map<std::string, float> fmeasureMap = calcFmeasure(idCounterMap);

    // ＜出力＞
    fh.outformat(fmeasureMap,outfile);
    
}

//private-method 
const std::map<std::string, float> Fmeasure::calcFmeasure(const std::map<std::string, std::map<std::string, int> >& idCounterMap) {
    std::set<std::string> memo;
    int denominator = 0;
    int fraction = 0;
    int hit = 0;
    float fmeasure = 0.00f;
    std::map<std::string, float> fmeasureMap;
    for (const auto& [selfidA, stCountA] : idCounterMap) {
        for (const auto& [selfidB, stCountB] : idCounterMap) {

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
    return fmeasureMap;
}

std::map<std::string, std::map<std::string,int> > FmeasurePt1::informat(const fs::path& indir) {
    std::string line;
    std::map<std::string, std::map<std::string, int> > idCounterMap; /**< map[idA]={idA:count,idB:count,...} */
    std::vector<std::string> idCountPairVec;  /**< (id:counter),(id:counter),... */
    std::vector<std::string> idCountVec;      /**< id:counter */
    fs::path infile; /**< 入力ファイル */
    for (const fs::directory_entry& x : fs::directory_iterator(indir)) {

        //自身のidを取得（ヘッダー処理）
        infile = x.path();
        std::ifstream in{ infile };
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

                    switch (cuway) {
                    case FmeasurePt1::CountUpWay::Exist:
                        idCounterMap[selfid][idCountVec[0]] += 1;
                        break;
                    case FmeasurePt1::CountUpWay::Amount :
                        idCounterMap[selfid][idCountVec[0]] += std::stoi(idCountVec[1]);
                        break;
                    default:
                        std::cout << "Parameter is invalid" << std::endl;
                        exit(1);
                    }
                }
            }
        }
    }
    return idCounterMap;
}

void FmeasurePt1::outformat(const std::map<std::string,float>& fmeasureMap, const fs::path outfile) {
    std::ofstream out{ outfile };
    for (const auto& [conbi, fmeasure] : fmeasureMap) {
        out << conbi << "\t" << fmeasure << "\n";
    }
}

