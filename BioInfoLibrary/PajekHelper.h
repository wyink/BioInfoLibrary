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


