#ifndef BLASTPARSER_H_
#define BLASTPARSER_H_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <unordered_map>
#include "Utils.h"

namespace fs = std::filesystem;

/**
* @enum BlastResultParam
* @brief BLAST application から出力されるタブ区切りのファイルの
*        それぞれのカラム
*/
enum BlastResultParam {
	QUERY_ID,
	REFERENCE_ID,
	IDENTITY,
	ALIGHN_LEN,
	MIS_MATCH_COUNT,
	GAP_COUNT,
	QUERY_START,
	QUERY_END,
	REF_START,
	REF_END,
	E_VALUE,
	SCORE,
	QUERY_LEN,
	REF_LEN
};

/**
* @brief BLAST結果ファイルを解析する際に利用できるハンドラ
* @details 
* BLAST結果ファイルを読み込む際、以下の3つのハンドラを設定することができる
* 1. BLAST結果ファイルの2カラム目の参照側IDをそれに紐づく別のIDに変換
* 2. BLAST結果ファイルの1レコードを利用して行う具体的な処理内容
* 3. 出力フォーマット
*/
class IBlastParser{
public:
	IBlastParser() = default;
	virtual ~IBlastParser() = default;

	/**
	* @brief BLAST結果ファイルの2カラム目の参照側IDをそれに紐づく別のIDに変換
	* @details このハンドラを実装する場合はそのメンバ変数に変換に使用するマップをコンストラクタで渡す必要がある
	* @param[reference] 変換したいID
	*/
	virtual const std::string& convert(const std::string& reference) = 0;

	/**
	* @brief BLAST結果ファイルの1レコードを利用して行う具体的な処理内容
	* @param[bquery] BLAST結果ファイルの第一カラム
	* @param[queryToRefVec] BLAST結果ファイルの1レコード
	*/
	virtual const std::string valueFormatter(
		const std::string& bquery, 
		const std::vector<std::vector<std::string> >& queryToRefVec
	) = 0;

	/**
	* @brief 出力フォーマットを決定する
	* @param[bquery] BLAST結果ファイルの第1カラム
	* @param[refcounter] bqueryに対する値
	*/
	virtual const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& refcounter
	) = 0;

};

/**
 * @brief blast結果ファイルの参照側を変換してその値をカウントアップして出力
 * @details
 *         Example :
 *                 A a   -> taxid 123
 *                 A a   -> taxid 123
 *                 A d   -> taxid 234
 *                 A b   -> taxid 123
 *                 
 *                 1. a,bをそれぞれ対応するidに変換する
 *                 2. 同じid(a,a)は2度目に出てきた場合にカウントしない。
 *                 3. a,bは変換後のidが同じであるためカウントアップ
 *                 4. 変換後のidの数も出力する。
 *                 5. 出力は以下を参照
 * -------------------------------------------
 *                    A 2
 *                    123:2,234:1
 * 
 * -------------------------------------------
 */
class BlastParserPt1Imple : public IBlastParser {
private:
	std::unordered_map<std::string, std::string> queRef;

public:
	explicit BlastParserPt1Imple(const std::unordered_map<std::string, std::string>& queRef) 
		:queRef(queRef) {}


	inline const std::string& convert(const std::string& reference) override {
		return queRef.at(reference);
	}

	const std::string valueFormatter(
		const std::string& bquery, 
		const std::vector<std::vector<std::string> >& queryToRefVec
	) override;


	 const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& refcounter
	) override;


};

/**
* @details 同一クエリに対し、referenceが同一である場合に参照の配列
*         のヒット領域が重ならない場合、スコアを加算する。
*         また、それぞのスコアを一定値で区切り、それをクエリごと
*         に出力する
* 　　　　ex. 
* 　　　　query1 reference1 
*/
class BlastParserPt2Imple : public IBlastParser {
private :
	std::unordered_map<std::string, std::string> queRef;

	const std::unordered_map<std::string, float>& refAlignDup3More(
		std::unordered_map<std::string, float>& scoreMap,
		std::vector<std::vector<std::string> >& requireRescore
	);

public:
	explicit BlastParserPt2Imple(const std::unordered_map<std::string, std::string>& queRef)
		:queRef(queRef) {}

	inline const std::string& convert(const std::string& reference) override {
		return queRef.at(reference);
	}

	const std::string valueFormatter(
		const std::string& bquery,
		const std::vector<std::vector<std::string> >& queryToRefVec
	) override;


	const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& scoreMap
	) override;

	
};


class BlastParser
{
private:
	const fs::path infile;
	std::shared_ptr<IBlastParser> bph;

public:
	explicit BlastParser(const fs::path& infile, std::shared_ptr<IBlastParser> bph);

	inline void setHandler(std::shared_ptr<IBlastParser> bph) {
		this->bph = std::move(bph);
	}

	/**
	* @brief blast結果ファイルの解析開始
	*/
	void run(const fs::path& outfile, const std::string& header);
};

#endif // !BLASTPARSER_H_