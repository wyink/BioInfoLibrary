#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

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
     * @param[sep]  ��������ۂɗ��p����f���~�^
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
     * @brief   �t�@�C�������݂��邩�ǂ���
     * @note    �J���邩�ǂ������m�F�����
     */
    static bool isExistFile(const std::string& str) ;

};

