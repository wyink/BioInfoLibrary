#include "PajekHelper.h"
#include "Utils.h"
#include <utility>

Pajek& PajekParser::load() {

	
	//*.netファイルを読み込む
	//n個のnodeを作成する->Verticesクラス作成
	std::ifstream in{ infile };
	std::string line;

	//1行目に記載されているNode数を取得 // ex. *Vertices 550
	std::getline(in, line);
	std::regex re(R"(^*Vertices (\d+)$)");
	std::smatch match;
	std::regex_search(line, match, re);
	std::string nodeCount_s = match.str(1);
	int nodeCount = std::stoi(nodeCount_s);

	//Verticesオブジェクトの作成
	std::vector<Node> nodeElements;//各nodeの情報を保持する．
	std::vector<std::string>oneNode;//一時的に一つのnodeの情報を保持する．
	while (std::getline(in, line)) {
		if (line[0] == '*') {
			//Archs
			break;
		}

		/**
		* 一行ずつ以下の文字列からnodeオブジェクトを生成
		*	ex. 533	"SRP076876/SRR3720069"	1.033	1.033 
		*/
		oneNode = Utils::split(line, "\t");
		Label label{ oneNode[1] };
		const int nodeid = std::stoi(oneNode[0]);
		float x = std::stof(oneNode[2]);
		float y = std::stof(oneNode[3]);
		std::string icolor = oneNode[4];
		std::string bcolor = oneNode[5];

		Node node(nodeid, label);
		node.setPosition(x, y)
			.setInnerColor(icolor)
			.setBorderColor(bcolor);
		
		nodeElements.emplace_back(node);

	}

	Vertices vertices(nodeElements);

	//Edgesオブジェクトの作成
	oneNode.clear();
	int p1 = 0;	//ペアの一つ
	int p2 = 0;	//ペアのもう一つ
	//int strength = 0;	//nodeの結びつきの強さ
	std::vector<std::pair<int, int>> mpair;//moduleのペアのvector
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

	//Pajekオブジェクトの作成
	Pajek pajek(vertices, egs);

	return pajek ;

}

Node& Node::setPosition(const float x, const float y){
	this->x = x;
	this->y = y;
	return *this;
}

Node& Node::setInnerColor(const std::string icolor){
	this->icolor = icolor;
	return *this;
}

Node& Node::setBorderColor(const std::string bcolor){
	this->bcolor = bcolor;
	return *this;
}

const std::string& Node::getSingleLine() {
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
	if (it != mnodeset.end()) { // 見つかった
		return true;
	}else {
		return false;
	}
		
}

