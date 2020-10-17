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

class BlastParserHandler {

public:
	BlastParserHandler() = default;
	virtual ~BlastParserHandler() = default;

	virtual const std::string& convert(const std::string& reference) = 0;

	virtual const std::string valueFormatter(
		const std::string& bquery, 
		const std::vector<std::vector<std::string> >& queryToRefVec
	) = 0;

	virtual const std::string outformat(
		const std::string& bquery,
		const std::unordered_map<std::string, float>& refcounter
	) = 0;

};

/**
 * @detail blast���ʃt�@�C���̎Q�Ƒ���ϊ����Ă��̒l���J�E���g�A�b�v���ďo��
 * 
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
class BlastParserPt1Imple : public BlastParserHandler {
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
* @detail ����N�G���ɑ΂��Areference������ł���ꍇ�ɎQ�Ƃ̔z��
*         �̃q�b�g�̈悪�d�Ȃ�Ȃ��ꍇ�A�X�R�A�����Z����B
*         �܂��A���ꂼ�̃X�R�A�����l�ŋ�؂�A������N�G������
*         �ɏo�͂���
* �@�@�@�@ex. 
* �@�@�@�@query1 reference1 
*/
class BlastParserPt2Imple : public BlastParserHandler {
private :
	std::map<std::string, std::string> queRef;

	const std::unordered_map<std::string, float>& refAlignDup3More(
		std::unordered_map<std::string, float>& scoreMap,
		std::vector<std::vector<std::string> >& requireRescore
	);

public:
	explicit BlastParserPt2Imple(const std::map<std::string, std::string>& queRef)
		:queRef(queRef) {}

	inline const std::string& convert(const std::string& reference) override {
		return queRef[reference];
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
	std::shared_ptr<BlastParserHandler> bph;

public:
	explicit BlastParser(const fs::path& infile, std::shared_ptr<BlastParserHandler> bph);

	inline void setHandler(std::shared_ptr<BlastParserHandler> bph) {
		this->bph = std::move(bph);
	}

	/**
	* @brief blast���ʃt�@�C���̉�͊J�n
	*/
	void run(const fs::path& outfile, const std::string& header);
};

#endif