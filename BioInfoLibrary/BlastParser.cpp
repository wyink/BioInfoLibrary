#include "BlastParser.h"
#include "Utils.h"


BlastParser::BlastParser(const fs::path& infile, std::shared_ptr<IBlastParser> bph) :
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
	std::unordered_map <std::string, float> refcounter; /**< key�͎Q�Ƃ�ϊ����id val�͂��̑��ݐ� */
	std::string refid; /**< �Q�Ƃ�ϊ���Ɏg�p����id*/


	for (auto ref : queryToRefVec) {

		refid = convert(ref[1]);
		++refcounter[refid];

	}

	return outformat(bquery, refcounter);
}

const std::string BlastParserPt1Imple::outformat(const std::string& bquery, const std::unordered_map<std::string,float>& refcounter){
	std::stringstream outtext;
	outtext << ">" << bquery << "\t" << refcounter.size() << "\n";

	for (auto iter = refcounter.begin(); iter != refcounter.end();++iter ) {
		outtext << iter->first << ":" << static_cast<int>(iter->second)<< ",";
	}

	std::string ret = outtext.str();
	ret.pop_back();
	ret += "\n";

	return ret;
}


const std::unordered_map<std::string, float>& BlastParserPt2Imple::refAlignDup3More(
	std::unordered_map<std::string, float>& scoreMap, std::vector<std::vector<std::string> >& requireRescore){
		

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
	
	std::unordered_map<std::string, float> scoreMap;

    //�ŏ��̗v�f�̂�
	std::vector<std::string> blastLine = queryToRefVec[0];
	std::string ref                    = blastLine[REFERENCE_ID];
	std::string bref                   = blastLine[REFERENCE_ID];
	int startBase                      = std::stoi(blastLine[REF_START]);
	int endBase                        = std::stoi(blastLine[REF_END]);
	float score                        = std::stof(blastLine[SCORE]);

	//�N�G���ƎQ�Ƃ��t�����ɃA���C�����g�����ꍇ
	if (endBase < startBase) {
		std::swap(startBase, endBase);
	}

	scoreMap[bref] = score;
	std::pair<int,int> range = std::make_pair(startBase, endBase);

	bool align_sec_flag = false;       /**< �Q�Ƃ̃A���C�����g�ӏ���2��������Ƃ��ɐ^ */
	bool align_sec_again_flag = false; /**< �Q�Ƃ̃A���C�����g�ӏ���3�����ȏ゠��Ƃ��ɐ^^(1) */
	std::vector<std::vector<std::string> > requireRescore; /**< �Q�Ƃ̖��[�ӏ���P����r�ł��Ȃ��Q��^(1)���܂�blastLine */
	int  skip_count = 0; /**< iter�𐳏�ɖ߂����߂̕ϐ��ŁA�Q�Ɣz�񂪏d�������ꍇ�ɐώZ */

	for (auto iter = (queryToRefVec.begin())+1;iter != queryToRefVec.end();++iter){
		blastLine = *iter;

		ref       = blastLine[REFERENCE_ID];
		startBase = std::stoi(blastLine[REF_START]);
		endBase   = std::stoi(blastLine[REF_END]);
		score     = std::stof(blastLine[SCORE]);

		if (endBase < startBase) {
			std::swap(startBase, endBase);
		}

		//�����Q�Ƃɕ�����q�b�g���邩�̔��f
		if (ref == bref) {
			//�O�̍s�ƃA���C�����g���d�����Ȃ��ꍇ�̓X�R�A�����Z
			if (range.second < startBase || range.first > endBase) {

				//�d�����Ȃ��A���C�����g�ӏ���3�����ȏ�̏ꍇ
				if (align_sec_flag) {
					//�d�����Ȃ��A���C�����g�ӏ���4�����ȏ�
					if (align_sec_again_flag) {
						requireRescore.emplace_back(blastLine);
					}//�d�����Ȃ��A���C�����g�ӏ���3������
					else {

						//3�����ȏ�A���C�����g�ӏ������݂���ꍇ�͂��̕��@�ł͐��m�ɔ��f�ł��Ȃ�
						//���߁A�ʂ̃A���S���Y���ōď������s���B�]���Ă��̎Q�Ƃɑ΂���X�R�A�͏���������B
						align_sec_again_flag = true;
						scoreMap[bref] = 0;

						//�ď������s�����߁A���Y�s�Ƃ��̑O��2�s���̗v�f���擾����
						//skip�����ꍇ�A���̕����߂��đ������
						int back_from_now = 2 + skip_count;
						iter -= back_from_now ;

						std::vector<std::string> tmpBlastLine;
						for (int i = 0; i < back_from_now; ++i) {
							tmpBlastLine = (*iter);
							requireRescore.emplace_back(tmpBlastLine);
							++iter;
						}

						//���݂̍s
						tmpBlastLine = (*iter);
						requireRescore.emplace_back(tmpBlastLine);

					}
				}//�d�����Ȃ��A���C�����g�ӏ������Y�Q�Ƃɑ΂���2������
				else {
					scoreMap[bref] += score;
					align_sec_flag = true;
				}
					
			}
			else{
				//�A���C�����g�������O�̍s�Əd�����邽�߃X�L�b�v
				++skip_count;
				continue;
			}

		}
		else { //�O�̍s�ƈقȂ�Q�ƂɃq�b�g�����ꍇ

			scoreMap[ref] = score ;
			range = std::make_pair(startBase, endBase);
			bref = blastLine[REFERENCE_ID];

			skip_count = 0;
			if (align_sec_flag) {
				align_sec_flag = false;
				align_sec_again_flag = false;
			}
		}

	}

	if (requireRescore.size() > 0) {

		//�d�����Ȃ��A���C�����g�ӏ���3�����ȏ゠��\��������ꍇ
		scoreMap = this->refAlignDup3More(scoreMap,requireRescore); 

	}

    //outformat���ĕԋp
	return this->outformat(bquery, scoreMap);

}


/**
* score  100 200 300 400 500 ... n*100
* query1   2  20   3  40   2 ...  0 //���̊֐��ŕԋp����s�̕�����E��
*/
const std::string BlastParserPt2Imple::outformat(const std::string& bquery, const std::unordered_map<std::string, float>& scoreMap) {
	const int histgramStep = 100; //�q�X�g�O�����ŕ\������ۂ̂P�u���b�N�̍��ݕ�

	//�q�X�g�O�����ŕ`�悷��ۂ̍ő�l���m��

	auto max_idx = std::max_element(
		scoreMap.begin(),
		scoreMap.end() ,
		[](const std::pair<std::string, float>& p1, const std::pair<std::string, float >& p2) {
			return p1.second < p2.second;
		}
	);

	//�q�X�g�O�����̊e�v�f�̏�����
	std::map<int,int> RangeCount; /**< index =�q�X�g�O�����̕�,val = �J�E���g */
	for (int i = 0; i <= ((max_idx->second) / histgramStep); ++i) {
		RangeCount[i] = 0;
	}

	int idx = 0;
	for (auto iter = scoreMap.begin(); iter != scoreMap.end(); ++iter) {
		idx = static_cast<int>(iter->second) / histgramStep;
		++RangeCount[idx];
	}

	//�q�X�g�O�����ŕ`�悷��ۂ̎���������(idx���������j���̂��珑���o��
	std::string returnLine = bquery;
	for (auto iter = RangeCount.begin(); iter != RangeCount.end(); ++iter) {
		returnLine += "\t" + std::to_string(iter->second);
	}

	return (returnLine + "\n");
}




