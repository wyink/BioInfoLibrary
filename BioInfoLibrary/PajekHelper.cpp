#include "PajekHelper.h"
#include "Utils.h"
#include <utility>

PajekParser::PajekParser(const std::string infile, LabelInterface& ilabel) :
    infile(infile),ilabel(ilabel){

    if (!Utils::isExistFile(infile)) {
        throw "No such File.";
    }
}

Pajek PajekParser::load() {
    
    //1行目に記載されているNode数を取得 // ex. *Vertices 550
    std::ifstream in{ infile };
    std::string line;
    std::getline(in, line);
    std::regex re(R"(^*Vertices (\d+)$)");
    std::smatch match;
    std::regex_search(line, match, re);
    std::string nodeCount_s = match.str(1);
    int nodeCount = std::stoi(nodeCount_s);

    //Verticesオブジェクトの作成
    std::vector<Node> nodeElements;        /**< 各nodeの情報を保持する．            */
    std::vector<std::string>oneNode;       /**< 一時的に一つのnodeの情報を保持する．*/
    while (std::getline(in, line)) {
        if (line[0] == '*') {              // *Archsについては現在取り扱わない
            break;
        }

        /**
         * 一行ずつ以下の文字列からnodeオブジェクトを生成
         *    ex. 533    "SRP076876/SRR3720069"    1.033    1.033 
         */
        oneNode = Utils::split(line, "\t");

        //"xxx/xxx"の'"'を省く
        oneNode[1].erase(0,1);
        oneNode[1].pop_back();


        //!Todo パラメータを持たないフォーマット対応
        //Nodeオブジェクト構築に必要なパラメータ
        const int nodeid = std::stoi(oneNode[0]);         /**< 各Nodeの一意のID    */
        LabelInterface* label = ilabel.clone(oneNode[1]); /**< 各Nodeのラベル　    */
        float x = std::stof(oneNode[2]);                  /**< 各Nodeのx座標       */
        float y = std::stof(oneNode[3]);                  /**< 各Nodeのy座標       */
        std::string icolor = oneNode[4];                  /**< 各Nodeの内側の色    */
        std::string bcolor = oneNode[5];                  /**< 各Nodeの境界線の色  */

        Node node(nodeid, label);
        node.setPosition(x, y)
            .setInnerColor(icolor)
            .setBorderColor(bcolor);
        
        nodeElements.emplace_back(node);

    }

    Vertices vertices(nodeElements);

    //Edgesオブジェクトの作成
    oneNode.clear();
    int p1 = 0;                                /**< ペアの一つのnodeid               */
    int p2 = 0;                                /**< ペアのもう一つのnodeid           */
    //int strength = 0;                        /**< nodeのつながりを示す重み         */
    std::vector<std::pair<int, int>> mpair;    /**< moduleのペアのvector             */
    int index = 0;                             /**< nodeidの走査に利用する一時変数   */

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

    singleLine << "\t" << nodeid << "\t" << label->getOutputLabel();

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


const std::string LabelSingle::getLabel() {
    return label;
}


const std::string LabelSingle::getOutputLabel() {
    return '"' + this->getLabel() + '"';
}


void LabelSingle::setLabel(const std::string label) {
    this->label = label;
}


LabelDouble::LabelDouble(std::string label) 
    : label(label){

    //initialize
    std::vector<std::string> labels = Utils::split(label,"/");
    upLabel = labels[0];
    lwLabel = labels[1];
}

const std::string LabelDouble::getLabel() {
    return this->label;
}

const std::string LabelDouble::getUpLabel(){
    return upLabel;
}


const std::string LabelDouble::getLwLabel(){
    return lwLabel;
}

const std::string LabelDouble::getOutputLabel() {
    return '"' + this->getLabel() + '"';
}

void LabelDouble::setLabel(const std::string label) {
    this->label = label;
}

Edges::Edges(const std::vector<std::pair<int, int>>& mpair):
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
    if (it != mnodeset.end()) { 
        return true;
    }else {
        return false;
    }
}

const std::vector<Pajek> CreateFromText::run() {
    std::ifstream in{ infile };
    std::string line;
    std::vector<std::string> vec;
    std::string id_A;
    std::string id_B;
    float value;
    std::map<float,std::vector<std::pair<int, int > > > valComSet;
    std::set<std::string> allIdSet;
    while (std::getline(in, line)) {
        //id_A id_B value(int)
        vec = Utils::split(line, "\t");
        id_A = std::stoi(vec[0]);
        id_B = std::stoi(vec[1]);
        value = std::stof(vec[2]);
        allIdSet.insert(id_A);
        allIdSet.insert(id_B);
        valComSet[value].emplace_back(std::make_pair(id_A, id_B) );
    }


    //Nodeオブジェクトの構築
    int nodeid = 0;
    std::vector<Node> nodeElements;
    LabelInterface* ilabel;
    for (const auto& id : allIdSet) {
        ilabel = m_ilabel->clone(id);
        Node node(nodeid, ilabel);
        node = addproperty(node);
        nodeElements.emplace_back(node);
        ++nodeid;
    }

    //Verticesオブジェクトの構築
    Vertices vertices(nodeElements);

    //Edgesオブジェクトの構築
    //value(1.00-0.00)まで変動する
    std::vector<Pajek> pajekArray;
    for (const auto& [value, idPairVec] : valComSet) {
        //value ...ex. f-measure,相関係数
        //vecにはidの組み合わせが格納されている．
        Edges edges(idPairVec);
        pajekArray.emplace_back(Pajek(vertices, edges));
    }
   
    return pajekArray;
}



//example 
//3番目の引数は中身を指定しない場合は以下のようにする
/*
 [](Node& node)->Node& {
    return node;
 }

*/
fs::path infile = "temp.txt";
LabelDouble* ilabel;
CreateFromText cft(
    infile,
    ilabel,
    [](Node& node)->Node& {
        node.setPosition(0.500, 0.500);
        return node;
    }
);