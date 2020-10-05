#include "PajekHelper.h"
#include "Utils.h"
#include <utility>

PajekParser::PajekParser(const fs::path infile, std::unique_ptr<LabelInterface> ilabelptr) :
    infile(infile),ilabelptr(std::move(ilabelptr)){
    if (!fs::exists(infile)) {
        throw "No such file!\n";
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
    std::shared_ptr<std::vector<std::shared_ptr<Node> > > nodeElements;       /**< �enode�̏���ێ�����D            */
    std::vector<std::string>oneNode;       /**< �ꎞ�I�Ɉ��node�̏���ێ�����D*/
    while (std::getline(in, line)) {
        if (line[0] == '*') {              // *Archs�ɂ��Ă͌��ݎ�舵��Ȃ�
            break;
        }

        /**
         * ��s���ȉ��̕����񂩂�node�I�u�W�F�N�g�𐶐�
         *    ex. 533    "SRP076876/SRR3720069"    1.033    1.033 
         */
        oneNode = Utils::split(line, "\t");

        //"xxx/xxx"��'"'���Ȃ�
        oneNode[1].erase(0,1);
        oneNode[1].pop_back();


        //!Todo �p�����[�^�������Ȃ��t�H�[�}�b�g�Ή�
        //Node�I�u�W�F�N�g�\�z�ɕK�v�ȃp�����[�^
        const int nodeid = std::stoi(oneNode[0]);         /**< �eNode�̈�ӂ�ID    */
        LabelInterface* ilabel_ = ilabelptr->clone(oneNode[1]);
        std::shared_ptr<LabelInterface> label(ilabel_);    /**< �eNode�̃��x���@    */
        float x = std::stof(oneNode[2]);                  /**< �eNode��x���W       */
        float y = std::stof(oneNode[3]);                  /**< �eNode��y���W       */
        std::string icolor = oneNode[4];                  /**< �eNode�̓����̐F    */
        std::string bcolor = oneNode[5];                  /**< �eNode�̋��E���̐F  */

        std::shared_ptr<Node> nodeptr = std::make_shared<Node>(nodeid, label);

        nodeptr->setPosition(x, y)->setInnerColor(icolor)->setBorderColor(bcolor);
        
        nodeElements->emplace_back(nodeptr);

    }

    std::shared_ptr<Vertices> vertices = std::make_shared<Vertices>(nodeElements);

    //Edges�I�u�W�F�N�g�̍쐬
    oneNode.clear();
    int p1 = 0;                                /**< �y�A�̈��nodeid               */
    int p2 = 0;                                /**< �y�A�̂������nodeid           */
    //int strength = 0;                        /**< node�̂Ȃ���������d��         */
    std::vector<std::pair<int, int>> mpair;    /**< module�̃y�A��vector             */
    int index = 0;                             /**< nodeid�̑����ɗ��p����ꎞ�ϐ�   */

    while (std::getline(in, line)) {
        oneNode = Utils::split(line, "\t");
        p1 = std::stoi(oneNode[0]);
        p2 = std::stoi(oneNode[1]);
        //strength = std::stoi(oneNode[2]);
        mpair.emplace_back(std::make_pair(p1, p2));
        index++;
    }

    std::unique_ptr<Edges> egs = std::make_unique<Edges>(mpair);

    //pajekLabel�ipajek�I�u�W�F�N�g�̓���id�j
    //�����ł̓t�@�C�����i�g���q����)��pajekLabel�Ɏw�肷��B
    std::string pajekLabel = infile.stem().string<char>();
    Pajek pajek(pajekLabel,vertices, std::move(egs));

    return pajek ;

}

void Vertices::setColor(const std::string& color) {
    for(auto iter = nodeElements->begin() ;iter != nodeElements->end() ; ++iter){
        (*iter)->setInnerColor(color);
        (*iter)->setBorderColor(color);
    }
}

Node* Node::setPosition(const float x, const float y){
    this->x = x;
    this->y = y;
    return this;
}

Node* Node::setInnerColor(const std::string& icolor){
    this->icolor = icolor;
    return this;
}


Node* Node::setBorderColor(const std::string& bcolor){
    this->bcolor = bcolor;
    return this;
}


const std::string Node::getSingleLine() {
    return static_cast<const Node*>(this)->getSingleLine();
}

const std::string Node::getSingleLine() const {
    std::stringstream singleLine;

    singleLine << "\t" << nodeid << "\t" << ilabel->getOutputLabel();

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




Vertices::Vertices(std::shared_ptr<std::vector<std::shared_ptr<Node> > > nodeElements):
    nodeElements(nodeElements){
}

std::string LabelSingle::getLabel() {
    return label;
}


std::string LabelSingle::getOutputLabel() {
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

std::string LabelDouble::getLabel() {
    return this->label;
}

std::string LabelDouble::getUpLabel(){
    return upLabel;
}


std::string LabelDouble::getLwLabel(){
    return lwLabel;
}

std::string LabelDouble::getOutputLabel() {
    return '"' + this->getLabel() + '"';
}

void LabelDouble::setLabel(const std::string label) {
    this->label = label;
}

Edges::Edges(const std::vector<std::pair<int, int>> mpair):
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

std::string Edges::getOutput() const {
    std::stringstream output;
    for (size_t i = 0; i < mpair.size(); ++i) {
        output << "\t" << mpair[i].first;
        output << "\t" << mpair[i].second;
        output << "\t1\n";
    }

    return output.str();
}


const std::vector<std::unique_ptr<Pajek> > CreateFromText::run() {
    std::ifstream in{ infile };

    /* id id value �ň�s���\�����ꂽ�e�L�X�g����value���L�[�Ƃ���map���\�� */
    std::map<float,std::vector<std::pair<std::string,std::string > >,std::greater<float> > valComSet;
    std::set<std::string> allIdSet; /*< ���̃e�L�X�g�Ɋ܂܂�邷�ׂĂ�idset */
    std::string line;
    std::vector<std::string> vec;
    std::string id_A;
    std::string id_B;
    float value;

    while (std::getline(in, line)) {
        vec = Utils::split(line, "\t");

        id_A = vec[0];              /*< id_A�ƂȂ���id */
        id_B = vec[1];              /*< id_B�ƂȂ���id */
        value = std::stof(vec[2]);  /*< id_A��id_B�̊֘A�������l */

        //value��������O�ʂ܂ł̐��x�ɕϊ�
        value *= 1000;
        value = round(value);
        value /= 1000;

        allIdSet.insert(id_A);
        allIdSet.insert(id_B);

        //value���L�[�Ƃ��ē����x�̂Ȃ��������id�y�A��valComSet�ŊǗ�����
        if (valComSet.count(value) == 1) {
            valComSet.at(value).emplace_back(std::make_pair(id_A, id_B));
        }
        else
        {
            std::vector<std::pair<std::string,std::string> > v{ std::make_pair(id_A,id_B) };
            valComSet[value] = v;
        }
    }


    //Node�I�u�W�F�N�g�̍\�z
    int nodeid = 0;
    std::shared_ptr<std::vector<std::shared_ptr<Node> > > nodeElements
        = std::make_shared<std::vector<std::shared_ptr<Node> > >();
    std::map<std::string, int> idLabelMap;
    for (const auto& id : allIdSet) {
        ++nodeid;
        LabelInterface* ilabelptr_ = m_ilabel->clone(id);
        std::shared_ptr<LabelInterface> ilabelptr(ilabelptr_);
        std::shared_ptr<Node> nodeptr = std::make_shared<Node>(nodeid, ilabelptr);
        nodeptr = addproperty(std::move(nodeptr));

        nodeElements->emplace_back(nodeptr);
        idLabelMap[id] = nodeid;
    }

    //Vertices�I�u�W�F�N�g�̍\�z
    std::shared_ptr<Vertices> vertices = std::make_shared<Vertices>(nodeElements);

    // Edges�I�u�W�F�N�g�̍\�z
    // value(1.00-0.00)�܂ŕϓ�����
    std::vector<std::unique_ptr<Pajek> > pajekArray;
    std::string pajekLabel;
    std::vector<std::pair<int, int> > idPairVec_; /**< �֌W���̂���nodeid�̑g���� */
    for(const auto& valCom :valComSet){
        //value ...ex. f-measure,���֌W��
        //vec�ɂ�id�̑g�ݍ��킹���i�[����Ă���D
        //id->nodeid
        int nid_1 = 0; /**< ��ڂ�nodeid */
        int nid_2 = 0; /**< ��ڂ�nodeid */
        for (const auto& idpair : valCom.second) {
            nid_1 = idLabelMap.at(idpair.first);
            nid_2 = idLabelMap.at(idpair.second);
            idPairVec_.emplace_back(std::make_pair(nid_1, nid_2));
        }

        std::unique_ptr<Edges> edges 
            = std::make_unique<Edges>(idPairVec_);

        pajekLabel = std::to_string(valCom.first).substr(0,5) ;//ex. 0.967000->0.967

        std::unique_ptr<Pajek> pajek
            = std::make_unique<Pajek>(pajekLabel, vertices, std::move(edges));
        pajekArray.emplace_back(std::move(pajek)); 
    }
   
    return pajekArray;
}


void Pajek::output(fs::path outfile) const {

    std::stringstream output;
    output << "*Vertices\t" << vtptr->size() << "\n";

    //�e�m�[�h
    std::string outputNode;
    const std::shared_ptr<std::vector<std::shared_ptr<Node> > > nodeElements = vtptr->getNodeElements();

    //for (const auto& node : nodeElements) {
    for(auto iter = nodeElements->begin();iter != nodeElements->end();++iter){
        outputNode += (*iter)->getSingleLine();
    }

    output << outputNode;
    output << "*Arcs" << "\n";
    output << "*Edges" << "\n";

    //�e�m�[�h�̊֌W��
    output << egsptr->getOutput();
    
    //�o�̓t�@�C���ɏ�������
    std::ofstream out{ outfile };
    out << output.str();
    out.close();
}

