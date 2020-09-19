#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <functional>


/**
 * @brief 各種フォーマットの変換に関わるクラス
 */
class Formatter
{
	
public:
	/**
	 * @brief 複数行のfaaをFastaファイルに変換
	 * @param[faafile] 入力ファイル（拡張子faa）
	 * @param[out]     出力ファイル（拡張子fasta）
	 * @param[re]      idの選別に利用する正規表現
	 */
	static void FaaToFasta(
		std::string faafile,
		std::string out,
		std::regex re,
		std::function<bool(std::string)> filter
	);

	/**
	* @brief  複数行のFastaファイルに変換
	* @detail この関数はオーバーロード
	*         第3引数を一般的なものに固定している
	*         idのフィルタリングは行わない．
	*/
	static void FaaToFasta(
		std::string faafile,
		std::string out
	);
};

