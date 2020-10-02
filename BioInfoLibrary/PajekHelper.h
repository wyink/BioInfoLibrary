#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <functional>
#include <memory>
#include <regex>
#include <set>
#include <map>

namespace fs = std::filesystem;

/**
 * @brief     Pajekアプリケーションで利用するファイルの解析
 * 
 * @detail    Pajekアプリケーションでは拡張子.netのファイルを出力として利用する．
 *            このクラスではそのファイルを解析してその内容を保管するためのPajek
 *            オブジェクトに変換する．
 */
class Pajek;
class LabelInterface;
class PajekParser {
    const fs::path infile;
    std::unique_ptr<LabelInterface> ilabelptr;

public:
    /**
     * @brief            PajekParserオブジェクトのコンストラクタ
     * @param[infile]    読み込むファイル（拡張子.net)
     * @detail           読み込みたい.netファイルのパスをセット
     */
    PajekParser(
        const fs::path infile,
        std::unique_ptr<LabelInterface> ilabelptr
    );

    /**
     * @brief    .netファイルをロード
     * @return    入力ファイルの情報を保持したPajekオブジェクト
     */
    Pajek load(); 
    
};


/**
* @brief 一意の文字列ラベルを示すインタフェース
*/
class LabelInterface {
public:

    LabelInterface() = default;

    /**
     * @brief     label文字列を取得
     * @return    ラベルに利用している文字列('"'を含まない）
     */
    virtual std::string getLabel() = 0;

    /**
     * @brief　  出力に使用するlabel文字列
     * @return   ラベルに利用している文字列（'"'を含む）
     */
    virtual std::string getOutputLabel() = 0;

    /**
     * @brief           label文字列をセット
     * @param[label]    更新したいラベル文字列
     */
    virtual void setLabel(const std::string label) = 0;

    virtual ~LabelInterface() = default;

    /**
     * @brief オブジェクトのコピーを作成
     *        メモリを動的に確保している．
     */
    virtual LabelInterface* clone(const std::string label) = 0;

};



/**
* @brief Verticesの一行分を構成するNodeに関するクラス
* @note 各NodeはPajekオブジェクトの部分オブジェクトを
*       構成する．各NodeはPajekオブジェクトでポインタ
*       として保持され，また複数のPajekオブジェクトで
*       共有される．したがってメンバ変数のilabelは
*       shared_ptrで管理している．
*/
class Node
{
private:
    int nodeid;
    std::shared_ptr<LabelInterface> ilabel;
    float x;
    float y;
    std::string icolor; //空文字列で初期化
    std::string bcolor; //空文字列で初期化

public:
    /**
     * @brief            Nodeクラスのコンストラクタ
     * @param[nodeid]    Nodeの一意のID
     * @param[ilabel]     Nodeが保持するラベル
     */
    Node(const int nodeid, std::shared_ptr<LabelInterface> ilabel) :
        nodeid(nodeid), ilabel(ilabel),
        x(-1), y(-1) {}
            

    inline const std::shared_ptr<LabelInterface> getLabelptr() const {
        return ilabel;
    }


    /**
     * @brief       各Nodeの座標をセット
     * @param[x]    各Nodeのx座標
     * @param[y]    各Nodeのy座標
     */
    Node* setPosition(
        const float x,
        const float y
    );

    /**
     * @brief            各Nodeの内側の色をセット
     * @param[icolor]    各Nodeを更新する内側の色
     */
    Node* setInnerColor(const std::string& icolor);

    /**
     * @brief            各Nodeの境界線の色をセット
     * @param[bcolor]    各Nodeを更新する境界線の色
     */
    Node* setBorderColor(const std::string& bcolor);

    /**
     * @brief    出力に使用するフォーマット文字列を取得
     * @return    .netでの出力に利用する文字列（改行含む）
     */
    const std::string getSingleLine();
    const std::string getSingleLine() const;
};



/**
* @brief Pajekの.netファイルを構成する各Nodeの定義
*/
class Vertices
{
    std::shared_ptr<std::vector<std::shared_ptr<Node> > > nodeElements;
    Vertices(const Vertices& vt) = delete;

public:

    /**
　　 * @brief     Verticesクラスのコンストラクタ
　　 * @detail    .netファイルにおける各Nodeの定義部分
　　 */
    explicit Vertices(std::shared_ptr<std::vector<std::shared_ptr<Node> > > nodeElements);


    /**
     * @brief 部分オブジェクトのnodeElementsの要素数を返す．
     */
    inline const size_t size() const {
        return nodeElements->size();
    }

    /**
    * @detail 部分オブジェクトnodeElemensの参照を返す．
    *         ただし、変更不可とする．変更はこのオブジェクトの
    *         メンバ関数で行うこととする．
    */
    inline const std::shared_ptr<std::vector<std::shared_ptr<Node> > > getNodeElements() const {
        return nodeElements;
    }


    /**
     * @brief           任意の色でVerticesオブジェクトのNodeの色を更新
     * @param[color]    全てのNodeを更新する色(ex. RGB(0,0,0) )
     */
    void setColor(const std::string& color);
};

class LabelSingle :public LabelInterface {
private:
    std::string label;
    LabelSingle(const LabelSingle& ld) {}//コピーコンストラクタ
    LabelSingle operator=(const LabelSingle& ld) {}

public:
    /**
     * @brief           Labelクラスのコンストラクタ
     * @param[label]    ラベルで使用するラベル用の文字列
     */
    explicit inline LabelSingle (const std::string label) :
        label(label) {}
    ~LabelSingle() {}
    std::string getLabel() override;
    std::string getOutputLabel() override;
    void setLabel(const std::string label) override;
    inline LabelSingle* clone(const std::string label) override {
        return new LabelSingle(label);
    }
};

/**
 * @brief "project/sample"などの階層ラベルを扱うクラス
 */
class LabelDouble:public LabelInterface {
private:
    std::string label;
    std::string upLabel;
    std::string lwLabel;
    LabelDouble(const LabelDouble& ld) {}//コピーコンストラクタ
    LabelDouble& operator=(const LabelDouble& ld) {}

public:

    /**
     * @brief             LabelDoubleのコンストラクタ
     * @param[label]      初期化するためのラベル文字列
     * @detail            階層ラベルの作成
     */
    explicit LabelDouble(std::string label);
    ~LabelDouble() {}
    std::string getLabel() override;

    /**
     * @brief     上階層のラベルを取得
     * @return    上階層のラベル文字列
     */
    std::string getUpLabel();

    /**
     * @brief     下階層のラベルを取得
     * @return    下階層のラベル文字列
     */
    std::string getLwLabel();

    std::string getOutputLabel() override;
    void setLabel(const std::string label) override;
    inline LabelDouble* clone(const std::string label) override {
        return new LabelDouble(label);
    }

};



/**
* @brief 相関のある2つのノードの集合を扱うクラス
*/
class Edges
{
    const std::vector<std::pair<int, int>> mpair;
    std::set<int> mnodeset;

public:

    /**
     * @brief     Edgesクラスのコンストラクタ
     * @detail    .netファイルにおけるNodeの繋がりを示す領域部分
     */
    explicit Edges(const std::vector<std::pair<int, int>> mpair);

    /**
     * @brief  該当Nodeがモジュールを形成しているかどうか
     * @retval true Nodeがモジュールを形成している場合
     * @retval false Nodeがモジュールを形成していない場合
     */
    bool isModule(int index);

    /**
     * @brief メンバ変数mpairを返却
     */
    inline const std::vector<std::pair<int, int>>& getMpair() const {
        return mpair;
    }

    std::string getOutput()const ;

    /**
     * @brief copy constructor;
     */
};



/**
 * @detail 拡張子.net以外のファイルからPajekファイル（.net)
 *         を作成するクラス
 * @note   このオブジェクトを作成する際の第二引数は動的確保した
 *         pointerを渡すこと。
 */
class CreateFromText {
private:
    const fs::path infile; //.net以外の入力ファイル
    std::unique_ptr<LabelInterface> m_ilabel; //Label文字列のふるまいを規定
    std::function<std::shared_ptr<Node>(std::shared_ptr<Node>)> addproperty; //Nodeクラスのカスタマイズ

    /**
     * @brief コピーコンストラクタ
     *        メンバ変数にポインタが含まれており、また、複製に
     *        意味がないため禁止
     */
    CreateFromText(const CreateFromText& cft) = delete;

public:
    /**
     * @brief コンストラクタ
     * @param[infile] 入力ファイル名（パス）
     * @param[m_ilabel] Label文字列オブジェクト（必ず動的確保したリソースを受け取る）
     * @param[addproperty] Nodeオブジェクトに追加するプロパティ
     *                     追加しない場合はそのままのreturnで返す．
     */
    explicit inline CreateFromText(
        const fs::path infile,
        std::unique_ptr<LabelInterface> m_ilabel,
        std::function<std::shared_ptr<Node>(std::shared_ptr<Node>)> addproperty
    ):infile(infile),m_ilabel(std::move(m_ilabel)),addproperty(addproperty) {};


    //informat
    //Nodeのふるまい（ilabel)
    //outformatはPajekクラスオブジェクトにかく
    const std::vector<std::unique_ptr<Pajek> > run();
};

/**
 * @brief    Pajekファイルの要素(Vertices,Edges)を保持するオブジェクト
 * @note     部分オブジェクトに動的確保したメモリが存在するのでコピーコンストラクタ
 *           を利用して新たにメモリを動的に確保する必要がある。従って
 *           コンストラクタでは値渡しで受け取る．//明示的にコピーコンストラクタを
 *           呼び出して資源を確保する．（Verticesオブジェクトを複製して利用するため）
 *           
 */
class Pajek
{
    const std::string pajekLabel; /**< このオブジェクトの識別id ex.相関0.980の時の.netなど*/
    std::shared_ptr<Vertices> vtptr;
    std::unique_ptr<Edges> egsptr;

public:
    /**
     * @brief         Pajekオブジェクトのコンストラクタ
     * @param[vt]     全Nodeの定義の集合
     * @param[egs]    相関のある2つのNodeの集合
     */
    inline Pajek(
        std::string pajekLabel,
        std::shared_ptr<Vertices> vtptr,
        std::unique_ptr<Edges> egsptr
    ): pajekLabel(pajekLabel),vtptr(vtptr), egsptr(std::move(egsptr)){}

    inline std::string getPajekLbel() const {
        return pajekLabel;
    }

    /**
     *  @brief Pajekオブジェクトをnetファイルに書き出す
     *  @param[outfile] 出力ファイル名
     */
    void output(fs::path outfile) const ;


};
