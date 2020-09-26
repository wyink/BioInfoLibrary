#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <functional>
#include <regex>
#include <set>
#include <map>

/**
 * @brief     Pajek�A�v���P�[�V�����ŗ��p����t�@�C���̉��
 * 
 * @detail    Pajek�A�v���P�[�V�����ł͊g���q.net�̃t�@�C�����o�͂Ƃ��ė��p����D
 *            ���̃N���X�ł͂��̃t�@�C������͂��Ă��̓��e��ۊǂ��邽�߂�Pajek
 *            �I�u�W�F�N�g�ɕϊ�����D
 */
class Pajek;
class LabelInterface;
class PajekParser {
    const std::string infile;
    LabelInterface& ilabel;

public:
    /**
     * @brief            PajekParser�I�u�W�F�N�g�̃R���X�g���N�^
     * @param[infile]    �ǂݍ��ރt�@�C���i�g���q.net)
     * @detail           �ǂݍ��݂���.net�t�@�C���̃p�X���Z�b�g
     */
    PajekParser(
        const std::string infile,
        LabelInterface& ilabel
    );

    /**
     * @brief    .net�t�@�C�������[�h
     * @return    ���̓t�@�C���̏���ێ�����Pajek�I�u�W�F�N�g
     */
    Pajek load(); 
};

/**
 * @brief    Pajek�t�@�C���̗v�f(Vertices,Edges)��ێ�����I�u�W�F�N�g
 */
class Vertices;
class Edges;
class Pajek
{
    Vertices& vt;
    Edges& egs;

public:
    /**
     * @brief         Pajek�I�u�W�F�N�g�̃R���X�g���N�^
     * @param[vt]     �SNode�̒�`�̏W��
     * @param[egs]    ���ւ̂���2��Node�̏W��
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
�@�@ * @brief     Vertices�N���X�̃R���X�g���N�^
�@�@ * @detail    .net�t�@�C���ɂ�����eNode�̒�`����
�@�@ */
    explicit Vertices(std::vector<Node>& nodeElements);

    /**
     * @brief           �C�ӂ̐F��Vertices�I�u�W�F�N�g��Node�̐F���X�V
     * @param[color]    �S�Ă�Node���X�V����F(ex. RGB(0,0,0) )
     */
    void setColor(const std::string& color);
};


/**
* @brief 
*/
class LabelInterface;
class Node
{
    int nodeid;
    LabelInterface* label;
    float x;
    float y;
    std::string icolor; //�󕶎���ŏ�����
    std::string bcolor; //�󕶎���ŏ�����


public:
    /**
     * @brief            Node�N���X�̃R���X�g���N�^
     * @param[nodeid]    Node�̈�ӂ�ID
     * @param[label]     Node���ێ����郉�x��
     */
    Node(const int nodeid, LabelInterface* label) :
        nodeid(nodeid),label(label),
        x(-1),y(-1){}
    
    inline ~Node() {
        delete label;
    }

    /**
     * @brief       �eNode�̍��W���Z�b�g
     * @param[x]    �eNode��x���W
     * @param[y]    �eNode��y���W
     */
    Node& setPosition(
        const float x, 
        const float y
    );

    /**
     * @brief            �eNode�̓����̐F���Z�b�g
     * @param[icolor]    �eNode���X�V��������̐F
     */
    Node& setInnerColor(const std::string& icolor);

    /**
     * @brief            �eNode�̋��E���̐F���Z�b�g
     * @param[bcolor]    �eNode���X�V���鋫�E���̐F
     */
    Node& setBorderColor(const std::string& bcolor);

    /**
     * @brief    �o�͂Ɏg�p����t�H�[�}�b�g��������擾
     * @return    .net�ł̏o�͂ɗ��p���镶����i���s�܂ށj
     */
    const std::string getSingleLine();
};


/**
* @brief ��ӂ̕����񃉃x���������C���^�t�F�[�X
*/
class LabelInterface {
public:

    LabelInterface() = default;

    /**
     * @brief     label��������擾
     * @return    ���x���ɗ��p���Ă��镶����('"'���܂܂Ȃ��j
     */
    virtual const std::string getLabel() = 0;

    /**
     * @brief�@  �o�͂Ɏg�p����label������
     * @return   ���x���ɗ��p���Ă��镶����i'"'���܂ށj
     */
    virtual const std::string getOutputLabel() = 0 ;

    /**
     * @brief           label��������Z�b�g
     * @param[label]    �X�V���������x��������
     */
    virtual void setLabel(const std::string label) = 0;

    virtual ~LabelInterface() = default;

    /**
     * @brief �I�u�W�F�N�g�̃R�s�[���쐬
     *        �������𓮓I�Ɋm�ۂ��Ă���D
     */
    virtual LabelInterface* clone(const std::string label) = 0;

};


class LabelSingle :public LabelInterface {
private:
    std::string label;

public:
    /**
     * @brief           Label�N���X�̃R���X�g���N�^
     * @param[label]    ���x���Ŏg�p���郉�x���p�̕�����
     */
    explicit inline LabelSingle (const std::string label) :
        label(label) {}

    const std::string getLabel() override;
    const std::string getOutputLabel() override;
    void setLabel(const std::string label) override;
    inline LabelSingle* clone(const std::string label) override {
        return new LabelSingle(label);
    }
};

/**
 * @brief "project/sample"�Ȃǂ̊K�w���x���������N���X
 */
class LabelDouble:public LabelInterface {
    std::string label;
    std::string upLabel;
    std::string lwLabel;

public:

    /**
     * @brief             LabelDouble�̃R���X�g���N�^
     * @param[label]      ���������邽�߂̃��x��������
     * @param[sep]        �K�w���x���𕪊�����f���~�^
     * @detail            �K�w���x���̍쐬
     */
    explicit inline LabelDouble(std::string label);

    const std::string getLabel() override;

    /**
     * @brief     ��K�w�̃��x�����擾
     * @return    ��K�w�̃��x��������
     */
    const std::string getUpLabel();

    /**
     * @brief     ���K�w�̃��x�����擾
     * @return    ���K�w�̃��x��������
     */
    const std::string getLwLabel();

    const std::string getOutputLabel() override;
    void setLabel(const std::string label) override;
    inline LabelDouble* clone(const std::string label) override {
        return new LabelDouble(label);
    }

};



/**
* @brief ���ւ̂���2�̃m�[�h�̏W���������N���X
*/
class Edges
{
    const std::vector<std::pair<int, int>>& mpair;
    std::set<int> mnodeset;

public:

    /**
     * @brief     Edges�N���X�̃R���X�g���N�^
     * @detail    .net�t�@�C���ɂ�����Node�̌q����������̈敔��
     */
    explicit Edges(const std::vector<std::pair<int, int>>& mpair);

    /**
     * @brief  �Y��Node�����W���[�����`�����Ă��邩�ǂ���
     * @retval true Node�����W���[�����`�����Ă���ꍇ
     * @retval false Node�����W���[�����`�����Ă��Ȃ��ꍇ
     */
    bool isModule(int index);

};

/**
 * @detail �g���q.net�ȊO�̃t�@�C������Pajek�t�@�C���i.net)
 *         ���쐬����N���X
 */
namespace fs = std::filesystem;
class CreateFromText {
private:
    const fs::path infile; //.net�ȊO�̓��̓t�@�C��
    LabelInterface* m_ilabel; //Label������̂ӂ�܂����K��
    std::function<Node&(Node&)> addproperty; //Node�N���X�̃J�X�^�}�C�Y

public:
    explicit inline CreateFromText(
        const fs::path infile,
        LabelInterface* ilabel,
        std::function<Node&(Node&)> addproperty
    ):infile(infile),m_ilabel(m_ilabel),addproperty(addproperty) {};

    //informat
    //Node�̂ӂ�܂��iilabel)
    //outformat��Pajek�N���X�I�u�W�F�N�g�ɂ���
    const std::vector<Pajek> run();
};


