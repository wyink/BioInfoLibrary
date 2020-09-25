#include "Analysis.h"
#include "Utils.h"

void Fmeasure::run(){

    if (!fs::exists(indir)) {
        std::cout << "No such Directory!" << std::endl;
        exit(1);
    }
    
    std::string line;
    std::map<std::string, std::map<std::string, int> > idCounterMap; /**< map[idA]={idA:count,idB:count,...} */
    fh.strategy(indir);
    
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
    fh.outformat(fmeasureMap,outfile);
    
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
                    case Exist :
                        idCounterMap[selfid][idCountVec[0]] += 1;
                        break;
                    case Amount:
                        idCounterMap[selfid][idCountVec[0]] += std::stoi(idCountVec[1]);
                    default:
                        std::cout << "Parameter is invalid" << std::endl;
                        exit(1);
                    }
                }
            }
        }
    }
}

void FmeasurePt1::outformat(const std::map<std::string,float>& fmeasureMap, const fs::path outfile) {
    std::ofstream out{ outfile };
    for (const auto& [conbi, fmeasure] : fmeasureMap) {
        out << conbi << "\t" << fmeasure << "\n";
    }
}

/*
class BlastParser
{
private:
    const std::string infile;
    BlastParserHandler& bph;

public:
    explicit BlastParser(const std::string& infile, BlastParserHandler& bph);

    inline void setHandler(BlastParserHandler& bph) {
        this->bph = bph;
    }

    /**
    * @brief blast結果ファイルの解析開始
    
void run(const std::string& outfile, const std::string& header);
};


*/