#ifndef UTILS_H_
#define UTILS_H_


#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <functional>
#include <unordered_map>

namespace fs = std::filesystem;

/**
* @brief �ėp�I�ɗ��p�ł���֐����W�߂��N���X
*/
class Utils
{
private:
    inline Utils() {};
public:
    /**
     * @brief       ������𕪊�����
     * @param[line] �����Ώۂ̕�����
     * @param[delimiter]  ��������ۂɗ��p����f���~�^
     * @return      ��������������̔z��
     */
    static std::vector<std::string> split(
        const std::string& line, 
        const char* delimiter
    );

    static std::vector<std::string> split(
        const std::string& line,
        const char* delimiter,
        const int maxSplitCount
    );

    /**
     * @brief �Œ蒷�z���split�Ŕz��𗘗p���邽�ߑ傫�߂̃t�@�C���ɗ��p
     * @param[line] �����Ώۂ̕�����
     * @param[delimiter] ��������ۂɗ��p����f���~�^
     * @param[arr] ��������i�[���邽�߂̔z��
     * @param[maxSplitCount] �ő啪����
     * 
     */
    static std::string* splitf(
        const std::string& line,
        const char* delimiter,
        std::string* arr,
        const int maxSplitCount
    );


    /**
     * @brief �f���~�^�ŋ�؂�ꂽ2�̃J��������Ȃ�e�L�X�g�t�@�C��
     *        �̍�����key�C�E����value�Ƃ���map�ɕϊ����ĕԋp����֐�
     *        �������A�����o�֐���func�𗘗p���邱�Ƃ�key,value�͎��R
     *        �ɕύX�\
     * @param[infile] ���̓t�@�C�����i�p�X�j
     * @param[del]    ���̓t�@�C�����i�p�X�j�Ɏg�p����Ă���f���~�^
     * @param[func]   key,value�����R�ɐݒ肵�ĕԋp���邽�߂̊֐�
     * @return        �쐬���ꂽmap
     */
    static std::unordered_map<std::string, std::string> keyValMakeFromFile(
        const fs::path& infile,
        const char* del,
        std::function<std::vector<std::string>&(std::vector<std::string>&)> func
    );

    /**
     * @brief   �t�@�C�������݂��邩�ǂ���
     * @note    �J���邩�ǂ������m�F�����
     */
    static bool isExistFile(const std::string& str) ;

};

#endif // !UTILS_H_