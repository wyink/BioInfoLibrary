#pragma once
#include <map>
#include <set>

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>
#include "Utils.h"

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


	const std::string& convert(const std::string& reference) override {
		return queRef[reference];
	}

	const std::string valueFormatter(const std::string& bquery ,const std::set<std::vector<std::string> >& queryToRefSet) override {
		std::map<std::string, int> refcounter; /**< key�͎Q�Ƃ�ϊ����id val�͂��̑��ݐ� */
		std::string refid; /**< �Q�Ƃ�ϊ���Ɏg�p����id*/


		for(auto ref : queryToRefSet) {

			refid = convert(ref[1]);

			if (refcounter.count(refid) == 1) {
				//�o�^�ς�
				refcounter[refid]++;
			}
			else {
				//���o�^
				refcounter[refid] = 1;
			}
		}

		return outformat(bquery, refcounter);
	}

	const std::string outformat(const std::string& bquery, const std::map<std::string, int>& refcounter) override {
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

};


class BlastParser
{
private:
	const std::string infile;
	BlastParserHandler& bph;

public:
	explicit BlastParser(const std::string& infile, BlastParserHandler& bph);

	void setHandler(BlastParserHandler& bph) {
		this->bph = bph;
	}

	/**
	* @brief blast���ʃt�@�C���̉�͊J�n
	*/
	void run(const std::string& outfile);
};

