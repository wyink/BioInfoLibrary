#include "Formatter.h"


void Formatter::FaaToFasta(
		std::string faafile, 
		std::string out, 
		std::regex re,
		std::function<bool(std::string)> filter
	) 
{
	std::ifstream in{ faafile };

	if (!in.is_open()) {
		std::cout << "Can't open the file!" << std::endl;
		return;
	}

	std::string line;
	std::smatch match;       /**< matchした文字列 */
	std::string id;          /**< id格納用 */
	std::string seqs;        /**< 配列格納用 */

	//一行目のみ別処理
	std::getline(in, line);
	std::regex_search(line, match, re);
	id = match.str(1);

	//出力ファイル
	std::ofstream of{ out };
	while (std::getline(in, line)) {
		if (line[0] == '>') {
			

			//前の行の情報を出力
			if (filter(id)) {
				of << id << "\n" << seqs << "\n";
			}
			seqs.clear();

			//現在の行の情報を取得
			std::regex_search(line, match, re);
			id = match.str(1);

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

void Formatter::FaaToFasta(std::string faafile,std::string out) {
	std::regex re{ "(^>[A-Z]+?\\d+?\\.\\d)" };

	Formatter::FaaToFasta(
		faafile, 
		out, 
		re, 
		[](std::string id)->bool {return true;}
	);
}

