#ifndef BLASTPARSER_H_
#define BLASTPARSER_H_

#include <map>
#include <set>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>
#include <filesystem>
#include "Utils.h"

namespace fs = std::filesystem;

class BlastParserHandler {

public:
	BlastParserHandler() = default;
	virtual ~BlastParserHandler() = default;

	virtual const std::string& convert(const std::string& reference) = 0;

	virtual const std::string valueFormatter(
		const std::string& bquery, 
		const std::set<std::vector<std::string> >& queryToRefSet
	) = 0;

	virtual const std::string outformat(
		const std::string& bquery, 
		const std::map<std::string, int>& refcounter
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
	std::map<std::string, std::string> queRef;

public:
	explicit BlastParserPt1Imple(const std::map<std::string, std::string>& queRef) 
		:queRef(queRef) {}


	inline const std::string& convert(const std::string& reference) override {
		return queRef[reference];
	}

	const std::string valueFormatter(
		const std::string& bquery, 
		const std::set<std::vector<std::string> >& queryToRefSet
	) override;

	const std::string outformat(
		const std::string& bquery, 
		const std::map<std::string, int>& refcounter
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