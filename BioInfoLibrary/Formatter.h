#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <functional>


/**
 * @brief 各種フォーマットの変換に関わるクラス
 * @detail 
 */
class FaaToFasta
{
private:
	const std::string infile; /**< 入力ファイル名 */
	const std::string out; /**< 出力ファイル名 */
	std::regex re; /**< idに利用する文字列を決定するための正規表現 */
	std::function<bool(std::string)> filter ; /** 特定のidの行のみを選択するための関数 */

public:
	/**
	 * @brief 複数行のfaaをFastaファイルに変換
	 * @detail このコンストラクタは以下の基準処理のみをになう．
	 *         必要に応じてメンバーを変更することで以下の処理を変更できる
	 * 
	 * 　　　　Example.
	 *          >ABC1234.1 ...some redundant infomation
	 *         
	 *         //action
	 *         ・>ABC1234.1をIDに利用する．
	 *         ・ID以降の情報をフィルターとして利用しない．
	 */
	FaaToFasta(std::string infile, std::string out);

	/**
	 * @brief idに利用する文字列を決定するための正規表現を変更
	 * @return 自身の参照を返す
	 */
	FaaToFasta& setIdFilter(std::regex re); 

	/**
	 * @brief  特定のidの行のみにフィルタリングするための関数
	 * @return 自身の参照を返す
	 * @detail この関数をsetすることで必要なIDと不必要なIDを選別する
	 *         ことができる。設定ナシの場合はすべてのIDを出力する．
	 */
	FaaToFasta& setIdFilterCall(std::function<bool(std::string)> fileter);

	/**
	 * @brief faaファイル形式をfastaファイル形式に変更する．
	 */
	void run();

};

