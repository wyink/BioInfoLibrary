#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

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
     * @param[sep]  分割する際に利用するデリミタ
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
     * @brief   ファイルが存在するかどうか
     * @note    開けるかどうかも確認される
     */
    static bool isExistFile(const std::string& str) ;

};

