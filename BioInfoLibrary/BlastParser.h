#ifndef BLASTPARSER_H_
#define BLASTPARSER_H_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <unordered_map>
#include "Utils.h"

namespace fs = std::filesystem;

/**
* @enum BlastResultParam
* @brief BLAST application ����o�͂����^�u��؂�̃t�@�C����
*        ���ꂼ��̃J����
*/
enum BlastResultParam {
	QUERY_ID,
	REFERENCE_ID,
	IDENTITY,
	ALIGHN_LEN,
	MIS_MATCH_COUNT,
	GAP_COUNT,
	QUERY_START,
	QUERY_END,
	REF_START,
	REF_END,
	E_VALUE,
	SCORE,
	QUERY_LEN,
	REF_LEN
};

/**
* @brief BLAST���ʃt�@�C������͂���ۂɗ��p�ł���n���h��
* @details 
* BLAST���ʃt�@�C����ǂݍ��ލہA�ȉ���3�̃n���h����ݒ肷�邱�Ƃ��ł���
* 1. BLAST���ʃt�@�C����2�J�����ڂ̎Q�Ƒ�ID������ɕR�Â��ʂ�ID�ɕϊ�
* 2. BLAST���ʃt�@�C����1���R�[�h�𗘗p���čs����̓I�ȏ������e
* 3. �o�̓t�H�[�}�b�g
*/
class IBlastParser{
public:
	IBlastParser() = default;
	virtual ~IBlastParser() = default;

	/**
	* @brief BLAST���ʃt�@�C����2�J�����ڂ̎Q�Ƒ�ID������ɕR�Â��ʂ�ID�ɕϊ�
	* @details ���̃n���h������������ꍇ�͂��̃����o�ϐ��ɕϊ��Ɏg�p����}�b�v���R���X�g���N�^�œn���K�v������
	* @param[reference] �ϊ�������ID
	*/
	virtual const std::string& convert(const std::string& reference) = 0;

	/**
	* @brief BLAST���ʃt�@�C����1���R�[�h�𗘗p���čs����̓I�ȏ������e
	* @param[bquery] BLAST���ʃt�@�C���̑��J����
	* @param[queryToRefVec] BLAST���ʃt�@�C����1���R�[�h
	*/
	virtual const std::string valueFormatter(
		const std::string& bquery, 
		const std::vector<std::vector<std::string> >& queryToRefVec
	) = 0;

	/**
	* @brief �o�̓t�H�[�}�b�g�����肷��
	* @param[bquery] BLAST���ʃt�@�C���̑�1�J����
	* @param[refcounter] bquery�ɑ΂���l
	*/
	virtual const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& refcounter
	) = 0;

};

/**
 * @brief Blast���ʃt�@�C���̎Q�Ƃ�taxid�ɕϊ����A�e�N�G���ɑ΂��Ă��̃J�E���g���o�͂��鏈�����s��
 * @details
 *                Blast���ʃt�@�C���̃N�G���[�ƎQ�Ƃ���т��̎Q�Ƃ��������taxid���ȉ��Ɏ����悤�ȑg�����ł���ꍇ�ɂ��čl����B
 *					
 *                ### ���͗�
 *                   Blast���ʃt�@�C���𔲐����ăe�[�u���ŕ\��
 *					|  Query   |  Reference  | taxid  |									
 *					| :------: | :---------: | -----: |									
 *					| QueryA   |	RefA     | 123	  |									
 *					| QueryA   |	RefA     | 123	  |									
 *					| QueryA   |	RefB     | 234	  |									
 *					| QueryA   |	RefD     | 123	  |									
 *					
 *                  ________________________________________________________________
 *					1. QueryA,QueryB�����ꂼ��Ή�����taxid�ɕϊ�						
 *					2. ����id(RefA)��2�x�o�ꂷ�邪�A���̂悤�ȏd���̓J�E���g���Ȃ�		
 *					3. RefA,RefD�͑�����taxid�������ł��邽�߃J�E���g�A�b�v				
 *					4. �ϊ����taxid�̐����o�͂���B									
 *					5. �o�͈͂ȉ����Q��													
 *					________________________________________________________________
 *
 *			     ### �o�͗�
 *                 QueryA�́u2�v��ނ�taxid(123,234)�̂ǂꂩ�ɏ�������\����������Ă���B
 *                 ���ӓ_�Ƃ��āAQueryA��123�ɑ�����2�̈قȂ�z��Ƀg�b�v�q�b�g���Ă���B
 *				   ____________															
 *				   QueryA 2	                  														
 *				   123:2,234:1																				
 *				   ____________														
 */
class BlastParserPt1Imple : public IBlastParser {
private:
	//�ϊ��p�̃}�b�v�iex. �Q��ID����taxonomyId�j
	std::unordered_map<std::string, std::string> queRef;

public:
	explicit BlastParserPt1Imple(const std::unordered_map<std::string, std::string>& queRef) 
		:queRef(queRef) {}


	/**
	* @brief BLAST���ʃt�@�C����2�J�����ڂ̎Q�Ƒ�ID������ɕR�Â��ʂ�ID�ɕϊ�
	* @details ���̃n���h������������ꍇ�͂��̃����o�ϐ��ɕϊ��Ɏg�p����}�b�v���R���X�g���N�^�œn���K�v������
	* @param[reference] �ϊ�������ID
	*/
	inline const std::string& convert(const std::string& reference) override {
		return queRef.at(reference);
	}

	/**
	* @brief BLAST���ʃt�@�C����1���R�[�h�𗘗p���čs����̓I�ȏ������e
	* @param[bquery] BLAST���ʃt�@�C���̑��J����
	* @param[queryToRefVec] BLAST���ʃt�@�C����1���R�[�h
	*/
	const std::string valueFormatter(
		const std::string& bquery,
		const std::vector<std::vector<std::string> >& queryToRefVec
	) override;

	/**
	* @brief �o�̓t�H�[�}�b�g�����肷��
	* @param[bquery] BLAST���ʃt�@�C���̑�1�J����
	* @param[refcounter] bquery�ɑ΂���l
	*/
	 const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& refcounter
	) override;


};

/**
 * @brief Blast���ʃt�@�C�����A�e�N�G���ɑ΂���Q�Ƃ̃X�R�A���z���o�͂���
 * @details
 *          �e�N�G���ɑ΂��ăq�b�g�����Q�Ƃ̃X�R�A�Q��ێ�����B�������A����N�G���ɑ΂��āu�����
 *          �Q�Ƃɕ����q�b�g�v���u���̃A���C�����g�̈悪�d�����Ȃ��ꍇ�v�͉��Z�����X�R�A��ێ�
 *          �������Ċe�N�G�����ێ�����X�R�A�Q�����l�ŋ�؂��ďo�͂���B�ȉ��ɂ��̗�������B
 *              ### ���͗�
 *                  ����N�G���E����Q�Ƃɑ΂��ĎQ�Ƃ̃A���C�����g���d�����Ȃ��ꍇ
 *
 * 					|  Query   | Reference   | RefAlignStart | RefAlignEnd | Score |
 *					| :------: | :---------: | ------------: | ----------: |  ---: |
 *					| QueryA   | Reference1  | 	1            | 150	       |   190 |
 *					| QueryA   | Reference1  | 	160          | 180	       |    30 |
 *					| QueryA   | Reference2  | 	1            | 190	       |   300 |
 *					| QueryB   | ...         | ...	         | ...	       |   ... |
 *
 *              ### �o�͗�
 *                  �d�����Ȃ�����190��30�����Z���ďo��
 *                    | score  | 100 | 200 | 300 | 400 | 500 | ... | n*100 |
 *                    | :---:  | --: | --: | --: | --: | --: | --: | ----: |
 *                    | QueryA |   0 |   1 |  1  |  0  | 0   |  0  | 	0  |
 *                    | QueryB | ... | ... | ... | ... | ... | ... |   ... |
 * 
*/
class BlastParserPt2Imple : public IBlastParser {
private :
	std::unordered_map<std::string, std::string> queRef;

	const std::unordered_map<std::string, float>& refAlignDup3More(
		std::unordered_map<std::string, float>& scoreMap,
		std::vector<std::vector<std::string> >& requireRescore
	);

public:
	explicit BlastParserPt2Imple(const std::unordered_map<std::string, std::string>& queRef)
		:queRef(queRef) {}
	
	//�ϊ����K�v�Ȃ��ꍇ
	explicit BlastParserPt2Imple()
		:queRef{ std::make_pair("","") } {}

	/**
	* @brief BLAST���ʃt�@�C����2�J�����ڂ̎Q�Ƒ�ID������ɕR�Â��ʂ�ID�ɕϊ�
	* @details ���̃n���h������������ꍇ�͂��̃����o�ϐ��ɕϊ��Ɏg�p����}�b�v���R���X�g���N�^�œn���K�v������
	* @param[reference] �ϊ�������ID
	*/
	inline const std::string& convert(const std::string& reference) override {
		return queRef.at(reference);
	}

	/**
	* @brief BLAST���ʃt�@�C����1���R�[�h�𗘗p���čs����̓I�ȏ������e
	* @param[bquery] BLAST���ʃt�@�C���̑��J����
	* @param[queryToRefVec] ����N�G����BLAST���ʃt�@�C����1���R�[�h�̃x�N�^
	*/
	const std::string valueFormatter(
		const std::string& bquery,
		const std::vector<std::vector<std::string> >& queryToRefVec
	) override;

	/**
	* @brief �o�̓t�H�[�}�b�g�����肷��
	* @param[bquery] BLAST���ʃt�@�C���̑�1�J����
	* @param[scoreMap] bquery�ɑ΂���l
	*/
	const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& scoreMap
	) override;

};

/*
* @brief ����N�G���ɑ΂��đ����������Ńq�b�g�����Q��ID�Q�ɂ����āA
*        �X�R�A��100��藣���ꍇ�͏o�͂��Ȃ��B
*        ����N�G���E����Q�Ƃɑ΂��鑊���������̃X�R�A�̉��Z�����͐e�N���X�̕��������̂܂܌p��
*/
class BlastParserPt2ex :public BlastParserPt2Imple {
public:
	explicit BlastParserPt2ex(){} //�e�N���X�̃f�t�H���g�R���X�g���N�^�Ăяo����ɍ\�z

	const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& scoreMap
	) override;
	

};

/**
* @brief BLAST���ʃt�@�C���̓ǂݍ��ނ����s����N���X
* @details ���̃N���X�ɂ�BLAST���ʃt�@�C�����ǂ̂悤�ɉ�͂��邩���w�肷�邽�߂�
*         �n���h����K�v�Ƃ���B
*/
class BlastParser
{
private:
	const fs::path infile;
	std::shared_ptr<IBlastParser> bph;

public:
	explicit BlastParser(const fs::path& infile, std::shared_ptr<IBlastParser> bph);

	inline void setHandler(std::shared_ptr<IBlastParser> bph) {
		this->bph = std::move(bph);
	}

	/**
	* @brief blast���ʃt�@�C���̉�͊J�n
	*/
	void run(const fs::path& outfile, const std::string& outTxtHeader,bool isHeader=false);
};

#endif // !BLASTPARSER_H_