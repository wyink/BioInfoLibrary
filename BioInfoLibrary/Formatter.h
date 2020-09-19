#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <functional>


/**
 * @brief �e��t�H�[�}�b�g�̕ϊ��Ɋւ��N���X
 */
class Formatter
{
	
public:
	/**
	 * @brief �����s��faa��Fasta�t�@�C���ɕϊ�
	 * @param[faafile] ���̓t�@�C���i�g���qfaa�j
	 * @param[out]     �o�̓t�@�C���i�g���qfasta�j
	 * @param[re]      id�̑I�ʂɗ��p���鐳�K�\��
	 */
	static void FaaToFasta(
		std::string faafile,
		std::string out,
		std::regex re,
		std::function<bool(std::string)> filter
	);

	/**
	* @brief  �����s��Fasta�t�@�C���ɕϊ�
	* @detail ���̊֐��̓I�[�o�[���[�h
	*         ��3��������ʓI�Ȃ��̂ɌŒ肵�Ă���
	*         id�̃t�B���^�����O�͍s��Ȃ��D
	*/
	static void FaaToFasta(
		std::string faafile,
		std::string out
	);
};

