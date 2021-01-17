BioInfoLibrary
=========================

BioInfoLibrary
Tools and library for bioinformatic analysis

[こちら](https://wyink.github.io/BioInfoLibDoc/annotated.html)にライブラリの内容をドキュメント形式で詳説しています．
内容については順次、更新していく予定です。

## CONTENT
- [Blast Analysys](#blast-analysys)

* * *

## Blast Analysys
**BLAST出力ファイルを解析することで特定の用途向けのデータ抽出を可能とする**</br>
BLASTとはNational Center for Biotecnology Information　が提供しているBasic Local Alignment Search Toolの略語である。これはユーザーが指定した塩基配列もしくはアミノ酸配列とデータベース中の配列を比較し、有意に類似していると判定した塩基配列もしくはアミノ酸配列を導き出すことができる。様々な出力方法があるが、ここではタブ区切りのテキスト出力ファイルについて行う。このようにして得られたデータには類似度のみではなく、どの配列領域が類似していると判定されたのか、類似度の妥当性などの情報が含まれている。また、その配列と紐づく分類群情報を利用することで解析の幅は広がる。例えば自身が有する配列Aは配列Bに最も有意に類似すると判定されたが、この配列BはB'属のB''種であることから配列AはB'属のB''種である可能性が高い、というような使い方である。これはDNAバーコーディングと呼ばれる手法である。大規模な出力データを利用する場合に以下で示すようなプログラムを利用して解析することが可能である。

**Blast出力ファイルの一例**

|  Query ID |  Reference ID  |  Identity  |  Alignment length  |  Mismatches  |  Gap openings  |  Query Start |  Query End  |  Reference Start   |  Reference End  | Evalue | bit Score|
| :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
| AB000302.1 | AFJ96244.2 | 96.124 | 129 | 5 | 0 | 154 | 282 | 1 | 129 | 1.72e-54 | 211 |
| AB000302.1 | SCA40105.1 | 94.574 | 129 | 7 | 0 | 154 | 282 | 1 | 129 | 3.72e-51 | 200 |
| AB000302.1 | AAB01117.1 | 93.798 | 129 | 8 | 0 | 154 | 282 | 1 | 129 | 1.34e-50 | 198 |
| AB000302.1 | AAD00317.1 | 93.798 | 129 | 8 | 0 | 154 | 282 | 1 | 129 | 4.81e-50 | 196 |

* * * 
![BlastParserのUML図](https://github.com/wyink/BioInfoLibrary/blob/master/BioInfoLibrary/Picture/BlastParser.png)


