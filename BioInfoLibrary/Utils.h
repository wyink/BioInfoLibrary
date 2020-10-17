#ifndef UTILS_H_
#define UTILS_H_


#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <functional>
#include <unordered_map>

namespace fs = std::filesystem;

/**
* @brief 汎用的に利用できる関数を集めたクラス
*/
class Utils
{
private:
    inline Utils() {};
public:
    /**
     * @brief       文字列を分割する
     * @param[line] 分割対象の文字列
     * @param[delimiter]  分割する際に利用するデリミタ
     * @return      分割した文字列の配列
     */
    static std::vector<std::string> split(
        const std::string& line, 
        const char* delimiter
    );

    static std::vector<std::string> split(
        const std::string& line,
        const char* delimiter,
        const int maxSplitCount
    );

    /**
     * @brief 固定長配列のsplitで配列を利用するため大きめのファイルに利用
     * @param[line] 分割対象の文字列
     * @param[delimiter] 分割する際に利用するデリミタ
     * @param[arr] 文字列を格納するための配列
     * @param[maxSplitCount] 最大分割数
     * 
     */
    static std::string* splitf(
        const std::string& line,
        const char* delimiter,
        std::string* arr,
        const int maxSplitCount
    );


    /**
     * @brief デリミタで区切られた2つのカラムからなるテキストファイル
     *        の左側をkey，右側をvalueとするmapに変換して返却する関数
     *        ただし、メンバ関数のfuncを利用することでkey,valueは自由
     *        に変更可能
     * @param[infile] 入力ファイル名（パス）
     * @param[del]    入力ファイル名（パス）に使用されているデリミタ
     * @param[func]   key,valueを自由に設定して返却するための関数
     * @return        作成されたmap
     */
    static std::unordered_map<std::string, std::string> keyValMakeFromFile(
        const fs::path& infile,
        const char* del,
        std::function<std::vector<std::string>&(std::vector<std::string>&)> func
    );

    /**
     * @brief   ファイルが存在するかどうか
     * @note    開けるかどうかも確認される
     */
    static bool isExistFile(const std::string& str) ;

};

#endif // !UTILS_H_