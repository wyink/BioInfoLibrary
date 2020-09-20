#pragma once
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <functional>

class BlastParser
{
private:
	std::string infile;
	std::map<std::string, std::string> queRef;
	std::function<void(std::string&)> convert;
	std::function<void(std::string&, std::string&)> valueFilter;

public:
	explicit BlastParser(std::string infile);

	void setRefConvertMap(const std::map<std::string, std::string> queRef);
	
	/**
	* @brief  �q�b�g�����Q��ID��ϊ����ĕԋp
	* @detail �Q��ID��Ή�����taxid,strain�ɕϊ�
	*/
	void setRefConvertCall(std::function<void(std::string&)> convert);

	/**
	* @brief blast���ʃt�@�C���̉�͊J�n
	*/
	void run();
};

