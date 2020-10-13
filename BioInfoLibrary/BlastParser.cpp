#include "BlastParser.h"
#include "Utils.h"

BlastParser::BlastParser(const fs::path& infile, std::shared_ptr<BlastParserHandler> bph) :
	infile(infile), bph(std::move(bph)){}


void BlastParser::run(const fs::path& outfile, const std::string& header) {

	std::ifstream in{ infile };
	if (!in.is_open()) {
		std::cout << "Can't open the file!" << std::endl;
		return;
	}

	std::string line;
	std::vector<std::string> vec;
	std::vector<std::vector<std::string> > queryToRefVec;
	std::string query;
	std::string bquery;
	
	//一行目で行う処理
	std::getline(in, line);
	vec = Utils::split(line, "\t");
	query = bquery = vec[0];
	queryToRefVec.emplace_back(vec);
	
	//2行目以降で行う処理
	std::ofstream out(outfile);
	out << header << "\n";
	while (std::getline(in, line)) {
		vec = Utils::split(line, "\t");
		query = vec[0];

		if (query == bquery) {
			//queryが同一のものはすべて回収
			//blastで参照側が整列して出力される保証がないため

			queryToRefVec.emplace_back(vec);

		}
		else {//(A,a)->(B,b)
			//これまでの行で読み込んだクエリー (A) の書き出し
			out << bph->valueFormatter(bquery, queryToRefVec);
			queryToRefVec.clear();

			bquery = query;//queryを(b)に再登録

			//現在の行(b)の処理
			queryToRefVec.emplace_back(vec);
		}

	}

	//EOFで行う処理
	out << bph->valueFormatter(bquery, queryToRefVec);

	in.close();
	out.close();

}

//map.countの処理がO(n)
const std::string BlastParserPt1Imple::valueFormatter(const std::string& bquery, const std::vector<std::vector<std::string> >& queryToRefSet) {
	std::map<std::string, int> refcounter; /**< keyは参照を変換後のid valはその存在数 */
	std::string refid; /**< 参照を変換後に使用するid*/


	for (auto ref : queryToRefSet) {

		refid = convert(ref[1]);
		refcounter[refid]++;

	}

	return outformat(bquery, refcounter);
}

const std::string BlastParserPt1Imple::outformat(const std::string& bquery, const std::map<std::string, int>& refcounter){
	std::stringstream outtext;
	outtext << ">" << bquery << "\t" << refcounter.size() << "\n";

	for (const auto& [ref, counter] : refcounter) {
		outtext << ref << ":" << counter << ",";
	}

	std::string ret = outtext.str();
	ret.pop_back();
	ret += "\n";

	return ret;
}


const std::string BlastParserPt2Imple::valueFormatter(const std::string& bquery, const std::vector<std::vector<std::string> >& queryToRefVec) {
	
	float score   = 0 ;
	int startBase = 0 ;
	int endBase   = 0 ;
	int idx = 0;
	std::string bref;
	std::pair<int,int> range;
	std::vector<int> scoreVec;

    //最初の要素のみ
	startBase = std::stoi(queryToRefVec[0][7]);
	endBase = std::stoi(queryToRefVec[0][8]);
	score = std::stoi(queryToRefVec[0][10]);

	//クエリと参照が逆向きにアライメントした場合
	if (endBase < startBase) {
		std::swap(startBase, endBase);
	}

	bref = queryToRefVec[0][2];
	range = std::make_pair(startBase, endBase);
	scoreVec[idx] = score;
	++idx;
	
	//for (const auto& ref : queryToRefVec) {
	for (auto iter = (queryToRefVec.begin())+1;iter != queryToRefVec.end();++iter){
		auto ref = *iter;

		startBase = std::stoi(ref[7]);
		endBase = std::stoi(ref[8]);
		score = std::stoi(ref[10]);

		if (endBase < startBase) {
			std::swap(startBase, endBase);
		}


		//同じ参照に複数回ヒットした場合
		if (ref[2] == bref) {
			//アライメントが重複しない場合(スコアの加算）
			if (range.second < startBase || range.first > endBase) {
				scoreVec[idx] += score;
			}
			else{
				//アライメント部分が前の行と重複するためスキップ
				continue;
			}

		}
		else { //前の行と異なる参照にヒットした場合
			scoreVec[idx] = score ;
		}

		range = std::make_pair(startBase, endBase);
		bref = ref[2];
		++idx;
	}

}

const std::string BlastParserPt2Imple::outformat(const std::string& bquery, const std::map<std::string, int>& refcounter) {

}


