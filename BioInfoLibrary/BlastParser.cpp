#include "BlastParser.h"
#include "Utils.h"

BlastParser::BlastParser(const std::string& infile, BlastParserHandler& bph) :
	infile(infile), bph(bph){}


void BlastParser::run(const std::string& outfile) {

	std::ifstream in{ infile };
	if (!in.is_open()) {
		std::cout << "Can't open the file!" << std::endl;
		return;
	}

	std::string line;
	std::vector<std::string> vec;
	std::set<std::vector<std::string> > queryToRefSet;
	std::string query;
	std::string bquery;
	
	//��s�ڂōs������
	std::getline(in, line);
	vec = Utils::split(line, "\t", 2);
	query = bquery = vec[0];
	queryToRefSet.insert(vec);
	
	//2�s�ڈȍ~�ōs������
	std::ofstream out(outfile);
	while (std::getline(in, line)) {
		vec = Utils::split(line, "\t");
		query = vec[0];

		if (query == bquery) {
			//query������̂��̂͂��ׂĉ��
			//blast�ŎQ�Ƒ������񂵂ďo�͂����ۏ؂��Ȃ�����

			queryToRefSet.insert(vec);

		}
		else {//(A,a)->(B,b)
			//����܂ł̍s�œǂݍ��񂾃N�G���[ (A) �̏����o��
			out << bph.valueFormatter(bquery, queryToRefSet);
			queryToRefSet.clear();

			bquery = query;//query��(b)�ɍēo�^


			//���݂̍s(b)�̏���
			queryToRefSet.insert(vec);
		}

	}

	//EOF�ōs������
	out << bph.valueFormatter(bquery, queryToRefSet);

	in.close();
	out.close();

}




