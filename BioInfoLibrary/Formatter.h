#ifndef FORMATTER_H_
#define FORMATTER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <functional>



namespace fs = std::filesystem;

/**
 * @brief �e��t�H�[�}�b�g�̕ϊ��Ɋւ��N���X
 */
class FaaToFasta
{
private:
	const fs::path infile; /**< ���̓t�@�C���� */
	const fs::path out; /**< �o�̓t�@�C���� */
	std::regex re; /**< id�ɗ��p���镶��������肷�邽�߂̐��K�\�� */
	std::function<bool(std::string)> filter ; /** �����id�̍s�݂̂�I�����邽�߂̊֐� */

public:
	/**
	 * @brief �����s��faa��Fasta�t�@�C���ɕϊ�
	 * @details ���̃R���X�g���N�^�͈ȉ��̊�����݂̂��ɂȂ��D
	 *         �K�v�ɉ����ă����o�[��ύX���邱�Ƃňȉ��̏�����ύX�ł���
	 * 
	 * �@�@�@�@Example.
	 *          >ABC1234.1 ...some redundant infomation
	 *         
	 *         //action
	 *         �E>ABC1234.1��ID�ɗ��p����D
	 *         �EID�ȍ~�̏����t�B���^�[�Ƃ��ė��p���Ȃ��D
	 */
	FaaToFasta(const fs::path& infile, const fs::path& out);

	/**
	 * @brief id�ɗ��p���镶��������肷�邽�߂̐��K�\����ύX
	 * @return ���g�̎Q�Ƃ�Ԃ�
	 */
	FaaToFasta& setIdFilter(std::regex re); 

	/**
	 * @brief  �����id�̍s�݂̂Ƀt�B���^�����O���邽�߂̊֐�
	 * @return ���g�̎Q�Ƃ�Ԃ�
	 * @details ���̊֐���set���邱�ƂŕK�v��ID�ƕs�K�v��ID��I�ʂ���
	 *         ���Ƃ��ł���B�ݒ�i�V�̏ꍇ�͂��ׂĂ�ID���o�͂���D
	 */
	FaaToFasta& setIdFilterCall(std::function<bool(std::string)> fileter);

	/**
	 * @brief faa�t�@�C���`����fasta�t�@�C���`���ɕύX����D
	 */
	void run();

};

#endif // !FORMATTER_H_