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
 * @brief blast���ʃt�@�C���̎Q�Ƒ���ϊ����Ă��̒l���J�E���g�A�b�v���ďo��
 * @details
 *         Example :
 *                 A a   -> taxid 123
 *                 A a   -> taxid 123
 *                 A d   -> taxid 234
 *                 A b   -> taxid 123
 *                 
 *                 1. a,b�����ꂼ��Ή�����id�ɕϊ�����
 *                 2. ����id(a,a)��2�x�ڂɏo�Ă����ꍇ�ɃJ�E���g���Ȃ��B
 *                 3. a,b�͕ϊ����id�������ł��邽�߃J�E���g�A�b�v
 *                 4. �ϊ����id�̐����o�͂���B
 *                 5. �o�͈͂ȉ����Q��
 * -------------------------------------------
 *                    A 2
 *                    123:2,234:1
 * 
 * -------------------------------------------
 */
class BlastParserPt1Imple : public IBlastParser {
private:
	std::unordered_map<std::string, std::string> queRef;

public:
	explicit BlastParserPt1Imple(const std::unordered_map<std::string, std::string>& queRef) 
		:queRef(queRef) {}


	inline const std::string& convert(const std::string& reference) override {
		return queRef.at(reference);
	}

	const std::string valueFormatter(
		const std::string& bquery, 
		const std::vector<std::vector<std::string> >& queryToRefVec
	) override;


	 const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& refcounter
	) override;


};

/**
* @details ����N�G���ɑ΂��Areference������ł���ꍇ�ɎQ�Ƃ̔z��
*         �̃q�b�g�̈悪�d�Ȃ�Ȃ��ꍇ�A�X�R�A�����Z����B
*         �܂��A���ꂼ�̃X�R�A�����l�ŋ�؂�A������N�G������
*         �ɏo�͂���
* �@�@�@�@ex. 
* �@�@�@�@query1 reference1 
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

	inline const std::string& convert(const std::string& reference) override {
		return queRef.at(reference);
	}

	const std::string valueFormatter(
		const std::string& bquery,
		const std::vector<std::vector<std::string> >& queryToRefVec
	) override;


	const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& scoreMap
	) override;

	
};


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
	void run(const fs::path& outfile, const std::string& header);
};

#endif // !BLASTPARSER_H_