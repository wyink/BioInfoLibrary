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


/**
* @brief fmeasureを計算するクラスの入出力を規定するインタフェース
* @details f-measureを計算する時に必要な「入力ファイル書式」\n
*         「出力ファイル書式」のインタフェースを規定する
*/
class IFmeasure {
public:

	IFmeasure() = default;
	virtual ~IFmeasure() = default;

	/*
	* @brief 入力ファイルに依存する処理
	* @details map[idA]={idA:count,idB:count,...} の形式に変換する   
	* @param[path] 入力ファイル/入力ディレクトリ
	*/
	virtual std::unordered_map<std::string, std::unordered_map<std::string, int> > 
		informat(const fs::path& path) = 0;

	/*
	* @brief 出力に利用するフォーマット部分を規定
	* @param[fmeasureMap] 鍵にはf-measureを計算した組合せ、値にその値を格納する            
	* @param[out] 出力ファイル名
	*/
	virtual void outformat(
		const std::map<std::string, float>& fmeasureMap,
		const fs::path out
	) = 0;

};

/**
* @brief 指定の入力形式からf-measureを計算する
* @details 入力ファイル形式例 ：strainA                                 \n 
*                           　  >abc123.1 strainA:1,strainD:3,strainC:2 \n 
*                           　  >abc124.2 strainA:1,strainD:4		    \n
*                               上記の形式4ファイル(strainA~strainD)	\n    
* 																		\n 
*        出力ファイル形式例 : strainA/strainB f-measure value		    \n
*/

class FmeasurePt1 :public IFmeasure{
public:
	/*
	* @brief あるidと自身を含むすべてのidとの関連値の集計方法
	* @details
	*        example:                                        \n
	*        												 \n
	*        strainA,strainB,strainC,straindDの4つのidが存在 \n
	*        ----------------------------------------------- \n
	*        strainA										 \n
	*        >abc123.1 strainA:1,strainD:3,strainC:2		 \n
	*        >abc124.2 strainA:1,strainD:4					 \n
	*  														 \n
	*        EOF											 \n
	*        ----------------------------------------------- \n
	* 
	*       集計方法：Amount         \n
	*       strainA-strainA 2  //1+1 \n
	*       strainA-strainB 0  //0+0 \n
	*       strainA-strainC 2  //2+0 \n
	*       strainA-strainD 7  //3+4 \n
	*                                \n
	*       集計方法:Exist           \n
	*       strainA-strainA 2  //1+1 \n
	*       strainA-strainB 0  //0+0 \n
	*       strainA-strainC 1  //1+0 \n
	*       strainA-strainD 2  //1+1 \n
	*/
	enum class CountUpWay{
		Amount, //存在量をそのまま積算
		Exist   //存在量は1or0で積算
	};

	/**
	* @brief コンストラクタ
	* @param[cuway] 集計方法をオブジェクト生成時に選択
	*/
	FmeasurePt1(
		const CountUpWay cuway
	) :cuway(cuway){}

	/**
	* @brief 
	* @details 入力ファイルが以下の時に対応                             \n
	* 																	\n
	*  ・それぞれのファイルがidに対応しており1行目にそのid名の記述あり  \n
	*  ・該当ファイルのidと他のidに対する値の記述が一行ごとに以下のよう
	*    にフォーマットされて出力されている． \n
	*  
	*  fileA ------------------------------------ \n
	*  id_A										  \n
	*  >abc123.1 id_A:1,id_D:3,id_C:2             \n
	*  >abc123.1 id_A:1,id_C:3                    \n
	*  ...                                        \n
	*  EOF                                        \n
	* 											  \n
	*  fileB------------------------------------- \n
	*  id_B                           		  	  \n
	*  >abc123.1 id_A:1,id_D:3,id_C:2 		  	  \n
	*  ...                            		  	  \n
	*  EOF                            		  	  \n
	*  \n										  \n
	*  id_C,id_Dも同様                		  	  \n
	*  ------------------------------------------ \n
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
 * @details 入力，出力のフォーマットはIFmeasureで規定
 */
class Fmeasure {
private:
	const fs::path indir;
	const fs::path outfile;
	std::unique_ptr<IFmeasure> fh;

	/**
	 * @brief f-measureの計算を行う（run関数で実行）
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
