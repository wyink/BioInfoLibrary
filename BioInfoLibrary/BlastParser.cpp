#include "BlastParser.h"
#include "Utils.h"

BlastParser::BlastParser(const std::string& infile, BlastParserHandler& bph) :
	infile(infile), bph(bph){}


void BlastParser::run(const std::string& outfile, const std::string& header) {

	std::ifstream in{ infile };
	if (!in.is_open()) {
		std::cout << "Can't open the file!" << std::endl;
		return;
	}

	std::string line;
	std::vector<std::string> vec;
	std::set<std::vector<std::string> > queryToRefSet;
	std::string query;
	std::string bquery;
	
	//一行目で行う処理
	std::getline(in, line);
	vec = Utils::split(line, "\t", 2);
	query = bquery = vec[0];
	queryToRefSet.insert(vec);
	
	//2行目以降で行う処理
	std::ofstream out(outfile);
	out << header << "\n";
	while (std::getline(in, line)) {
		vec = Utils::split(line, "\t");
		query = vec[0];

		if (query == bquery) {
			//queryが同一のものはすべて回収
			//blastで参照側が整列して出力される保証がないため

			queryToRefSet.insert(vec);

		}
		else {//(A,a)->(B,b)
			//これまでの行で読み込んだクエリー (A) の書き出し
			out << bph.valueFormatter(bquery, queryToRefSet);
			queryToRefSet.clear();

			bquery = query;//queryを(b)に再登録


			//現在の行(b)の処理
			queryToRefSet.insert(vec);
		}

	}

	//EOFで行う処理
	out << bph.valueFormatter(bquery, queryToRefSet);

	in.close();
	out.close();

}

//map.countの処理がO(n)
const std::string BlastParserPt1Imple::valueFormatter(const std::string& bquery, const std::set<std::vector<std::string> >& queryToRefSet) {
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




