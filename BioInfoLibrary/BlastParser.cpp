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

const std::string BlastParserPt1Imple::valueFormatter(const std::string& bquery, const std::vector<std::vector<std::string> >& queryToRefVec) {
	std::unordered_map <std::string, int> refcounter; /**< key�͎Q�Ƃ�ϊ����id val�͂��̑��ݐ� */
	std::string refid; /**< �Q�Ƃ�ϊ���Ɏg�p����id*/


	for (auto ref : queryToRefVec) {

		refid = convert(ref[1]);
		++refcounter[refid];

	}

	return outformat(bquery, refcounter);
}

const std::string BlastParserPt1Imple::outformat(const std::string& bquery, const std::unordered_map<std::string,int>& refcounter){
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
	
	bool align_sec_flag = false;
	float score   = 0.0f ;
	int startBase = 0 ;
	int endBase   = 0 ;
	std::string bref;
	std::pair<int,int> range;
	std::vector<std::string> blastLine;
	std::unordered_map<std::string, float> scoreMap;
	std::vector<std::vector<std::string> > requireRescore;

    //�ŏ��̗v�f�̂�
	blastLine = queryToRefVec[0];
	startBase = std::stoi(blastLine[7]);
	endBase   = std::stoi(blastLine[8]);
	score     = std::stof(blastLine[10]);

	//�N�G���ƎQ�Ƃ��t�����ɃA���C�����g�����ꍇ
	if (endBase < startBase) {
		std::swap(startBase, endBase);
	}

	scoreMap[blastLine[1]] = score;
	bref = blastLine[2];
	range = std::make_pair(startBase, endBase);
	
	for (auto iter = (queryToRefVec.begin())+1;iter != queryToRefVec.end();++iter){
		blastLine = *iter;

		startBase = std::stoi(blastLine[7]);
		endBase   = std::stoi(blastLine[8]);
		score     = std::stof(blastLine[10]);

		if (endBase < startBase) {
			std::swap(startBase, endBase);
		}

		//�����Q�Ƃɕ�����q�b�g�����ꍇ
		if (blastLine[1] == bref) {
			//�A���C�����g���d�����Ȃ��ꍇ(�X�R�A�̉��Z�j
			if (range.second < startBase || range.first > endBase) {

				//�A���C�����g�̈��ǉ��i�����q�b�g�p�j
				if (align_sec_flag) {
					scoreMap[bref] = 0;
					requireRescore.emplace_back(blastLine);
				}
				else {
					scoreMap[bref] += score;
					align_sec_flag = true;
				}
					
			}
			else{
				//�A���C�����g�������O�̍s�Əd�����邽�߃X�L�b�v
				continue;
			}

		}
		else { //�O�̍s�ƈقȂ�Q�ƂɃq�b�g�����ꍇ
			scoreMap[blastLine[1]] = score ;
			align_sec_flag = false;
		}

		range = std::make_pair(startBase, endBase);
		bref = blastLine[1];
	}

    //�d�����Ȃ��A���C�����g�ӏ���3�����ȏ゠��\��������ꍇ
	startBase = 0;
	endBase = 0;
	score = 0;
	std::string reference;
	if (requireRescore.size() > 0) {
		std::vector<int> hitBaseLoc;

		//firstLblastLine;
		blastLine = requireRescore[0];
		reference = blastLine[1];
		startBase = std::stoi(blastLine[7]);
		endBase = std::stoi(blastLine[8]);
		score = std::stof(blastLine[10]);

		scoreMap[reference] = score ;
		for (int i = startBase; i <= endBase; ++i) {
			hitBaseLoc.push_back(i);
		}
		std::string brf = blastLine[1];

		for (auto iter = requireRescore.begin() + 1; iter != requireRescore.end();++iter){
			blastLine = (*iter);

			reference = blastLine[1];
			startBase = std::stoi(blastLine[7]);
			endBase   = std::stoi(blastLine[8]);
			score    = std::stof(blastLine[10]);	

			//�N�G���ƎQ�Ƃ��t�����ɃA���C�����g�����ꍇ.
			if (endBase < startBase) {
				std::swap(startBase, endBase);
			}


			if (reference != brf) {
				//���Y�s�̏���
				hitBaseLoc.clear();
				scoreMap[reference] = score;
				for (int i = startBase; i <= endBase; ++i) {
					hitBaseLoc.push_back(i);
				}
			}
			else {
				//�A���C�����g�̈悪�d������ꍇ
				std::vector<int>tmpVec;
				for (int i = startBase; i <= endBase;++i) {
					tmpVec.push_back(i);
				}

				bool flag = false;
				for (const auto& i : hitBaseLoc) {
					if (std::find(tmpVec.begin(), tmpVec.end(), i) != tmpVec.end()) {
						flag = true;
						break;
					}
				}

				//�A���C�����g�̈悪�d�����Ȃ��ꍇ ����histBaseLoc�ɓo�^
				if (!flag) {
					for (int i = startBase; i <= endBase; ++i) {
						hitBaseLoc.push_back(i);
					}
					scoreMap[reference] += score;
				}
			}

			brf = reference;
		}
	}

    //outformat���ĕԋp
	return outformat(bquery, scoreMap);

}

const std::string BlastParserPt2Imple::outformat(const std::string& bquery, const std::unordered_map <std::string, float>& scoreMap) {
	const int histgramStep = 100; //�q�X�g�O�����ŕ\������ۂ̂P�u���b�N�̍��ݕ�
	/**
	* score  100 200 300 400 500 ... n*100
	* query1   2  20   3  40   2 ...  0 //���̊֐��ŕԋp����s�̕�����E��
	*/

	//�q�X�g�O�����ŕ`�悷��ۂ̍ő�l���m��
	auto max_idx = std::max_element(
		std::begin(scoreMap),
		std::end(scoreMap),
		[](const std::pair<std::string, float>& p1, const std::pair<std::string, float >& p2) {
			return p1.second < p2.second;
		}
	);

	std::vector<int> RangeCount; /**< index =�q�X�g�O�����̕�,val = �J�E���g */
	for (int i = 0; i <= ((max_idx->second) / histgramStep); ++i) {
		RangeCount[i] = 0;
	}

	int idx = 0;
	for (auto iter = scoreMap.begin(); iter != scoreMap.end();++iter) {
		idx = int(iter->second/histgramStep);
		++RangeCount[idx];
	}

	//�q�X�g�O�����ŕ`�悷��ۂ̎���������(idx���������j���̂��珑���o��
	std::string returnLine = bquery;
	for (auto iter = RangeCount.begin(); iter != RangeCount.end(); ++iter) {
		returnLine += "\t" + std::to_string(*iter);

	}

	return (returnLine + "\n");

}


