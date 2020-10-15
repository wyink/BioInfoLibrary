#include "Utils.h"
#include "Analysis.h"
#include "Formatter.h"
#include "PajekHelper.h"
#include "BlastParser.h"
#include "BioInfoLibrary.h"

namespace fs = std::filesystem;

/* TestCase1
* Faa形式のファイルをFasta形式に変換
* 必要なIDの塩基配列のみを抽出
*/
void main1();

/* TestCase2
* Blastの結果からAccessionIDの情報を種情報に変換してフォーマットを
* 整理して出力する
*/
void main2();

/* TestCase3
* F-measureを計算して指定のフォーマットで出力
*/
void main3();

/* TestCase4
* F-measureの結果からPajek形式のファイルを作成
*/
void main4();

/* TestCase5
* blast結果ファイルからスコアを100刻みでヒストグラム化する
* また、結果ファイルのアライメント箇所は重複しない場合には加算
* 
*/
void main5();

int main() {

    //main4();
    main5();

}

void main1() {
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
}

void main2() {
    //各fastaファイルのAccessionIDとstrainIDの対応mapの作成
    std::map<std::string, std::string> ac_st_map = Utils::keyValMakeFromFile(
        "D:/perflingens/new_resources/5_prid_strain.txt",/**< 入力ファイル */
        "\t",                                            /**< 入力ファイルに使用されているデリミタ */
        [](std::vector<std::string>& vec)->std::vector<std::string>& {
            vec[0] = vec[0].substr(1);/** 先頭文字の'>'は省く。vec[1]（値）は変更しない */
            return vec;
        }
    );

    //ファイル名（パス）とstrainIDの対応mapの作成
    std::map<std::string, std::string> fpath_stid_map = Utils::keyValMakeFromFile(
        "D:/perflingens/resources/prokaryotes_changed.txt", /**< 入力ファイル */
        "\t",                                               /**< 入力ファイルに使用されているデリミタ */
        [](std::vector<std::string>& vec)->std::vector<std::string>& {
            vec[0] = Utils::split(vec[14], "/").back(); /**< ファイル名（パス）*/
            vec[1] = vec[2];                            /**< strainID */
            return vec;
        }
    );

    //各AccessionIDに対してヒットしたAccessionIDをstrainIDに変換し、再集計したのちに出力
    //各ファイルの1行目にはそのファイルが所属するstrainIDを出力
    fs::path outfile;
    std::string filename;
    std::string header;
    std::regex re{ "(.+).fasta_re.txt$" };
    std::smatch sm;
    std::shared_ptr<BlastParserPt1Imple> bpi = std::make_shared<BlastParserPt1Imple>(ac_st_map);
    for (const fs::directory_entry& x : fs::directory_iterator("D:/perflingens/4_blast/result/")) {
        outfile = "D:/perflingens/4_blast/analyzedAgain" / x.path().filename();
        std::cout << outfile << "\n";

        //入力ファイル名をstrainIDに変換して出力ファイルのheaderとして出力
        filename = x.path().filename().string();
        regex_match(filename, sm, re);
        header = fpath_stid_map[sm.str(1)];
        std::cout << header << std::endl;
        std::make_unique<BlastParser>(x.path(), bpi)->run(outfile, header);
        exit(0);
    }

}

void main3() {

    Fmeasure fm(
        "D:/perflingens/4_blast/analyzedAgain", /**< 入力ディレクトリ */
        "D:/perflingens/out.txt",               /**< 出力ファイル */
        std::make_unique<FmeasurePt1>(FmeasurePt1::CountUpWay::Exist)
    );

    fm.run();
}

void main4() {
    fs::path dir = "D:/perflingens";
    fs::path infile = dir / "new_resources/f-measure.txt"; /**< 入力ファイル */

    CreateFromText cft(
        infile,
        std::make_unique<LabelSingle>("Label"),
        [](std::shared_ptr<Node> node)->std::shared_ptr<Node> {
            node->setPosition(0.500, 0.500);
            return node;
        }
    );

    std::vector<std::unique_ptr<Pajek> >  pvec = cft.run();
    for (auto iter = pvec.begin(); iter != pvec.end(); ++iter) {
        fs::path outfile = dir / "9_pajek_new";
        outfile /= (*iter)->getPajekLbel() + ".net"; //string to fs::path
        (*iter)->output(outfile);
    }
}

void main5() {
    //referenceIDの変換は今回行わない。
    std::map<std::string, std::string> queref{
        std::make_pair("reference", "no-change")
    };

    const fs::path infile = "G:/perflingens/temp.txt";
    BlastParserPt2Imple bp2(queref);
    BlastParser bp(infile, std::make_shared<BlastParserPt2Imple>(bp2));
    bp.run("output.txt", "histgram");

}