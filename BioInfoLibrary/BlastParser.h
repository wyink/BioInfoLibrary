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
	* @brief  ヒットした参照IDを変換して返却
	* @detail 参照IDを対応するtaxid,strainに変換
	*/
	void setRefConvertCall(std::function<void(std::string&)> convert);

	/**
	* @brief blast結果ファイルの解析開始
	*/
	void run();
};

