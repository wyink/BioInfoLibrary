#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>
#include "Utils.h"

class BlastParserHandler {

public:
	BlastParserHandler() = default;
	virtual ~BlastParserHandler() = default;

	virtual const std::string& convert(const std::string& reference) = 0;

	virtual const std::string valueFormatter(
		const std::string& bquery, 
		const std::set<std::vector<std::string> >& queryToRefSet
	) = 0;

	virtual const std::string outformat(
		const std::string& bquery, 
		const std::map<std::string, int>& refcounter
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
	std::map<std::string, std::string> queRef;

public:
	explicit BlastParserPt1Imple(const std::map<std::string, std::string>& queRef) 
		:queRef(queRef) {}


	inline const std::string& convert(const std::string& reference) override {
		return queRef[reference];
	}

	const std::string valueFormatter(
		const std::string& bquery, 
		const std::set<std::vector<std::string> >& queryToRefSet
	) override;

	const std::string outformat(
		const std::string& bquery, 
		const std::map<std::string, int>& refcounter
	) override;

};


class BlastParser
{
private:
	const std::string infile;
	BlastParserHandler& bph;

public:
	explicit BlastParser(const std::string& infile, BlastParserHandler& bph);

	inline void setHandler(BlastParserHandler& bph) {
		this->bph = bph;
	}

	/**
	* @brief blast結果ファイルの解析開始
	*/
	void run(const std::string& outfile);
};

