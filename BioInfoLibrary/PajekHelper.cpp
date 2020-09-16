#include "PajekHelper.h"
#include "Utils.h"
#include <utility>


Pajek& PajekParser::load() {

	
	//*.net�t�@�C����ǂݍ���
	//n��node���쐬����->Vertices�N���X�쐬
	std::ifstream in{ infile };
	std::string line;

	//1�s�ڂɋL�ڂ���Ă���Node�����擾 // ex. *Vertices 550
	std::getline(in, line);
	std::regex re(R"(^*Vertices (\d+)$)");
	std::smatch match;
	std::regex_search(line, match, re);
	std::string nodeCount_s = match.str(1);
	int nodeCount = std::stoi(nodeCount_s);

	//Vertices�I�u�W�F�N�g�̍쐬
	std::vector<Node> nodeElements;//�enode�̏���ێ�����D
	std::vector<std::string>oneNode;//�ꎞ�I�Ɉ��node�̏���ێ�����D
	while (std::getline(in, line)) {
		if (line[0] == '*') {
			//Archs
			break;
		}

		/**
		* ��s���ȉ��̕����񂩂�node�I�u�W�F�N�g�𐶐�
		*	ex. 533	"SRP076876/SRR3720069"	1.033	1.033 
		*/
		oneNode = Utils::split(line, "\t");
		Label label{ oneNode[1] };
		const int nodeid = std::stoi(oneNode[0]);
		int x = std::stoi(oneNode[2]);
		int y = std::stoi(oneNode[3]);
		Node node(nodeid, label, x, y);
		nodeElements.emplace_back(node);

	}

	Vertices vertices(nodeElements);

	//Edges�I�u�W�F�N�g�̍쐬
	oneNode.clear();
	int p1 = 0;	//�y�A�̈��
	int p2 = 0;	//�y�A�̂������
	//int strength = 0;	//node�̌��т��̋���
	std::vector<std::pair<int, int>> mpair;//module�̃y�A��vector
	int index = 0;
	while (std::getline(in, line)) {
		oneNode = Utils::split(line, "\t");
		p1 = std::stoi(oneNode[0]);
		p2 = std::stoi(oneNode[1]);
		//strength = std::stoi(oneNode[2]);
		mpair.emplace_back(std::make_pair(p1, p2));
		index++;
	}
	Edges egs(mpair);

	//Pajek�I�u�W�F�N�g�̍쐬
	Pajek pajek(vertices, egs);

	return pajek ;

}

Vertices::Vertices(std::vector<Node>& nodeElements):
	nodeElements(nodeElements){
}

std::string Label::getLabel() const {
	return label;
}

void Label::setLabel(const std::string label) {
	this->label = label;
}

LabelDouble::LabelDouble(std::string label, const std::string sep) 
	: Label(label) ,sep(sep) {

	//initialize
	std::vector<std::string> labels = Utils::split(label,sep);
	upLabel = labels[0];
	lwLabel = labels[1];
}

std::string LabelDouble::getUpLabel() const {
	return upLabel;
}

std::string LabelDouble::getLwLabel() const {
	return lwLabel;
}

Edges::Edges(std::vector<std::pair<int, int>>& mpair):
	mpair(mpair) {

	//initialize
	for (size_t i = 0; i < mpair.size(); ++i) {
		mnodeset.insert(mpair[i].first);
		mnodeset.insert(mpair[i].second);
	}

}

bool Edges::isModule(int index)
{
	decltype(mnodeset)::iterator it = mnodeset.find(index);
	if (it != mnodeset.end()) { // ��������
		return true;
	}else {
		return false;
	}
		
}

