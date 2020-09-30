
#pragma once
#include "Utils.h"
#include "Analysis.h"
#include "Formatter.h"
#include "PajekHelper.h"
#include "BlastParser.h"
#include <filesystem>



int main() {
    namespace fs = std::filesystem;

/* testCase 1
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

/*testCase3 */
    /*
    const fs::path indir = "G:/perflingens/4_blast/analyzedAgain";
    const fs::path out = "G:/perflingens/out.txt";

    FmeasurePt1 fh(FmeasurePt1::CountUpWay::Exist);
    Fmeasure fm(indir,out,fh);
    fm.run();
    */

    fs::path dir = "D:/perflingens";
    fs::path infile = dir / "out.txt"; /**< 入力ファイル */
    
    CreateFromText cft(
        infile,
        std::make_unique<LabelSingle>("Label"),
        [](std::shared_ptr<Node> node)->std::shared_ptr<Node> {
            node->setPosition(0.500, 0.500);
            return node;
        }
    );
    
    std::vector<Pajek*> pvec = cft.run();
    for (const auto* pajekptr : pvec) {
        fs::path infile = dir;
        infile /= pajekptr->getPajekLbel() + ".txt"; //string to fs::path
        pajekptr->output(infile);
    }

}

