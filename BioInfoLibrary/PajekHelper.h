#pragma once
#include <iostream>
#include <fstream>
#include <regex>
#include <set>


class Pajek;
class PajekParser {
	const std::string infile;

public:
	PajekParser(const std::string infile) 
		:infile(infile) {}

	Pajek& load(); 
};

class Vertices;
class Edges;
class Pajek
{
	Vertices& vt;
	Edges& egs;

public:
	inline Pajek(Vertices& vt,Edges& egs):
	vt(vt),egs(egs){}
};

class Node;
class Vertices
{
	std::vector<Node>& nodeElements;

public:
	// そのLabel持ってる？
	//すべての色を変更する
	explicit Vertices(std::vector<Node>& nodeElements);
};

class Label;
class Node
{
	int nodeid;
	const Label& label;
	int x;
	int y;


public:
	//inline Node() {}	//Node[]で使用される
	Node(const int nodeid,const Label& label,const int x,const int y) :
		nodeid(nodeid),
		label(label),
		x(x),
		y(y)
	{}
};


/**
* @brief 一意の文字列ラベル
*/
class Label {
	std::string label;

public:
	inline Label(){}	//VerticesでNodeobj作成時に呼び出される
	explicit Label(const std::string lavel) :label(label) {}
	std::string getLabel() const;
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

	LabelDouble(std::string label, const std::string sep);
	std::string getUpLabel() const ;
	std::string getLwLabel() const ;


};



/**
* @brief 相関のある2つのノードの集合を扱うクラス
*/
class Edges
{
	std::vector<std::pair<int, int>>& mpair;
	std::set<int> mnodeset;

public:

	//相関のある2つのノードリストのポインタを受け取る．
	explicit Edges(std::vector<std::pair<int, int>>& mpair);

	//moduleを形成しているかどうか
	bool isModule(int index);

};


