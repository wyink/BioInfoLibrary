#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <functional>
#include <memory>
#include <regex>
#include <set>
#include <map>

namespace fs = std::filesystem;

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
    const fs::path infile;
    std::unique_ptr<LabelInterface> ilabelptr;

public:
    /**
     * @brief            PajekParser�I�u�W�F�N�g�̃R���X�g���N�^
     * @param[infile]    �ǂݍ��ރt�@�C���i�g���q.net)
     * @detail           �ǂݍ��݂���.net�t�@�C���̃p�X���Z�b�g
     */
    PajekParser(
        const fs::path infile,
        std::unique_ptr<LabelInterface> ilabelptr
    );

    /**
     * @brief    .net�t�@�C�������[�h
     * @return    ���̓t�@�C���̏���ێ�����Pajek�I�u�W�F�N�g
     */
    Pajek load(); 
    
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
    virtual std::string getLabel() = 0;

    /**
     * @brief�@  �o�͂Ɏg�p����label������
     * @return   ���x���ɗ��p���Ă��镶����i'"'���܂ށj
     */
    virtual std::string getOutputLabel() = 0;

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



/**
* @brief Vertices�̈�s�����\������Node�Ɋւ���N���X
* @note �eNode��Pajek�I�u�W�F�N�g�̕����I�u�W�F�N�g��
*       �\������D�eNode��Pajek�I�u�W�F�N�g�Ń|�C���^
*       �Ƃ��ĕێ�����C�܂�������Pajek�I�u�W�F�N�g��
*       ���L�����D���������ă����o�ϐ���ilabel��
*       shared_ptr�ŊǗ����Ă���D
*/
class Node
{
private:
    int nodeid;
    std::shared_ptr<LabelInterface> ilabel;
    float x;
    float y;
    std::string icolor; //�󕶎���ŏ�����
    std::string bcolor; //�󕶎���ŏ�����

public:
    /**
     * @brief            Node�N���X�̃R���X�g���N�^
     * @param[nodeid]    Node�̈�ӂ�ID
     * @param[ilabel]     Node���ێ����郉�x��
     */
    Node(const int nodeid, std::shared_ptr<LabelInterface> ilabel) :
        nodeid(nodeid), ilabel(ilabel),
        x(-1), y(-1) {}
            

    inline const std::shared_ptr<LabelInterface> getLabelptr() const {
        return ilabel;
    }


    /**
     * @brief       �eNode�̍��W���Z�b�g
     * @param[x]    �eNode��x���W
     * @param[y]    �eNode��y���W
     */
    Node* setPosition(
        const float x,
        const float y
    );

    /**
     * @brief            �eNode�̓����̐F���Z�b�g
     * @param[icolor]    �eNode���X�V��������̐F
     */
    Node* setInnerColor(const std::string& icolor);

    /**
     * @brief            �eNode�̋��E���̐F���Z�b�g
     * @param[bcolor]    �eNode���X�V���鋫�E���̐F
     */
    Node* setBorderColor(const std::string& bcolor);

    /**
     * @brief    �o�͂Ɏg�p����t�H�[�}�b�g��������擾
     * @return    .net�ł̏o�͂ɗ��p���镶����i���s�܂ށj
     */
    const std::string getSingleLine();
    const std::string getSingleLine() const;
};



/**
* @brief Pajek��.net�t�@�C�����\������eNode�̒�`
*/
class Vertices
{
    std::shared_ptr<std::vector<std::shared_ptr<Node> > > nodeElements;
    Vertices(const Vertices& vt) = delete;

public:

    /**
�@�@ * @brief     Vertices�N���X�̃R���X�g���N�^
�@�@ * @detail    .net�t�@�C���ɂ�����eNode�̒�`����
�@�@ */
    explicit Vertices(std::shared_ptr<std::vector<std::shared_ptr<Node> > > nodeElements);


    /**
     * @brief �����I�u�W�F�N�g��nodeElements�̗v�f����Ԃ��D
     */
    inline const size_t size() const {
        return nodeElements->size();
    }

    /**
    * @detail �����I�u�W�F�N�gnodeElemens�̎Q�Ƃ�Ԃ��D
    *         �������A�ύX�s�Ƃ���D�ύX�͂��̃I�u�W�F�N�g��
    *         �����o�֐��ōs�����ƂƂ���D
    */
    inline const std::shared_ptr<std::vector<std::shared_ptr<Node> > > getNodeElements() const {
        return nodeElements;
    }


    /**
     * @brief           �C�ӂ̐F��Vertices�I�u�W�F�N�g��Node�̐F���X�V
     * @param[color]    �S�Ă�Node���X�V����F(ex. RGB(0,0,0) )
     */
    void setColor(const std::string& color);
};

class LabelSingle :public LabelInterface {
private:
    std::string label;
    LabelSingle(const LabelSingle& ld) {}//�R�s�[�R���X�g���N�^
    LabelSingle operator=(const LabelSingle& ld) {}

public:
    /**
     * @brief           Label�N���X�̃R���X�g���N�^
     * @param[label]    ���x���Ŏg�p���郉�x���p�̕�����
     */
    explicit inline LabelSingle (const std::string label) :
        label(label) {}
    ~LabelSingle() {}
    std::string getLabel() override;
    std::string getOutputLabel() override;
    void setLabel(const std::string label) override;
    inline LabelSingle* clone(const std::string label) override {
        return new LabelSingle(label);
    }
};

/**
 * @brief "project/sample"�Ȃǂ̊K�w���x���������N���X
 */
class LabelDouble:public LabelInterface {
private:
    std::string label;
    std::string upLabel;
    std::string lwLabel;
    LabelDouble(const LabelDouble& ld) {}//�R�s�[�R���X�g���N�^
    LabelDouble& operator=(const LabelDouble& ld) {}

public:

    /**
     * @brief             LabelDouble�̃R���X�g���N�^
     * @param[label]      ���������邽�߂̃��x��������
     * @detail            �K�w���x���̍쐬
     */
    explicit LabelDouble(std::string label);
    ~LabelDouble() {}
    std::string getLabel() override;

    /**
     * @brief     ��K�w�̃��x�����擾
     * @return    ��K�w�̃��x��������
     */
    std::string getUpLabel();

    /**
     * @brief     ���K�w�̃��x�����擾
     * @return    ���K�w�̃��x��������
     */
    std::string getLwLabel();

    std::string getOutputLabel() override;
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
    const std::vector<std::pair<int, int>> mpair;
    std::set<int> mnodeset;

public:

    /**
     * @brief     Edges�N���X�̃R���X�g���N�^
     * @detail    .net�t�@�C���ɂ�����Node�̌q����������̈敔��
     */
    explicit Edges(const std::vector<std::pair<int, int>> mpair);

    /**
     * @brief  �Y��Node�����W���[�����`�����Ă��邩�ǂ���
     * @retval true Node�����W���[�����`�����Ă���ꍇ
     * @retval false Node�����W���[�����`�����Ă��Ȃ��ꍇ
     */
    bool isModule(int index);

    /**
     * @brief �����o�ϐ�mpair��ԋp
     */
    inline const std::vector<std::pair<int, int>>& getMpair() const {
        return mpair;
    }

    std::string getOutput()const ;

    /**
     * @brief copy constructor;
     */
};



/**
 * @detail �g���q.net�ȊO�̃t�@�C������Pajek�t�@�C���i.net)
 *         ���쐬����N���X
 * @note   ���̃I�u�W�F�N�g���쐬����ۂ̑������͓��I�m�ۂ���
 *         pointer��n�����ƁB
 */
class CreateFromText {
private:
    const fs::path infile; //.net�ȊO�̓��̓t�@�C��
    std::unique_ptr<LabelInterface> m_ilabel; //Label������̂ӂ�܂����K��
    std::function<std::shared_ptr<Node>(std::shared_ptr<Node>)> addproperty; //Node�N���X�̃J�X�^�}�C�Y

    /**
     * @brief �R�s�[�R���X�g���N�^
     *        �����o�ϐ��Ƀ|�C���^���܂܂�Ă���A�܂��A������
     *        �Ӗ����Ȃ����ߋ֎~
     */
    CreateFromText(const CreateFromText& cft) = delete;

public:
    /**
     * @brief �R���X�g���N�^
     * @param[infile] ���̓t�@�C�����i�p�X�j
     * @param[m_ilabel] Label������I�u�W�F�N�g�i�K�����I�m�ۂ������\�[�X���󂯎��j
     * @param[addproperty] Node�I�u�W�F�N�g�ɒǉ�����v���p�e�B
     *                     �ǉ����Ȃ��ꍇ�͂��̂܂܂�return�ŕԂ��D
     */
    explicit inline CreateFromText(
        const fs::path infile,
        std::unique_ptr<LabelInterface> m_ilabel,
        std::function<std::shared_ptr<Node>(std::shared_ptr<Node>)> addproperty
    ):infile(infile),m_ilabel(std::move(m_ilabel)),addproperty(addproperty) {};


    //informat
    //Node�̂ӂ�܂��iilabel)
    //outformat��Pajek�N���X�I�u�W�F�N�g�ɂ���
    const std::vector<std::unique_ptr<Pajek> > run();
};

/**
 * @brief    Pajek�t�@�C���̗v�f(Vertices,Edges)��ێ�����I�u�W�F�N�g
 * @note     �����I�u�W�F�N�g�ɓ��I�m�ۂ��������������݂���̂ŃR�s�[�R���X�g���N�^
 *           �𗘗p���ĐV���Ƀ������𓮓I�Ɋm�ۂ���K�v������B�]����
 *           �R���X�g���N�^�ł͒l�n���Ŏ󂯎��D//�����I�ɃR�s�[�R���X�g���N�^��
 *           �Ăяo���Ď������m�ۂ���D�iVertices�I�u�W�F�N�g�𕡐����ė��p���邽�߁j
 *           
 */
class Pajek
{
    const std::string pajekLabel; /**< ���̃I�u�W�F�N�g�̎���id ex.����0.980�̎���.net�Ȃ�*/
    std::shared_ptr<Vertices> vtptr;
    std::unique_ptr<Edges> egsptr;

public:
    /**
     * @brief         Pajek�I�u�W�F�N�g�̃R���X�g���N�^
     * @param[vt]     �SNode�̒�`�̏W��
     * @param[egs]    ���ւ̂���2��Node�̏W��
     */
    inline Pajek(
        std::string pajekLabel,
        std::shared_ptr<Vertices> vtptr,
        std::unique_ptr<Edges> egsptr
    ): pajekLabel(pajekLabel),vtptr(vtptr), egsptr(std::move(egsptr)){}

    inline std::string getPajekLbel() const {
        return pajekLabel;
    }

    /**
     *  @brief Pajek�I�u�W�F�N�g��net�t�@�C���ɏ����o��
     *  @param[outfile] �o�̓t�@�C����
     */
    void output(fs::path outfile) const ;


};
