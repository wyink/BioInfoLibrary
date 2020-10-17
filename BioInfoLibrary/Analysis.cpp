#include "Analysis.h"
#include "Utils.h"

void Fmeasure::run(){

    if (!fs::exists(indir)) {
        std::cout << indir << std::endl;
        std::cout << "No such Directory!" << std::endl;
        exit(1);
    }
    
    // ＜入力＞
    std::unordered_map<std::string, std::unordered_map<std::string, int> > idCounterMap = fh->informat(indir);
    
    // f-measureの計算
    std::map<std::string, float> fmeasureMap = calcFmeasure(idCounterMap);

    // ＜出力＞
    fh->outformat(fmeasureMap,outfile);
    
}

//private-method 
const std::map<std::string, float> Fmeasure::calcFmeasure(const std::unordered_map<std::string, std::unordered_map<std::string, int> >& idCounterMap) {
    std::unordered_set<std::string> memo;
    int denominator = 0;
    int fraction = 0;
    int hit = 0;
    float fmeasure = 0.00f;
    std::map<std::string, float> fmeasureMap; //出力するため
    //for (const auto& [selfidA, stCountA] : idCounterMap) {
    for (auto iter_A = idCounterMap.begin();iter_A != idCounterMap.end();++iter_A){
        //for (const auto& [selfidB, stCountB] : idCounterMap) {
        for(auto iter_B = idCounterMap.begin();iter_B != idCounterMap.end();++iter_B){
            if (iter_A->first == iter_B->first) {
                continue;
            }
            else if (memo.find(iter_B->first + "\t" + iter_A->first) != memo.end()) {
                //裏返しはskip
                continue;
            }
            else {
                denominator = (iter_A->second).at(iter_A->first) + (iter_B->second).at(iter_B->first);
                //denominator = int(stCountA.at(selfidA)) + int(stCountB.at(selfidB));
                hit = (iter_A->second).at(iter_B->first) + (iter_B->second).at(iter_A->first);
                //hit = stCountA.at(selfidB) + stCountB.at(selfidA);

                fmeasure = (float)hit / denominator;
                fmeasureMap[iter_A->first + "\t" + iter_B->first] = fmeasure;
                memo.insert(iter_A->first + "\t" + iter_B->first);
            }
        }
    }
    return fmeasureMap;
}

std::unordered_map<std::string, std::unordered_map<std::string,int> > FmeasurePt1::informat(const fs::path& indir) {
    std::string line;
    std::unordered_map<std::string, std::unordered_map<std::string, int> > idCounterMap; /**< map[idA]={idA:count,idB:count,...} */
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

