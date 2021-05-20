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
/*BlastPrserPt1Impleハンドラを利用する今回の場合は同一クエリ・同一参照に対して
* の複数箇所ヒットを考慮しない。そのため、複数個所にヒットしているものがある
* 場合、カウントが増える。しかし、今回の場合は同一タンパク質のあるなしを比較するため、
* この重複カウントに意味はない。
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

    main6();

}

void main6() {

    //入力ファイル
    const fs::path infile = "D:/perflingens/4_blast/result/GCA_000009685.1_ASM968v1.fasta_re.txt";
    
    //BLAST結果ファイル解析用コンストラクタ生成
    BlastParser bp(infile, std::make_shared<BlastParserPt2ex>());

    //BLAST結果ファイル解析
    std::string outTxt = "D:/perflingens/output.txt";
    std::string header = "クエリID\t参照ID（同一クエリにヒットした参照IDリストを降順に並べた際にスコア間隔が100以下のもの）";
    bp.run( outTxt, header);

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
    std::unordered_map<std::string, std::string> ac_st_map = Utils::keyValMakeFromFile(
        "G:/perflingens/new_resources/5_prid_strain.txt",/**< 入力ファイル */
        "\t",                                            /**< 入力ファイルに使用されているデリミタ */
        [](std::vector<std::string>& vec)->std::vector<std::string>& {
            vec[0] = vec[0].substr(1);/** 先頭文字の'>'は省く。vec[1]（値）は変更しない */
            return vec;
        }
    );

    //ファイル名（パス）とstrainIDの対応mapの作成
    std::unordered_map<std::string, std::string> fpath_stid_map = Utils::keyValMakeFromFile(
        "G:/perflingens/resources/prokaryotes_changed.txt", /**< 入力ファイル */
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
    for (const fs::directory_entry& x : fs::directory_iterator("G:/perflingens/4_blast/result/")) {
        outfile = "G:/perflingens/4_blast/analyzedAgain" / x.path().filename();
        std::cout << outfile << "\n";

        //入力ファイル名をstrainIDに変換して出力ファイルのheaderとして出力
        filename = x.path().filename().string();
        regex_match(filename, sm, re);
        header = fpath_stid_map.at(sm.str(1));
        std::cout << header << std::endl;
        std::make_unique<BlastParser>(x.path(), bpi)->run(outfile, header);
    }

}

void main3() {

    Fmeasure fm(
        "G:/perflingens/4_blast/analyzedAgain", /**< 入力ディレクトリ */
        "G:/perflingens/out.txt",               /**< 出力ファイル */
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
    std::unordered_map<std::string, std::string> queref{
        std::make_pair("reference", "no-change")
    };

    const fs::path infile = "D:/perflingens/4_blast/result/GCA_000009685.1_ASM968v1.fasta_re.txt";
    BlastParser bp(infile, std::make_shared<BlastParserPt2Imple>(queref));
    bp.run("D:/perflingens/output.txt", "histgram");

}