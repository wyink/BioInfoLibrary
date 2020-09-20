#include "BlastParser.h"
#include "Utils.h"

BlastParser::BlastParser(std::string infile) :
	infile(infile),
	convert(NULL),
	valueFilter(
		[this](std::vector<std::string > vec )->void {
			for (size_t i = 0; i < vec.size(); ++i) {
				this->convert(vec[i]);
				
			}
		}
	){

}

void BlastParser::setRefConvertMap(std::map<std::string, std::string> queRef) {
	this->queRef = queRef;
	
	setRefConvertCall(
		[this](std::string& reference) -> void {
			reference = this->queRef[reference];
		}
	);
}

void BlastParser::setRefConvertCall(std::function<void(std::string&)> convert) {
	this->convert = convert;
}

void BlastParser::run() {

	std::ifstream in{ infile };
	if (!in.is_open()) {
		std::cout << "Can't open the file!" << std::endl;
		return;
	}
	
	std::string line;
	std::vector<std::string> vec;
	std::vector<std::string> mapvalue;
	std::map<std::string, std::vector<std::string> > map;
	std::string query;
	std::string bquery;
	std::string reference;
	std::string breference;

	//一行目で行う処理
	std::getline(in, line);
	vec = Utils::split(line, "\t");
	query = bquery = vec[0];
	reference = breference = vec[1];
	
	//2行目以降で行う処理
	while (std::getline(in, line)) {
		vec = Utils::split(line, "\t");
		query = vec[0];
		reference = vec[1];

		if (query == bquery) {
			if (breference == reference) {
				//同じ配列の異なる場所にヒットした場合

			}else {
				mapvalue.push_back(breference);
				breference = reference;
			}
		}
		else {

			mapvalue.push_back(breference);
			//mapvalueをforloopでconvert
			/**map.insert(std::make_pair(query, mapvalue));*/
			//valueFilterを行うことでkaiketu dekiru
			//printout(bquery,mapvalue);
			mapvalue.clear();

			bquery = query;
		}

	}

	//EOFで行う処理
	mapvalue.push_back(breference);
	map.insert(std::make_pair(query, mapvalue));
	mapvalue.clear();

	in.close();
}