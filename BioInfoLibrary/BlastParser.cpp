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
	
	//一行目で行う処理
	std::getline(in, line);
	vec = Utils::split(line, "\t");
	query = bquery = vec[0];
	queryToRefVec.emplace_back(vec);
	
	//2行目以降で行う処理
	std::ofstream out(outfile);
	out << header << "\n";
	while (std::getline(in, line)) {
		vec = Utils::split(line, "\t");
		query = vec[0];

		if (query == bquery) {
			//queryが同一のものはすべて回収
			//blastで参照側が整列して出力される保証がないため

			queryToRefVec.emplace_back(vec);

		}
		else {//(A,a)->(B,b)
			//これまでの行で読み込んだクエリー (A) の書き出し
			out << bph->valueFormatter(bquery, queryToRefVec);
			queryToRefVec.clear();

			bquery = query;//queryを(b)に再登録

			//現在の行(b)の処理
			queryToRefVec.emplace_back(vec);
		}

	}

	//EOFで行う処理
	out << bph->valueFormatter(bquery, queryToRefVec);

	in.close();
	out.close();

}

const std::string BlastParserPt1Imple::valueFormatter(const std::string& bquery, const std::vector<std::vector<std::string> >& queryToRefVec) {
	std::unordered_map <std::string, int> refcounter; /**< keyは参照を変換後のid valはその存在数 */
	std::string refid; /**< 参照を変換後に使用するid*/


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
		

		//最初の要素（行）の処理
		std::vector<std::string> blastLine = requireRescore[0];
		std::string brf                    = blastLine[REFERENCE_ID];
		std::string reference              = blastLine[REFERENCE_ID]; 
		int startBase                      = std::stoi(blastLine[REF_START]);
		int endBase                        = std::stoi(blastLine[REF_END]);
		float score                        = std::stof(blastLine[SCORE]);

		std::vector<int> hitBaseLoc; /**< アライメントした参照側の配列の場所 */
		scoreMap[reference] = score;

		//クエリと参照が逆向きにアライメントした場合.
		if (endBase < startBase) {
			std::swap(startBase, endBase);
		}

		for (int i = startBase; i <= endBase; ++i) {
			hitBaseLoc.push_back(i);
		}

		//２つめの要素（２行目）以降の処理
		for (auto iter = requireRescore.begin() + 1; iter != requireRescore.end(); ++iter) {

			blastLine = (*iter);
			reference = blastLine[REFERENCE_ID];
			startBase = std::stoi(blastLine[REF_START]);
			endBase   = std::stoi(blastLine[REF_END]);
			score     = std::stof(blastLine[SCORE]);

			//クエリと参照が逆向きにアライメントした場合.
			if (endBase < startBase) {
				std::swap(startBase, endBase);
			}

			
			if (reference != brf) {
				hitBaseLoc.clear(); //前行の参照のアライメント箇所は破棄

				scoreMap[reference] = score;

				//参照アライメント箇所の再登録（この行）
				for (int i = startBase; i <= endBase; ++i) {
					hitBaseLoc.push_back(i);
				}

				brf = reference;

			}
			else {
				//同一配列の別箇所にアライメントした場合.

				std::unordered_map<int,bool> tmpMap; /**< この行の参照アライメント箇所保持 */

				for (int i = startBase; i <= endBase; ++i) {
					tmpMap[i] = true;
				}

				bool flag = false; /**< 参照のアライメント箇所が重複したかどうか */
				for (const auto& i : hitBaseLoc) {
					if(tmpMap.count(i)== 1){
						flag = true;
						break;
					}
				}

				//アライメント領域が重複しない場合 ＝＞histBaseLocに登録
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

    //最初の要素のみ
	blastLine = queryToRefVec[0];
	startBase = std::stoi(blastLine[QUERY_START]);
	endBase   = std::stoi(blastLine[QUERY_END]);
	score     = std::stof(blastLine[SCORE]);

	//クエリと参照が逆向きにアライメントした場合
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

		//同じ参照に複数回ヒットした場合
		if (blastLine[REFERENCE_ID] == bref) {
			//アライメントが重複しない場合(スコアの加算）
			if (range.second < startBase || range.first > endBase) {

				//アライメント領域を追加（複数ヒット用）
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
				//アライメント部分が前の行と重複するためスキップ
				continue;
			}

		}
		else { //前の行と異なる参照にヒットした場合
			scoreMap[blastLine[SCORE]] = score ;
			align_sec_flag = false;
		}

		range = std::make_pair(startBase, endBase);
		bref = blastLine[REFERENCE_ID];
	}

	if (requireRescore.size() > 0) {

		//重複しないアライメント箇所が3か所以上ある可能性がある場合
		scoreMap = refAlignDup3More(scoreMap,requireRescore); 

	}

    //outformatして返却
	return outformat(bquery, scoreMap);

}

const std::string BlastParserPt2Imple::outformat(const std::string& bquery, const std::unordered_map <std::string, float>& scoreMap) {
	const int histgramStep = 100; //ヒストグラムで表示する際の１ブロックの刻み幅
	/**
	* score  100 200 300 400 500 ... n*100
	* query1   2  20   3  40   2 ...  0 //この関数で返却する行の文字列・例
	*/

	//ヒストグラムで描画する際の最大値を確定
	auto max_idx = std::max_element(
		std::begin(scoreMap),
		std::end(scoreMap),
		[](const std::pair<std::string, float>& p1, const std::pair<std::string, float >& p2) {
			return p1.second < p2.second;
		}
	);

	std::vector<int> RangeCount; /**< index =ヒストグラムの幅,val = カウント */
	for (int i = 0; i <= ((max_idx->second) / histgramStep); ++i) {
		RangeCount[i] = 0;
	}

	int idx = 0;
	for (auto iter = scoreMap.begin(); iter != scoreMap.end();++iter) {
		idx = int(iter->second/histgramStep);
		++RangeCount[idx];
	}

	//ヒストグラムで描画する際の軸が小さい(idxが小さい）ものから書き出す
	std::string returnLine = bquery;
	for (auto iter = RangeCount.begin(); iter != RangeCount.end(); ++iter) {
		returnLine += "\t" + std::to_string(*iter);

	}

	return (returnLine + "\n");

}


