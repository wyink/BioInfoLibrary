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
    
    
        //��s�ڂŃt�@�C�����g��id���擾
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
                       �������q�b�g�����z��𑍍�����C
                   �@  ������maximum�ɑΉ�����ꍇ
                    */
                    //idCounterMap[selfid][idCountVec[0]] += std::stoi(idCountVec[1]);
    
                    /*�e�z��q�b�g�������ǂ����𔻒�(�q�b�g��1�j*/
                    //����̏ꍇ��no-hit�̏ꍇ��strain�ɓo�^����Ȃ��D
                    idCounterMap[selfid][idCountVec[0]] += 1;
                }
    
            }
        }
    }
    
    //fmeasure�̌v�Z
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
                //���Ԃ���skip
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
