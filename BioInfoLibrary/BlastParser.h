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
 * @brief blast結果ファイルの参照をtaxidに変換し、各クエリに対してこのカウントを出力する処理を行う
 * @details
 *         Example : Blast結果ファイルのクエリーと参照およびその参照が属するのtaxidが  \n
 *                   以下に示すような組合せである場合について考える。				   \n
 * 																					   \n
 *                 　Query  Reference  taxid										   \n
 *                 　========================										   \n
 *                 　QueryA   RefA      123							   				   \n
 *                 　QueryA   RefA      123					    					   \n
 *                 　QueryA   RefB      234							   				   \n
 *                 　QueryA   RefD      123								   			   \n
 *                 													   				   \n
 *                 1. QueryA,QueryBをそれぞれ対応するtaxidに変換				   	   \n
 *                 2. 同じid(RefA)は2度登場するが、このような重複はカウントしない 	   \n
 *                 3. RefA,RefDは属するtaxidが同じであるためカウントアップ	   		   \n
 *                 4. 変換後のtaxidの数も出力する。					   				   \n
 *                 5. 出力は以下を参照												   \n
 *																					   \n
 *                 QueryAは「2」種類のtaxidどどれかに所属する可能性があり、			   \n
 *                 それは123もしくは234である。しかし、QueryAは123に属する2つの異なる  \n
 *                 配列にトップヒットしている。										   \n
 *                 ------------------------						   					   \n
 *                  QueryA 2											   			   \n
 *                  123:2,234:1									    				   \n
 *                 ------------------------						   					   \n
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
* @brief blast結果ファイルより、各クエリに対する参照のスコア分布を出力する
* @details 各クエリに対してヒットした参照のスコア群を保持する。しかし、同一クエリに対して「同一の		\n	
*          参照に複数ヒット」かつ「そのアライメント領域が重複しない場合」は加算したスコアを保持         \n
*          そうして各クエリが保持するスコア群を一定値で区切って出力する。								\n
* 																										\n
* 　　　　 Example :Blast結果ファイル（一部のカラムを抜粋加工）が以下のようになった場合					\n
* 																										\n
*                       <<入力>>　同一クエリ・同一参照に対して参照のアライメントが重複しない場合		\n
*                       <<出力>>  重複しないため190と30を加算して出力する。								\n
* 																										\n
*                　　　　入力例																		    \n
*                　　　　   Query   Reference   ReferenceAlignmemtStart  ReferenceAlignmentEnd  Score	\n
*                　　　　   ==========================================================================	\n
* 　　　　       　　　　   query1  reference1          1                        150              190	\n
* 　　　　       　　　　   query1  reference1         160                       180               30	\n
* 　　　　       　　　　   query1  reference2          1                        190              300	\n
*                　　　　   query2  ....																\n
* 				 　　　　																				\n
*                　　　　出力例																		    \n
*                　　　　    score  100 200 300 400 500 ... n*100										\n
*                　　　  　  query1   0   1   1   0   0 ... 		                                    \n
*                            query2  ...
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

/**
* @brief BLAST結果ファイルの読み込むを実行するクラス
* @details このクラスにはBLAST結果ファイルをどのように解析するかを指定するための
*         ハンドラを必要とする。
*/
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