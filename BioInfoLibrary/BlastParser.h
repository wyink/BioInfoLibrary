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

class BlastParserHandler {

public:
	BlastParserHandler() = default;
	virtual ~BlastParserHandler() = default;

	virtual const std::string& convert(const std::string& reference) = 0;

	virtual const std::string valueFormatter(
		const std::string& bquery, 
		const std::vector<std::vector<std::string> >& queryToRefVec
	) = 0;

	virtual const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& refcounter
	) = 0;

};

/**
 * @detail blast結果ファイルの参照側を変換してその値をカウントアップして出力
 * 
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
class BlastParserPt1Imple : public BlastParserHandler {
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
* @detail 同一クエリに対し、referenceが同一である場合に参照の配列
*         のヒット領域が重ならない場合、スコアを加算する。
*         また、それぞのスコアを一定値で区切り、それをクエリごと
*         に出力する
* 　　　　ex. 
* 　　　　query1 reference1 
*/
class BlastParserPt2Imple : public BlastParserHandler {
private :
	std::map<std::string, std::string> queRef;

	const std::unordered_map<std::string, float>& refAlignDup3More(
		std::unordered_map<std::string, float>& scoreMap,
		std::vector<std::vector<std::string> >& requireRescore
	);

public:
	explicit BlastParserPt2Imple(const std::map<std::string, std::string>& queRef)
		:queRef(queRef) {}

	inline const std::string& convert(const std::string& reference) override {
		return queRef[reference];
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
	std::shared_ptr<BlastParserHandler> bph;

public:
	explicit BlastParser(const fs::path& infile, std::shared_ptr<BlastParserHandler> bph);

	inline void setHandler(std::shared_ptr<BlastParserHandler> bph) {
		this->bph = std::move(bph);
	}

	/**
	* @brief blast結果ファイルの解析開始
	*/
	void run(const fs::path& outfile, const std::string& header);
};

#endif