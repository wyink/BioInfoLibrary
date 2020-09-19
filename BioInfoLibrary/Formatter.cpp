#include "Formatter.h"


void Formatter::FaaToFasta(
		std::string faafile, 
		std::string out, 
		std::regex re,
		std::function<bool(std::string)> filter
	) 
{
	std::ifstream in{ faafile };

	if (!in.is_open()) {
		std::cout << "Can't open the file!" << std::endl;
		return;
	}

	std::string line;
	std::smatch match;       /**< match���������� */
	std::string id;          /**< id�i�[�p */
	std::string seqs;        /**< �z��i�[�p */

	//��s�ڂ̂ݕʏ���
	std::getline(in, line);
	std::regex_search(line, match, re);
	id = match.str(1);

	//�o�̓t�@�C��
	std::ofstream of{ out };
	while (std::getline(in, line)) {
		if (line[0] == '>') {
			

			//�O�̍s�̏����o��
			if (filter(id)) {
				of << id << "\n" << seqs << "\n";
			}
			seqs.clear();

			//���݂̍s�̏����擾
			std::regex_search(line, match, re);
			id = match.str(1);

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

void Formatter::FaaToFasta(std::string faafile,std::string out) {
	std::regex re{ "(^>[A-Z]+?\\d+?\\.\\d)" };

	Formatter::FaaToFasta(
		faafile, 
		out, 
		re, 
		[](std::string id)->bool {return true;}
	);
}

