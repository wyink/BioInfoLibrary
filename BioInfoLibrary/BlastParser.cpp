#include "BlastParser.h"
#include "Utils.h"

BlastParser::BlastParser(std::string infile) :
	infile(infile),
	convert(NULL),
	valueFormatter(
		[this](std::vector<std::string>& mapvalue)->std::map<std::string, int> {
			//�ꎞ�Imap
			std::map<std::string, int> counterMap;

			for (size_t i = 0,max = mapvalue.size(); i < max ; ++i) {

				this->convert(mapvalue[i]);

				if (counterMap.count(mapvalue[i]) == 1) {
					//�o�^�ς�
					counterMap[mapvalue[i]]++;
				}
				else {
					//���o�^
					counterMap.insert(std::make_pair(mapvalue[i], 1));
				}
			}

			return counterMap;
		}
	),
	outformat(
		[](std::string bquery, std::map<std::string, int> refC) {
			std::stringstream outtext;
			outtext << bquery <<"\t"<< refC.size()<<"\n" ;

			for (const auto& [ref, counter] : refC) {
				outtext << ref << ":" << counter << ",";
			}

			std::string ret = outtext.str();
			ret.pop_back();
			ret += "\n";
			return ret;
		}
	)
{}

void BlastParser::setRefConvertMap(const std::map<std::string, std::string>& queRef) {
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

void BlastParser::run(const std::string outfile) {

	std::ifstream in{ infile };
	if (!in.is_open()) {
		std::cout << "Can't open the file!" << std::endl;
		return;
	}

	std::string line;
	std::vector<std::string> vec;
	std::vector<std::string> mapvalue;
	std::map<std::string, int> refC;
	std::string query;
	std::string bquery;
	std::string reference;
	std::string breference;
	std::string outtext;

	std::map<std::string, std::string> tmpref;

	
	//��s�ڂōs������
	std::getline(in, line);
	vec = Utils::split(line, "\t",2);
	query = bquery = vec[0];
	reference = reference = vec[1];
	mapvalue.emplace_back(reference);
	
	
	//2�s�ڈȍ~�ōs������
	std::ofstream out(outfile);
	while (std::getline(in, line)) {
		vec = Utils::split(line, "\t", 2);
		query = vec[0];
		reference = vec[1];
		

		if (query == bquery) {
			mapvalue.emplace_back(reference);
		}
		else {
			//map������o����printout
			refC = valueFormatter(mapvalue);
			out << outformat(bquery, refC);
			refC.clear();
			mapvalue.clear();

			bquery = query;


			//���݂̍s�̏���
			mapvalue.emplace_back(reference);
		}

		/*
		if (query == bquery) {
			if (breference == reference) {
				//�����z��̈قȂ�ꏊ�Ƀq�b�g�����ꍇ

			}else {
				mapvalue.emplace_back(breference);
				breference = reference;
			}
		}
		else {

			mapvalue.emplace_back(breference);

			refC = valueFormatter(mapvalue);
			out << outformat(bquery,refC);

			refC.clear();
			mapvalue.clear();
			bquery = query;
		}
		*/

	}

	//EOF�ōs������
	mapvalue.emplace_back(breference);
	refC = valueFormatter(mapvalue);
	out << outformat(bquery, refC);

	in.close();
	out.close();
}




