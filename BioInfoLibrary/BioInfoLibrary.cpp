#include "Utils.h"
#include "Analysis.h"
#include "Formatter.h"
#include "PajekHelper.h"
#include "BlastParser.h"
#include "BioInfoLibrary.h"

namespace fs = std::filesystem;
const fs::path COMMON_DIR = "D:/perflingens";

/* TestCase1
* Faa形式のファイルをFasta形式に変換
* 必要なIDの塩基配列のみを抽出
*/
void main1();

/* TestCase2
* Blastの結果からAccessionIDの情報を株情報に変換してフォーマットを
* 整理して出力する
* BlastPrserPt1Impleハンドラを利用する今回の場合は同一クエリ・同一参照に対して
* の複数箇所ヒットを考慮しない。そのため、複数個所にヒットしているものがある
* 場合、カウントが増える。しかし、今回の場合は同一タンパク質のあるなしを比較するため、
* この重複カウントに意味はない。
* 
* @param[inEndDir] 最終階層の入力ディレクトリ名
* @param[outEndDir] 最終階層の出力ディレクトリ名
*/
void main2(const fs::path& inEndDir,const fs::path& outEndDir);


/* TestCase3
* F-measureを計算して指定のフォーマットで出力
* @param[inEnddir] 最終階層の入力ディレクトリ名
* @param[filename] 出力ファイル名
*/
void main3(const fs::path& inEnddir,const fs::path& filename);

/* TestCase4
* F-measureの結果からPajek形式のファイルを作成
* @param[filename] F-measureの計算結果ファイル名；
* @param[outDir]   pajek結果ファイルの出力先ディレクトリ名；
*/
void main4(const fs::path& filename, const fs::path& outDir);

/* TestCase5
* blast結果ファイルからスコアを100/50刻みでヒストグラム化する
* また、結果ファイルのアライメント箇所は重複しない場合には加算
* 
*/
void main5();

/*TestCase6
* 最も高いクエリからスコアが100離れている場合に関しては省く処理を行う。
*/
void main6();


int main() {
    //main1();
    //main2("result","analyzedAgain");
    //main3("analyzedAgain","f-measure.txt");
    //main4("f-measure.txt","9_pajek");
    //main5();
    //main6();
    //main2("result2","analyzedAgain2");
    //main3("analyzedAgain2","f-measure2.txt");
    main4("f-measure2.txt","9_pajek2");

}

void main6() {

    const fs::path inDir = COMMON_DIR / "4_blast";   /*入力と出力で共通のディレクトリ*/
    std::string fileId;                                 /*ファイル識別子*/

    for (const fs::directory_entry& x : fs::directory_iterator(inDir/"result/")) {

        //BLAST結果ファイル解析用コンストラクタ生成
        fs::path infile = x.path();
        BlastParser bp(infile, std::make_shared<BlastParserPt2ex>());

        //BLAST結果ファイル解析
        fileId = x.path().filename().string();
        fs::path outTxt = inDir / "result2" / fileId;
        const std::string outTxtHeader = "";
        bp.run(outTxt,outTxtHeader);

    }


}



void main1() {
    fs::path faafile = COMMON_DIR / "2_protein/GCA_000009685.1_ASM968v1_protein.faa";
    fs::path out = COMMON_DIR / "out.fasta";

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

void main2(const fs::path& inEndDir,const fs::path& outEndDir) {
    //各fastaファイルのAccessionIDとstrainIDの対応mapの作成
    std::unordered_map<std::string, std::string> ac_st_map = Utils::keyValMakeFromFile(
        COMMON_DIR / "new_resources/05_prid_strain.txt", /**< 入力ファイル */
        "\t",                                           /**< 入力ファイルに使用されているデリミタ */
        [](std::vector<std::string>& vec)->std::vector<std::string>& {
            vec[0] = vec[0].substr(1);/** 先頭文字の'>'は省く。vec[1]（値）は変更しない */
            return vec;
        }
    );

    //ファイル名（パス）とstrainIDの対応mapの作成
    std::unordered_map<std::string, std::string> fpath_stid_map = Utils::keyValMakeFromFile(
        COMMON_DIR / "resources/prokaryotes_changed.txt", /**< 入力ファイル */
        "\t",                                             /**< 入力ファイルに使用されているデリミタ */
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
    std::string outTxtHeader;
    std::regex re{ "(.+).fasta_re.txt$" };
    std::smatch sm;
    fs::path commonDir = COMMON_DIR / "4_blast";
    std::shared_ptr<BlastParserPt1Imple> bpi = std::make_shared<BlastParserPt1Imple>(ac_st_map);
    for (const fs::directory_entry& x : fs::directory_iterator(commonDir /inEndDir)) {
        outfile = commonDir/outEndDir/ x.path().filename();
        std::cout << outfile << "\n";

        //入力ファイル名をstrainIDに変換して出力ファイルのheaderとして出力
        filename = x.path().filename().string();
        regex_match(filename, sm, re);
        outTxtHeader = fpath_stid_map.at(sm.str(1));
        std::cout << outTxtHeader << std::endl;
        std::make_unique<BlastParser>(x.path(), bpi)->run(outfile, outTxtHeader);
    }

}

void main3(const fs::path& inEndDir,const fs::path& filename) {

    fs::path commonDir = COMMON_DIR / "4_blast/";      /**< 入力ディレクトリ */
    fs::path outfile = COMMON_DIR / "new_resources" / filename; /**< 出力ファイル */
    Fmeasure fm(
        commonDir /inEndDir,
        outfile,
        std::make_unique<FmeasurePt1>(FmeasurePt1::CountUpWay::Exist)
    );

    fm.run();
}

void main4(const fs::path& filename, const fs::path& outDir) {
    fs::path infile = COMMON_DIR / "new_resources" / filename; /**< 入力ファイル */

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
        fs::path outfile = COMMON_DIR / outDir;
        outfile /= (*iter)->getPajekLbel() + ".net"; //string to fs::path
        (*iter)->output(outfile);
    }
}

void main5() {
    //referenceIDの変換は今回行わない。
    std::unordered_map<std::string, std::string> queref{
        std::make_pair("reference", "no-change")
    };

    const fs::path infile = COMMON_DIR / "4_blast/result/GCA_000009685.1_ASM968v1.fasta_re.txt";
    BlastParser bp(infile, std::make_shared<BlastParserPt2Imple>(queref));
    bp.run(COMMON_DIR / "new_resources/output.txt", "histgram");

}