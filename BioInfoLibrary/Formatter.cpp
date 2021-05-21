#include "Formatter.h"



FaaToFasta::FaaToFasta(const fs::path& infile, const fs::path& out):
	infile(infile),
	out(out),
	re("(^>[A-Z]+?\\d+?\\.\\d)") ,
	filter(NULL){}

void FaaToFasta::run() {
	std::ifstream in{ infile };

	if (!in.is_open()) {
		std::cout << "Can't open the file!" << std::endl;
		return;
	}

	std::string line;
	std::smatch match;       /**< matchした文字列 */
	std::string id;          /**< id格納用 */
	std::string seqs;        /**< 配列格納用 */
	std::string idLine;      /**< idのフィルターに使用 */

	//一行目のみ別処理
	std::getline(in, line);
	std::regex_search(line, match, re);
	id = match.str(1);
	idLine = line;

	//出力ファイル
	std::ofstream of{ out };
	while (std::getline(in, line)) {
		if (line[0] == '>') {
			

			//前の行の情報を出力
			if (filter(idLine)) {
				of << id << "\n" << seqs << "\n";
			}
			seqs.clear();

			//現在の行の情報を取得
			std::regex_search(line, match, re);
			id = match.str(1);
			idLine = line;
		}
		else {
			seqs += line;
		}
	}

	//最後の行の情報を出力
	of << id << "\n" << seqs << "\n";
	seqs.clear();

	//自動的にデストラクタは呼ばれる
	in.close();
	of.close();

}

FaaToFasta& FaaToFasta::setIdFilter(std::regex re) {
	this->re = re;
	return *this;
}

FaaToFasta& FaaToFasta::setIdFilterCall(std::function<bool(std::string)> filter) {
	this->filter = filter;
	return *this;
}


