#ifndef ANALYSIS_H_
#define ANALYSIS_H_

#include <filesystem>
#include <iostream>
#include <fstream>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>
#include <set>
#include <map>

namespace fs = std::filesystem;


/*
* @brief fmeasureを計算するクラスの入出力を規定するインタフェース
*         
*/
class IFmeasure {
public:

	IFmeasure() = default;
	virtual ~IFmeasure() = default;

	/*
	* @brief 入力ファイルに依存する処理
	* @details
	*        map[idA]={idA:count,idB:count,...} の形式に
	*        変換する
	* @param[path] 入力ファイル/入力ディレクトリ
	*/
	virtual std::unordered_map<std::string, std::unordered_map<std::string, int> > 
		informat(const fs::path& path) = 0;

	/*
	* @brief 出力に利用するフォーマット部分を規定
	* @param[fmeasureMap] 鍵にはf-measureを計算した組合せ、値にその値を格納
	*                     
	* @param[out] 出力ファイル名
	*/
	virtual void outformat(
		const std::map<std::string, float>& fmeasureMap,
		const fs::path out
	) = 0;

};

/**
* @details 入力ファイル形式例 ：strainA
*                           　>abc123.1 strainA:1,strainD:3,strainC:2
*                           　>abc124.2 strainA:1,strainD:4
*                             上記の形式4ファイル(strainA~strainD)
* 
*        出力ファイル形式例 : strainA/strainB f-measure value
*/

class FmeasurePt1 :public IFmeasure{
public:
	/*
	* @brief あるidと自身を含むすべてのidとの関連値の集計方法
	* @details
	*        example:
	*        
	*        strainA,strainB,strainC,straindDの4つのidが存在
	*        -----------------------------------------------
	*        strainA
	*        >abc123.1 strainA:1,strainD:3,strainC:2
	*        >abc124.2 strainA:1,strainD:4
	*  
	*        EOF
	*        -----------------------------------------------
	* 
	*       集計方法：Amount
	*       strainA-strainA 2  //1+1
	*       strainA-strainB 0  //0+0
	*       strainA-strainC 2  //2+0
	*       strainA-strainD 7  //3+4
	* 
	*       集計方法:Exist
	*       strainA-strainA 2  //1+1
	*       strainA-strainB 0  //0+0
	*       strainA-strainC 1  //1+0
	*       strainA-strainD 2  //1+1
	*/
	enum class CountUpWay{
		Amount, //存在量をそのまま積算
		Exist   //存在量は1or0で積算
	};

	/**
	* @param[cuway] 集計方法をオブジェクト生成時に選択
	*/
	FmeasurePt1(
		const CountUpWay cuway
	) :cuway(cuway){}

	/**
	* @details 入力ファイルが以下の時に対応
	* 
	*  ・それぞれのファイルがidに対応しており1行目にそのid名の記述あり
	*  ・該当ファイルのidと他のidに対する値の記述が一行ごとに以下のよう
	*    にフォーマットされて出力されている．
	*  
	*  fileA ------------------------------------
	*  id_A
	*  >abc123.1 id_A:1,id_D:3,id_C:2 
	*  >abc123.1 id_A:1,id_C:3
	*  ...
	*  EOF
	* 
	*  fileB------------------------------------
	*  id_B
	*  >abc123.1 id_A:1,id_D:3,id_C:2
	*  ...
	*  EOF
	* 
	* id_C,id_Dも同様
	*  -----------------------------------------
	* 
	* @param[indir] 全てのidのファイル(id_A~id_D)が存在するディレクトリ
	* @return {id_A:{id_A:fma,～id_D:fmd},...id_D:{id_A:fma,～id_D:fmd}}
	*/
	std::unordered_map<std::string, std::unordered_map<std::string, int> > 
		informat(const fs::path& indir) override;

	/**
	* @brief idA,idB,f-measure valueで出力
	* @param[fmeasureMap] タグと対応するf-measure値の連想配列。出力するため、mapを使用
	* @param[outfile] 出力ファイルパス
	*/
	void outformat(
		const std::map<std::string,float>& fmeasureMap,
		fs::path outfile
	) override;

private:
	const CountUpWay cuway;

};

/**
 * @brief f-measureの計算結果を送出するクラス
 *        入力，出力のフォーマットはIFmeasureで規定
 */
class Fmeasure {
private:
	const fs::path indir;
	const fs::path outfile;
	std::unique_ptr<IFmeasure> fh;

	/**
	 * @brief f-measureの計算を行う（run関数で実行される）
	 */
	const std::map<std::string, float> calcFmeasure(
		const std::unordered_map<std::string, std::unordered_map<std::string, int> >& idCounterMap
	);

public:
	explicit inline Fmeasure(
		const fs::path indir,
		const fs::path outfile,
		std::unique_ptr<IFmeasure> fh
	) :indir(indir), outfile(outfile), fh(std::move(fh)){}

	void setHandler(std::unique_ptr<IFmeasure> fh) {
		this->fh = std::move(fh);
	}

	void run();

};

#endif // !ANALYSIS_H_
