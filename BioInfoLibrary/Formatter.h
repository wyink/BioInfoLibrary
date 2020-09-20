#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <functional>


/**
 * @brief �e��t�H�[�}�b�g�̕ϊ��Ɋւ��N���X
 * @detail 
 */
class FaaToFasta
{
private:
	const std::string infile; /**< ���̓t�@�C���� */
	const std::string out; /**< �o�̓t�@�C���� */
	std::regex re; /**< id�ɗ��p���镶��������肷�邽�߂̐��K�\�� */
	std::function<bool(std::string)> filter ; /** �����id�̍s�݂̂�I�����邽�߂̊֐� */

public:
	/**
	 * @brief �����s��faa��Fasta�t�@�C���ɕϊ�
	 * @detail ���̃R���X�g���N�^�͈ȉ��̊�����݂̂��ɂȂ��D
	 *         �K�v�ɉ����ă����o�[��ύX���邱�Ƃňȉ��̏�����ύX�ł���
	 * 
	 * �@�@�@�@Example.
	 *          >ABC1234.1 ...some redundant infomation
	 *         
	 *         //action
	 *         �E>ABC1234.1��ID�ɗ��p����D
	 *         �EID�ȍ~�̏����t�B���^�[�Ƃ��ė��p���Ȃ��D
	 */
	FaaToFasta(std::string infile, std::string out);

	/**
	 * @brief id�ɗ��p���镶��������肷�邽�߂̐��K�\����ύX
	 * @return ���g�̎Q�Ƃ�Ԃ�
	 */
	FaaToFasta& setIdFilter(std::regex re); 

	/**
	 * @brief  �����id�̍s�݂̂Ƀt�B���^�����O���邽�߂̊֐�
	 * @return ���g�̎Q�Ƃ�Ԃ�
	 * @detail ���̊֐���set���邱�ƂŕK�v��ID�ƕs�K�v��ID��I�ʂ���
	 *         ���Ƃ��ł���B�ݒ�i�V�̏ꍇ�͂��ׂĂ�ID���o�͂���D
	 */
	FaaToFasta& setIdFilterCall(std::function<bool(std::string)> fileter);

	/**
	 * @brief faa�t�@�C���`����fasta�t�@�C���`���ɕύX����D
	 */
	void run();

};

