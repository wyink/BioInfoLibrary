#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <set>

//�Q�Ƃ�Ԃ��Ă�����̂��m�F->temporary!

class Pajek;
class PajekParser {
	const std::string infile;;

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
	float x;
	float y;
	std::string icolor; //�󕶎���ŏ�����
	std::string bcolor; //�󕶎���ŏ�����


public:
	//printout����Ƃ���ic,bc������K�v����
	//�`�F�C�����\�b�h�ŃR���X�g���N�^�̎���
	Node(const int nodeid, const Label& label) :
		nodeid(nodeid),label(label),
		x(-1),y(-1){}
	
	//�C�ӂ̈���
	Node& setPosition(const float x, const float y);
	Node& setInnerColor(const std::string icolor);
	Node& setBorderColor(const std::string bcolor);

	const std::string& getSingleLine();
};


/**
* @brief ��ӂ̕����񃉃x��
*/
class Label {
	std::string label;

public:
	inline Label(){}	//Vertices��Nodeobj�쐬���ɌĂяo�����
	explicit Label(const std::string label) :label(label) {}
	std::string getLabel();
	std::string getOutputLabel();
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


