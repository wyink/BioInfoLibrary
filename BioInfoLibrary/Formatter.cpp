#include "Formatter.h"



FaaToFasta::FaaToFasta(const fs::path& infile, const fs::path& out):
	infile(infile),
	out(out),
	re("(^>[A-Z]+?\\d+?\\.\\d)") ,
	filter(NULL){}

void FaaToFasta::run() {
	std::ifstream in{ infile };

	if (!in.is_open()) {
		std::cout << "Can't open the file!" << std::endl;
		return;
	}

	std::string line;
	std::smatch match;       /**< match���������� */
	std::string id;          /**< id�i�[�p */
	std::string seqs;        /**< �z��i�[�p */
	std::string idLine;      /**< id�̃t�B���^�[�Ɏg�p */

	//��s�ڂ̂ݕʏ���
	std::getline(in, line);
	std::regex_search(line, match, re);
	id = match.str(1);
	idLine = line;

	//�o�̓t�@�C��
	std::ofstream of{ out };
	while (std::getline(in, line)) {
		if (line[0] == '>') {
			

			//�O�̍s�̏����o��
			if (filter(idLine)) {
				of << id << "\n" << seqs << "\n";
			}
			seqs.clear();

			//���݂̍s�̏����擾
			std::regex_search(line, match, re);
			id = match.str(1);
			idLine = line;
		}
		else {
			seqs += line;
		}
	}

	//�Ō�̍s�̏����o��
	of << id << "\n" << seqs << "\n";
	seqs.clear();

	//�����I�Ƀf�X�g���N�^�͌Ă΂��
	in.close();
	of.close();

}

FaaToFasta& FaaToFasta::setIdFilter(std::regex re) {
	this->re = re;
	return *this;
}

FaaToFasta& FaaToFasta::setIdFilterCall(std::function<bool(std::string)> filter) {
	this->filter = filter;
	return *this;
}


