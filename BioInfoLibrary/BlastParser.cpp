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

	//��s�ڂōs������
	std::getline(in, line);
	vec = Utils::split(line, "\t");
	query = bquery = vec[0];
	reference = breference = vec[1];
	
	//2�s�ڈȍ~�ōs������
	while (std::getline(in, line)) {
		vec = Utils::split(line, "\t");
		query = vec[0];
		reference = vec[1];

		if (query == bquery) {
			if (breference == reference) {
				//�����z��̈قȂ�ꏊ�Ƀq�b�g�����ꍇ

			}else {
				mapvalue.push_back(breference);
				breference = reference;
			}
		}
		else {

			mapvalue.push_back(breference);
			//mapvalue��forloop��convert
			/**map.insert(std::make_pair(query, mapvalue));*/
			//valueFilter���s�����Ƃ�kaiketu dekiru
			//printout(bquery,mapvalue);
			mapvalue.clear();

			bquery = query;
		}

	}

	//EOF�ōs������
	mapvalue.push_back(breference);
	map.insert(std::make_pair(query, mapvalue));
	mapvalue.clear();

	in.close();
}