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
 * @brief Blast結果ファイルの参照をtaxidに変換し、各クエリに対してこのカウントを出力する処理を行う
 * @details
 *                Blast結果ファイルのクエリーと参照およびその参照が属するのtaxidが以下に示すような組合せである場合について考える。
 *					
 *                ### 入力例
 *                   Blast結果ファイルを抜粋してテーブルで表示
 *					|  Query   |  Reference  | taxid  |									
 *					| :------: | :---------: | -----: |									
 *					| QueryA   |	RefA     | 123	  |									
 *					| QueryA   |	RefA     | 123	  |									
 *					| QueryA   |	RefB     | 234	  |									
 *					| QueryA   |	RefD     | 123	  |									
 *					
 *                  ________________________________________________________________
 *					1. QueryA,QueryBをそれぞれ対応するtaxidに変換						
 *					2. 同じid(RefA)は2度登場するが、このような重複はカウントしない		
 *					3. RefA,RefDは属するtaxidが同じであるためカウントアップ				
 *					4. 変換後のtaxidの数も出力する。									
 *					5. 出力は以下を参照													
 *					________________________________________________________________
 *
 *			     ### 出力例
 *                 QueryAは「2」種類のtaxid(123,234)のどれかに所属する可能性が示されている。
 *                 注意点として、QueryAは123に属する2つの異なる配列にトップヒットしている。
 *				   ____________															
 *				   QueryA 2	                  														
 *				   123:2,234:1																				
 *				   ____________														
 */
class BlastParserPt1Imple : public IBlastParser {
private:
	//変換用のマップ（ex. 参照ID＝＞taxonomyId）
	std::unordered_map<std::string, std::string> queRef;

public:
	explicit BlastParserPt1Imple(const std::unordered_map<std::string, std::string>& queRef) 
		:queRef(queRef) {}


	/**
	* @brief BLAST結果ファイルの2カラム目の参照側IDをそれに紐づく別のIDに変換
	* @details このハンドラを実装する場合はそのメンバ変数に変換に使用するマップをコンストラクタで渡す必要がある
	* @param[reference] 変換したいID
	*/
	inline const std::string& convert(const std::string& reference) override {
		return queRef.at(reference);
	}

	/**
	* @brief BLAST結果ファイルの1レコードを利用して行う具体的な処理内容
	* @param[bquery] BLAST結果ファイルの第一カラム
	* @param[queryToRefVec] BLAST結果ファイルの1レコード
	*/
	const std::string valueFormatter(
		const std::string& bquery,
		const std::vector<std::vector<std::string> >& queryToRefVec
	) override;

	/**
	* @brief 出力フォーマットを決定する
	* @param[bquery] BLAST結果ファイルの第1カラム
	* @param[refcounter] bqueryに対する値
	*/
	 const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& refcounter
	) override;


};

/**
 * @brief Blast結果ファイルより、各クエリに対する参照のスコア分布を出力する
 * @details
 *          各クエリに対してヒットした参照のスコア群を保持する。しかし、同一クエリに対して「同一の
 *          参照に複数ヒット」かつ「そのアライメント領域が重複しない場合」は加算したスコアを保持
 *          そうして各クエリが保持するスコア群を一定値で区切って出力する。以下にその例を示す。
 *              ### 入力例
 *                  同一クエリ・同一参照に対して参照のアライメントが重複しない場合
 *
 * 					|  Query   | Reference   | RefAlignStart | RefAlignEnd | Score |
 *					| :------: | :---------: | ------------: | ----------: |  ---: |
 *					| QueryA   | Reference1  | 	1            | 150	       |   190 |
 *					| QueryA   | Reference1  | 	160          | 180	       |    30 |
 *					| QueryA   | Reference2  | 	1            | 190	       |   300 |
 *					| QueryB   | ...         | ...	         | ...	       |   ... |
 *
 *              ### 出力例
 *                  重複しないため190と30を加算して出力
 *                    | score  | 100 | 200 | 300 | 400 | 500 | ... | n*100 |
 *                    | :---:  | --: | --: | --: | --: | --: | --: | ----: |
 *                    | QueryA |   0 |   1 |  1  |  0  | 0   |  0  | 	0  |
 *                    | QueryB | ... | ... | ... | ... | ... | ... |   ... |
 * 
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
	
	//変換が必要ない場合
	explicit BlastParserPt2Imple()
		:queRef{ std::make_pair("","") } {}

	/**
	* @brief BLAST結果ファイルの2カラム目の参照側IDをそれに紐づく別のIDに変換
	* @details このハンドラを実装する場合はそのメンバ変数に変換に使用するマップをコンストラクタで渡す必要がある
	* @param[reference] 変換したいID
	*/
	inline const std::string& convert(const std::string& reference) override {
		return queRef.at(reference);
	}

	/**
	* @brief BLAST結果ファイルの1レコードを利用して行う具体的な処理内容
	* @param[bquery] BLAST結果ファイルの第一カラム
	* @param[queryToRefVec] 同一クエリのBLAST結果ファイルの1レコードのベクタ
	*/
	const std::string valueFormatter(
		const std::string& bquery,
		const std::vector<std::vector<std::string> >& queryToRefVec
	) override;

	/**
	* @brief 出力フォーマットを決定する
	* @param[bquery] BLAST結果ファイルの第1カラム
	* @param[scoreMap] bqueryに対する値
	*/
	const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& scoreMap
	) override;

};

/*
* @brief 同一クエリに対して相同性検索でヒットした参照ID群において、
*        スコアが100より離れる場合は出力しない。
*        同一クエリ・同一参照に対する相同性検索のスコアの加算方式は親クラスの方式をそのまま継承
*/
class BlastParserPt2ex :public BlastParserPt2Imple {
public:
	explicit BlastParserPt2ex(){} //親クラスのデフォルトコンストラクタ呼び出し後に構築

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
	void run(const fs::path& outfile, const std::string& outTxtHeader,bool isHeader=false);
};

#endif // !BLASTPARSER_H_