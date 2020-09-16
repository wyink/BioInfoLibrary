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
	// ����Label�����Ă�H
	//���ׂĂ̐F��ύX����
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
	//inline Node() {}	//Node[]�Ŏg�p�����
	Node(const int nodeid,const Label& label,const int x,const int y) :
		nodeid(nodeid),
		label(label),
		x(x),
		y(y)
	{}
};


/**
* @brief ��ӂ̕����񃉃x��
*/
class Label {
	std::string label;

public:
	inline Label(){}	//Vertices��Nodeobj�쐬���ɌĂяo�����
	explicit Label(const std::string lavel) :label(label) {}
	std::string getLabel() const;
	void setLabel(const std::string label) ;

	
};


/**
* @brief "project/sample"�Ȃǂ̊K�w���x���������N���X
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
* @brief ���ւ̂���2�̃m�[�h�̏W���������N���X
*/
class Edges
{
	std::vector<std::pair<int, int>>& mpair;
	std::set<int> mnodeset;

public:

	//���ւ̂���2�̃m�[�h���X�g�̃|�C���^���󂯎��D
	explicit Edges(std::vector<std::pair<int, int>>& mpair);

	//module���`�����Ă��邩�ǂ���
	bool isModule(int index);

};


