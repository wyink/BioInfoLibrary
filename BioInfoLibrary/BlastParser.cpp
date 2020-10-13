#include "BlastParser.h"
#include "Utils.h"

BlastParser::BlastParser(const fs::path& infile, std::shared_ptr<BlastParserHandler> bph) :
	infile(infile), bph(std::move(bph)){}


void BlastParser::run(const fs::path& outfile, const std::string& header) {

	std::ifstream in{ infile };
	if (!in.is_open()) {
		std::cout << "Can't open the file!" << std::endl;
		return;
	}

	std::string line;
	std::vector<std::string> vec;
	std::vector<std::vector<std::string> > queryToRefVec;
	std::string query;
	std::string bquery;
	
	//��s�ڂōs������
	std::getline(in, line);
	vec = Utils::split(line, "\t");
	query = bquery = vec[0];
	queryToRefVec.emplace_back(vec);
	
	//2�s�ڈȍ~�ōs������
	std::ofstream out(outfile);
	out << header << "\n";
	while (std::getline(in, line)) {
		vec = Utils::split(line, "\t");
		query = vec[0];

		if (query == bquery) {
			//query������̂��̂͂��ׂĉ��
			//blast�ŎQ�Ƒ������񂵂ďo�͂����ۏ؂��Ȃ�����

			queryToRefVec.emplace_back(vec);

		}
		else {//(A,a)->(B,b)
			//����܂ł̍s�œǂݍ��񂾃N�G���[ (A) �̏����o��
			out << bph->valueFormatter(bquery, queryToRefVec);
			queryToRefVec.clear();

			bquery = query;//query��(b)�ɍēo�^

			//���݂̍s(b)�̏���
			queryToRefVec.emplace_back(vec);
		}

	}

	//EOF�ōs������
	out << bph->valueFormatter(bquery, queryToRefVec);

	in.close();
	out.close();

}

//map.count�̏�����O(n)
const std::string BlastParserPt1Imple::valueFormatter(const std::string& bquery, const std::vector<std::vector<std::string> >& queryToRefSet) {
	std::map<std::string, int> refcounter; /**< key�͎Q�Ƃ�ϊ����id val�͂��̑��ݐ� */
	std::string refid; /**< �Q�Ƃ�ϊ���Ɏg�p����id*/


	for (auto ref : queryToRefSet) {

		refid = convert(ref[1]);
		refcounter[refid]++;

	}

	return outformat(bquery, refcounter);
}

const std::string BlastParserPt1Imple::outformat(const std::string& bquery, const std::map<std::string, int>& refcounter){
	std::stringstream outtext;
	outtext << ">" << bquery << "\t" << refcounter.size() << "\n";

	for (const auto& [ref, counter] : refcounter) {
		outtext << ref << ":" << counter << ",";
	}

	std::string ret = outtext.str();
	ret.pop_back();
	ret += "\n";

	return ret;
}


const std::string BlastParserPt2Imple::valueFormatter(const std::string& bquery, const std::vector<std::vector<std::string> >& queryToRefVec) {
	
	float score   = 0 ;
	int startBase = 0 ;
	int endBase   = 0 ;
	int idx = 0;
	std::string bref;
	std::pair<int,int> range;
	std::vector<int> scoreVec;

    //�ŏ��̗v�f�̂�
	startBase = std::stoi(queryToRefVec[0][7]);
	endBase = std::stoi(queryToRefVec[0][8]);
	score = std::stoi(queryToRefVec[0][10]);

	//�N�G���ƎQ�Ƃ��t�����ɃA���C�����g�����ꍇ
	if (endBase < startBase) {
		std::swap(startBase, endBase);
	}

	bref = queryToRefVec[0][2];
	range = std::make_pair(startBase, endBase);
	scoreVec[idx] = score;
	++idx;
	
	//for (const auto& ref : queryToRefVec) {
	for (auto iter = (queryToRefVec.begin())+1;iter != queryToRefVec.end();++iter){
		auto ref = *iter;

		startBase = std::stoi(ref[7]);
		endBase = std::stoi(ref[8]);
		score = std::stoi(ref[10]);

		if (endBase < startBase) {
			std::swap(startBase, endBase);
		}


		//�����Q�Ƃɕ�����q�b�g�����ꍇ
		if (ref[2] == bref) {
			//�A���C�����g���d�����Ȃ��ꍇ(�X�R�A�̉��Z�j
			if (range.second < startBase || range.first > endBase) {
				scoreVec[idx] += score;
			}
			else{
				//�A���C�����g�������O�̍s�Əd�����邽�߃X�L�b�v
				continue;
			}

		}
		else { //�O�̍s�ƈقȂ�Q�ƂɃq�b�g�����ꍇ
			scoreVec[idx] = score ;
		}

		range = std::make_pair(startBase, endBase);
		bref = ref[2];
		++idx;
	}

}

const std::string BlastParserPt2Imple::outformat(const std::string& bquery, const std::map<std::string, int>& refcounter) {

}


