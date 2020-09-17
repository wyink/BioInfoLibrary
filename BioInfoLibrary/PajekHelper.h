#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <set>

/**
 * @brief	Pajekアプリケーションで利用するファイルの解析
 * 
 * @detail	Pajekアプリケーションでは拡張子.netのファイルを出力として利用する．
 *			このクラスではそのファイルを解析してその内容を保管するためのPajek
 *			オブジェクトに変換する．
 */
class Pajek;
class PajekParser {
	const std::string infile;

public:
	/**
	 * @brief			PajekParserオブジェクトのコンストラクタ
	 * @param[infile]	読み込むファイル（拡張子.net)
	 * @detail			読み込みたい.netファイルのパスをセット
	 */
	PajekParser(const std::string infile);

	/**
	 * @brief	.netファイルをロード
	 * @return	入力ファイルの情報を保持したPajekオブジェクト
	 */
	Pajek load(); 
};

/**
 * @brief	Pajekファイルの要素(Vertices,Edges)を保持するオブジェクト
 */
class Vertices;
class Edges;
class Pajek
{
	Vertices& vt;
	Edges& egs;

public:
	/**
	 * @brief Pajekオブジェクトのコンストラクタ
	 * @param[vt]	全Nodeの定義の集合
	 * @param[egs]	相関のある2つのNodeの集合
	 * 
	 */
	inline Pajek(
		Vertices& vt,
		Edges& egs
	):vt(vt),egs(egs){}
};

/**
* @brief Pajekの.netファイルを構成する各Nodeの定義
*/
class Node;
class Vertices
{
	std::vector<Node>& nodeElements;

public:

	/**
　　 * @brief	Verticesクラスのコンストラクタ
　　 * @detail	.netファイルにおける各Nodeの定義部分
　　 */
	explicit Vertices(std::vector<Node>& nodeElements);

	/**
	 * @brief			任意の色でVerticesオブジェクトのNodeの色を更新
	 * @param[color]	全てのNodeを更新する色(ex. RGB(0,0,0) )
	 */
	void setColor(const std::string& color);
};


/**
* @brief 
*/
class Label;
class Node
{
	int nodeid;
	Label& label;
	float x;
	float y;
	std::string icolor; //空文字列で初期化
	std::string bcolor; //空文字列で初期化


public:
	/**
	 * @brief Nodeクラスのコンストラクタ
	 * @param[nodeid]	Nodeの一意のID
	 * @param[label]		Nodeが保持するラベル
	 */
	Node(const int nodeid, Label& label) :
		nodeid(nodeid),label(label),
		x(-1),y(-1){}
	
	/**
	 * @brief		各Nodeの座標をセット
	 * @param[x]	各Nodeのx座標
	 * @param[y]	各Nodeのy座標
	 */
	Node& setPosition(
		const float x, 
		const float y
	);

	/**
	 * @brief			各Nodeの内側の色をセット
	 * @param[icolor]	各Nodeを更新する内側の色
	 */
	Node& setInnerColor(const std::string& icolor);

	/**
	 * @brief			各Nodeの境界線の色をセット
	 * @param[bcolor]	各Nodeを更新する境界線の色
	 */
	Node& setBorderColor(const std::string& bcolor);

	/**
	 * @brief	出力に使用するフォーマット文字列を取得
	 * @return	.netでの出力に利用する文字列（改行含む）
	 */
	const std::string getSingleLine();
};


/**
* @brief 一意の文字列ラベル
*/
class Label {
	std::string label;

public:

	/**
	 * @brief			Labelクラスのコンストラクタ
	 * @param[label]	ラベルで使用するラベル用の文字列
	 */
	explicit Label(const std::string label) :
		label(label) {}

	/**
	 * @brief	label文字列を取得
	 * @return	ラベルに利用している文字列('"'を含まない）
	 */
	std::string getLabel();

	/**
	 * @brief　出力に使用するlabel文字列
	 * @return ラベルに利用している文字列（'"'を含む）
	 */
	std::string getOutputLabel();

	/**
	 * @brief			label文字列をセット
	 * @param[label]	更新したいラベル文字列
	 */
	void setLabel(const std::string label) ;

	
};


/**
* @brief "project/sample"などの階層ラベルを扱うクラス
*/
class LabelDouble:Label {
	std::string upLabel;
	std::string lwLabel;
	const std::string sep;

public:

	/**
	 * @brief			LabelDoubleのコンストラクタ
	 * @param[label]	初期化するためのラベル文字列
	 * @param[sep]		階層ラベルを分割するデリミタ
	 * @detail			階層ラベルの作成
	 */
	LabelDouble(std::string label, const char* sep);

	/**
	 * @brief	上階層のラベルを取得
	 * @return	上階層のラベル文字列
	 */
	std::string getUpLabel();

	/**
	 * @brief	下階層のラベルを取得
	 * @return	下階層のラベル文字列
	 */
	std::string getLwLabel();

};



/**
* @brief 相関のある2つのノードの集合を扱うクラス
*/
class Edges
{
	std::vector<std::pair<int, int>>& mpair;
	std::set<int> mnodeset;

public:

	/**
	 * @brief Edgesクラスのコンストラクタ
	 * @detail .netファイルにおけるNodeの繋がりを示す領域部分
	 */
	explicit Edges(std::vector<std::pair<int, int>>& mpair);

	/**
	 * @brief 該当Nodeがモジュールを形成しているかどうか
	 * @retval true Nodeがモジュールを形成している場合
	 * @retval false Nodeがモジュールを形成していない場合
	 */
	bool isModule(int index);

};


