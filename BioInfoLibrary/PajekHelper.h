#pragma once
#include <iostream>
#include <fstream>
#include <regex>
#include <set>


class Pajek;
class PajekParser {

public:
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
	const std::string upclass;
	const std::string lwclass;

public:

	LabelDouble(std::string label, std::string upclass, std::string lwclass) :
		Label(label),
		upclass(upclass),
		lwclass(lwclass)
	{}
	void print_upper() const;
	void print_lower() const;
	void print_label() const;
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


