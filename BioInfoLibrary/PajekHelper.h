#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <set>

/**
 * @brief	Pajek�A�v���P�[�V�����ŗ��p����t�@�C���̉��
 * 
 * @detail	Pajek�A�v���P�[�V�����ł͊g���q.net�̃t�@�C�����o�͂Ƃ��ė��p����D
 *			���̃N���X�ł͂��̃t�@�C������͂��Ă��̓��e��ۊǂ��邽�߂�Pajek
 *			�I�u�W�F�N�g�ɕϊ�����D
 */
class Pajek;
class PajekParser {
	const std::string infile;

public:
	/**
	 * @brief			PajekParser�I�u�W�F�N�g�̃R���X�g���N�^
	 * @param[infile]	�ǂݍ��ރt�@�C���i�g���q.net)
	 * @detail			�ǂݍ��݂���.net�t�@�C���̃p�X���Z�b�g
	 */
	PajekParser(const std::string infile);

	/**
	 * @brief	.net�t�@�C�������[�h
	 * @return	���̓t�@�C���̏���ێ�����Pajek�I�u�W�F�N�g
	 */
	Pajek load(); 
};

/**
 * @brief	Pajek�t�@�C���̗v�f(Vertices,Edges)��ێ�����I�u�W�F�N�g
 */
class Vertices;
class Edges;
class Pajek
{
	Vertices& vt;
	Edges& egs;

public:
	/**
	 * @brief Pajek�I�u�W�F�N�g�̃R���X�g���N�^
	 * @param[vt]	�SNode�̒�`�̏W��
	 * @param[egs]	���ւ̂���2��Node�̏W��
	 * 
	 */
	inline Pajek(
		Vertices& vt,
		Edges& egs
	):vt(vt),egs(egs){}
};

/**
* @brief Pajek��.net�t�@�C�����\������eNode�̒�`
*/
class Node;
class Vertices
{
	std::vector<Node>& nodeElements;

public:

	/**
�@�@ * @brief	Vertices�N���X�̃R���X�g���N�^
�@�@ * @detail	.net�t�@�C���ɂ�����eNode�̒�`����
�@�@ */
	explicit Vertices(std::vector<Node>& nodeElements);

	/**
	 * @brief			�C�ӂ̐F��Vertices�I�u�W�F�N�g��Node�̐F���X�V
	 * @param[color]	�S�Ă�Node���X�V����F(ex. RGB(0,0,0) )
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
	std::string icolor; //�󕶎���ŏ�����
	std::string bcolor; //�󕶎���ŏ�����


public:
	/**
	 * @brief Node�N���X�̃R���X�g���N�^
	 * @param[nodeid]	Node�̈�ӂ�ID
	 * @param[label]		Node���ێ����郉�x��
	 */
	Node(const int nodeid, Label& label) :
		nodeid(nodeid),label(label),
		x(-1),y(-1){}
	
	/**
	 * @brief		�eNode�̍��W���Z�b�g
	 * @param[x]	�eNode��x���W
	 * @param[y]	�eNode��y���W
	 */
	Node& setPosition(
		const float x, 
		const float y
	);

	/**
	 * @brief			�eNode�̓����̐F���Z�b�g
	 * @param[icolor]	�eNode���X�V��������̐F
	 */
	Node& setInnerColor(const std::string& icolor);

	/**
	 * @brief			�eNode�̋��E���̐F���Z�b�g
	 * @param[bcolor]	�eNode���X�V���鋫�E���̐F
	 */
	Node& setBorderColor(const std::string& bcolor);

	/**
	 * @brief	�o�͂Ɏg�p����t�H�[�}�b�g��������擾
	 * @return	.net�ł̏o�͂ɗ��p���镶����i���s�܂ށj
	 */
	const std::string getSingleLine();
};


/**
* @brief ��ӂ̕����񃉃x��
*/
class Label {
	std::string label;

public:

	/**
	 * @brief			Label�N���X�̃R���X�g���N�^
	 * @param[label]	���x���Ŏg�p���郉�x���p�̕�����
	 */
	explicit Label(const std::string label) :
		label(label) {}

	/**
	 * @brief	label��������擾
	 * @return	���x���ɗ��p���Ă��镶����('"'���܂܂Ȃ��j
	 */
	std::string getLabel();

	/**
	 * @brief�@�o�͂Ɏg�p����label������
	 * @return ���x���ɗ��p���Ă��镶����i'"'���܂ށj
	 */
	std::string getOutputLabel();

	/**
	 * @brief			label��������Z�b�g
	 * @param[label]	�X�V���������x��������
	 */
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

	/**
	 * @brief			LabelDouble�̃R���X�g���N�^
	 * @param[label]	���������邽�߂̃��x��������
	 * @param[sep]		�K�w���x���𕪊�����f���~�^
	 * @detail			�K�w���x���̍쐬
	 */
	LabelDouble(std::string label, const char* sep);

	/**
	 * @brief	��K�w�̃��x�����擾
	 * @return	��K�w�̃��x��������
	 */
	std::string getUpLabel();

	/**
	 * @brief	���K�w�̃��x�����擾
	 * @return	���K�w�̃��x��������
	 */
	std::string getLwLabel();

};



/**
* @brief ���ւ̂���2�̃m�[�h�̏W���������N���X
*/
class Edges
{
	std::vector<std::pair<int, int>>& mpair;
	std::set<int> mnodeset;

public:

	/**
	 * @brief Edges�N���X�̃R���X�g���N�^
	 * @detail .net�t�@�C���ɂ�����Node�̌q����������̈敔��
	 */
	explicit Edges(std::vector<std::pair<int, int>>& mpair);

	/**
	 * @brief �Y��Node�����W���[�����`�����Ă��邩�ǂ���
	 * @retval true Node�����W���[�����`�����Ă���ꍇ
	 * @retval false Node�����W���[�����`�����Ă��Ȃ��ꍇ
	 */
	bool isModule(int index);

};


