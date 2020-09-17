#include "PajekHelper.h"
#include "Utils.h"
#include <utility>

PajekParser::PajekParser(const std::string infile) :
	infile(infile) {

	if (!Utils::isExistFile(infile)) {
		throw "No such File.";
	}
}

Pajek PajekParser::load() {
	
	//1�s�ڂɋL�ڂ���Ă���Node�����擾 // ex. *Vertices 550
	std::ifstream in{ infile };
	std::string line;
	std::getline(in, line);
	std::regex re(R"(^*Vertices (\d+)$)");
	std::smatch match;
	std::regex_search(line, match, re);
	std::string nodeCount_s = match.str(1);
	int nodeCount = std::stoi(nodeCount_s);

	//Vertices�I�u�W�F�N�g�̍쐬
	std::vector<Node> nodeElements;		/**< �enode�̏���ێ�����D*/
	std::vector<std::string>oneNode;	/**< �ꎞ�I�Ɉ��node�̏���ێ�����D*/
	while (std::getline(in, line)) {
		if (line[0] == '*') {			// *Archs�ɂ��Ă͌��ݎ�舵��Ȃ�
			break;
		}

		/**
		* ��s���ȉ��̕����񂩂�node�I�u�W�F�N�g�𐶐�
		*	ex. 533	"SRP076876/SRR3720069"	1.033	1.033 
		*/
		oneNode = Utils::split(line, "\t");

		//"xxx/xxx"��'"'���Ȃ�
		oneNode[1].erase(0,1);
		oneNode[1].pop_back();

		//!Todo �p�����[�^�������Ȃ��t�H�[�}�b�g�Ή�
		//Node�I�u�W�F�N�g�\�z�ɕK�v�ȃp�����[�^
		const int nodeid = std::stoi(oneNode[0]);	/**< �eNode�̈�ӂ�ID   */
		Label label{ oneNode[1] };					/**< �eNode�̃��x���@   */
		float x = std::stof(oneNode[2]);			/**< �eNode��x���W		*/
		float y = std::stof(oneNode[3]);			/**< �eNode��y���W		*/
		std::string icolor = oneNode[4];			/**< �eNode�̓����̐F	*/
		std::string bcolor = oneNode[5];			/**< �eNode�̋��E���̐F */

		Node node(nodeid, label);
		node.setPosition(x, y)
			.setInnerColor(icolor)
			.setBorderColor(bcolor);
		
		nodeElements.emplace_back(node);

	}

	Vertices vertices(nodeElements);

	//Edges�I�u�W�F�N�g�̍쐬
	oneNode.clear();
	int p1 = 0;								/**< �y�A�̈��nodeid				*/
	int p2 = 0;								/**< �y�A�̂������nodeid			*/
	//int strength = 0;						/**< node�̂Ȃ���������d��		*/
	std::vector<std::pair<int, int>> mpair; /**< module�̃y�A��vector			*/
	int index = 0;							/**< nodeid�̑����ɗ��p����ꎞ�ϐ� */

	while (std::getline(in, line)) {
		oneNode = Utils::split(line, "\t");
		p1 = std::stoi(oneNode[0]);
		p2 = std::stoi(oneNode[1]);
		//strength = std::stoi(oneNode[2]);
		mpair.emplace_back(std::make_pair(p1, p2));
		index++;
	}

	Edges egs(mpair);
	Pajek pajek(vertices, egs);

	return pajek ;

}

void Vertices::setColor(const std::string& color) {
	for (size_t i = 0; i < nodeElements.size(); ++i) {
		nodeElements[i].setInnerColor(color);
		nodeElements[i].setBorderColor(color);
	}
}

Node& Node::setPosition(const float x, const float y){
	this->x = x;
	this->y = y;
	return *this;
}

Node& Node::setInnerColor(const std::string& icolor){
	this->icolor = icolor;
	return *this;
}


Node& Node::setBorderColor(const std::string& bcolor){
	this->bcolor = bcolor;
	return *this;
}


const std::string Node::getSingleLine() {
	std::stringstream singleLine;

	singleLine << "\t" << nodeid << "\t" << label.getOutputLabel();

	if (x > 0 && y > 0) {
		singleLine << "\t" << x << "\t" << y;
	}
	if (!icolor.empty()) {
		singleLine << "\tic" << icolor;
	}
	if (!bcolor.empty()) {
		singleLine << "\tbc" << bcolor;
	}

	singleLine << std::endl;
	return singleLine.str();
}


Vertices::Vertices(std::vector<Node>& nodeElements):
	nodeElements(nodeElements){
}


std::string Label::getLabel() {
	return label;
}


std::string Label::getOutputLabel() {
	std::string outputLabel = '"' + getLabel() + '"';
	
	return outputLabel ;
}


void Label::setLabel(const std::string label) {
	this->label = label;
}


LabelDouble::LabelDouble(std::string label, const char* sep) 
	: Label(label) ,sep(sep) {

	//initialize
	std::vector<std::string> labels = Utils::split(label,sep);
	upLabel = labels[0];
	lwLabel = labels[1];
}


std::string LabelDouble::getUpLabel(){
	return upLabel;
}


std::string LabelDouble::getLwLabel(){
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

