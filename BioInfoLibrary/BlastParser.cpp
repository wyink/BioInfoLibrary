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


const std::unordered_map<std::string, float>& BlastParserPt2Imple::refAlignDup3More(
	std::unordered_map<std::string, float>& scoreMap, std::vector<std::vector<std::string> > requireRescore){
		

		//�ŏ��̗v�f�i�s�j�̏���
		std::vector<std::string> blastLine = requireRescore[0];
		std::string brf                    = blastLine[REFERENCE_ID];
		std::string reference              = blastLine[REFERENCE_ID]; 
		int startBase                      = std::stoi(blastLine[REF_START]);
		int endBase                        = std::stoi(blastLine[REF_END]);
		float score                        = std::stof(blastLine[SCORE]);

		std::vector<int> hitBaseLoc; /**< �A���C�����g�����Q�Ƒ��̔z��̏ꏊ */
		scoreMap[reference] = score;

		//�N�G���ƎQ�Ƃ��t�����ɃA���C�����g�����ꍇ.
		if (endBase < startBase) {
			std::swap(startBase, endBase);
		}

		for (int i = startBase; i <= endBase; ++i) {
			hitBaseLoc.push_back(i);
		}

		//�Q�߂̗v�f�i�Q�s�ځj�ȍ~�̏���
		for (auto iter = requireRescore.begin() + 1; iter != requireRescore.end(); ++iter) {

			blastLine = (*iter);
			reference = blastLine[REFERENCE_ID];
			startBase = std::stoi(blastLine[REF_START]);
			endBase   = std::stoi(blastLine[REF_END]);
			score     = std::stof(blastLine[SCORE]);

			//�N�G���ƎQ�Ƃ��t�����ɃA���C�����g�����ꍇ.
			if (endBase < startBase) {
				std::swap(startBase, endBase);
			}

			
			if (reference != brf) {
				hitBaseLoc.clear(); //�O�s�̎Q�Ƃ̃A���C�����g�ӏ��͔j��

				scoreMap[reference] = score;

				//�Q�ƃA���C�����g�ӏ��̍ēo�^�i���̍s�j
				for (int i = startBase; i <= endBase; ++i) {
					hitBaseLoc.push_back(i);
				}

				brf = reference;

			}
			else {
				//����z��̕ʉӏ��ɃA���C�����g�����ꍇ.

				std::unordered_map<int,bool> tmpMap; /**< ���̍s�̎Q�ƃA���C�����g�ӏ��ێ� */

				for (int i = startBase; i <= endBase; ++i) {
					tmpMap[i] = true;
				}

				bool flag = false; /**< �Q�Ƃ̃A���C�����g�ӏ����d���������ǂ��� */
				for (const auto& i : hitBaseLoc) {
					if(tmpMap.count(i)== 1){
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

		}

	return scoreMap;
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
	startBase = std::stoi(blastLine[QUERY_START]);
	endBase   = std::stoi(blastLine[QUERY_END]);
	score     = std::stof(blastLine[SCORE]);

	//�N�G���ƎQ�Ƃ��t�����ɃA���C�����g�����ꍇ
	if (endBase < startBase) {
		std::swap(startBase, endBase);
	}

	scoreMap[blastLine[SCORE]] = score;
	bref = blastLine[REFERENCE_ID];
	range = std::make_pair(startBase, endBase);
	
	for (auto iter = (queryToRefVec.begin())+1;iter != queryToRefVec.end();++iter){
		blastLine = *iter;

		startBase = std::stoi(blastLine[REF_START]);
		endBase   = std::stoi(blastLine[REF_END]);
		score     = std::stof(blastLine[SCORE]);

		if (endBase < startBase) {
			std::swap(startBase, endBase);
		}

		//�����Q�Ƃɕ�����q�b�g�����ꍇ
		if (blastLine[REFERENCE_ID] == bref) {
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
			scoreMap[blastLine[SCORE]] = score ;
			align_sec_flag = false;
		}

		range = std::make_pair(startBase, endBase);
		bref = blastLine[REFERENCE_ID];
	}

	if (requireRescore.size() > 0) {

		//�d�����Ȃ��A���C�����g�ӏ���3�����ȏ゠��\��������ꍇ
		scoreMap = refAlignDup3More(scoreMap,requireRescore); 

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


