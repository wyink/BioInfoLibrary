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

### 背景
**BLAST出力ファイルを解析することで特定の用途向けのデータ抽出を可能とする**</br>
BLASTとはNational Center for Biotecnology Informationが提供しているBasic Local Alignment Search Toolの略語です。これはユーザーが指定した塩基配列もしくはアミノ酸配列とデータベース中の配列を比較し、有意に類似していると判定した塩基配列もしくはアミノ酸配列を導き出すことができます。様々な出力方法を指定することができますが、本プログラムでは結果をタブ区切りのテキストで出力した場合についてのみ扱います。このようにして得られたデータには類似度のみではなく、どの配列領域が類似していると判定されたのか、類似度の妥当性などの情報が含まれています。また、その配列と紐づく分類群情報を利用することで解析の幅は広がります。例えば自身が有する配列Aは配列Bに最も有意に類似すると判定されたが、この配列BはB'属のB''種であることから配列AはB'属のB''種である可能性が高い、というような使い方です。これはDNAバーコーディングと呼ばれる手法です。大規模な出力データを利用する場合に以下で示すようなプログラムを利用して解析することが可能です。

**Blast出力ファイルの一例**

|  Query ID |  Reference ID  |  Identity  |  Alignment length  |  Mismatches  |  Gap openings  |  Query Start |  Query End  |  Reference Start   |  Reference End  | Evalue | bit Score|
| :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
| AB000302.1 | AFJ96244.2 | 96.124 | 129 | 5 | 0 | 154 | 282 | 1 | 129 | 1.72e-54 | 211 |
| AB000302.1 | SCA40105.1 | 94.574 | 129 | 7 | 0 | 154 | 282 | 1 | 129 | 3.72e-51 | 200 |
| AB000302.1 | AAB01117.1 | 93.798 | 129 | 8 | 0 | 154 | 282 | 1 | 129 | 1.34e-50 | 198 |
| AB000302.1 | AAD00317.1 | 93.798 | 129 | 8 | 0 | 154 | 282 | 1 | 129 | 4.81e-50 | 196 |

### 概要

1. UML図
2. クラス・インタフェースの役割
   - [BlastParser](#blastparser)
   - [BlastParserPt1Imple](#blastparserpt1imple)
   - [BlastParserPt2Imple](#blastparserpt2imple)


![BlastParserのUML図](https://github.com/wyink/BioInfoLibrary/blob/master/BioInfoLibrary/Picture/BlastParser.png)

#### BlastParser
[Documentはこちら](https://wyink.github.io/BioInfoLibDoc/class_blast_parser.html) </br>

#### BlastParserPt1Imple
[Documentはこちら](https://wyink.github.io/BioInfoLibDoc/class_blast_parser_pt1_imple.html) </br>

Blast結果ファイルのクエリーと参照およびその参照が属するのtaxidが以下に示すような組合せである場合について考える。
#### 入力例
Blast結果ファイルを抜粋してテーブルで表示
|  Query   |  Reference  | taxid  |									
| :------: | :---------: | -----: |									
| QueryA   |	RefA     | 123	  |									
| QueryA   |	RefA     | 123	  |									
| QueryA   |	RefB     | 234	  |									
| QueryA   |	RefD     | 123	  |									

- QueryA,QueryBをそれぞれ対応するtaxidに変換						
-  同じid(RefA)は2度登場するが、このような重複はカウントしない		
- RefA,RefDは属するtaxidが同じであるためカウントアップ				
- 変換後のtaxidの数も出力する。									
											

#### 出力例
QueryAは「2」種類のtaxid(123,234)のどれかに所属する可能性が示されている。
注意点として、QueryAは123に属する2つの異なる配列にトップヒットしている。

```													
QueryA 2	                  														
123:2,234:1																				
```

#### BlastParserPt2Imple
[Documentはこちら](https://wyink.github.io/BioInfoLibDoc/class_blast_parser_pt2_imple.html) </br>
各クエリに対してヒットした参照のスコア群を保持する。しかし、同一クエリに対して「同一の参照に複数ヒット」かつ「そのアライメント領域が重複しない場合」は加算したスコアを保持して各クエリが保持するスコア群を一定値で区切って出力する。以下にその例を示す。
 

#### 入力例

同一クエリ・同一参照に対して参照のアライメントが重複しない場合
|  Query   | Reference   | RefAlignStart | RefAlignEnd | Score |
| :------: | :---------: | ------------: | ----------: |  ---: |
| QueryA   | Reference1  | 	1            | 150	       |   190 |
| QueryA   | Reference1  | 	160          | 180	       |    30 |
| QueryA   | Reference2  | 	1            | 190	       |   300 |
| QueryB   | ...         | ...	         | ...	       |   ... |

#### 出力例

重複しないため190と30を加算して出力
  | score  | 100 | 200 | 300 | 400 | 500 | ... | n\*100 |
  | :---:  | --: | --: | --: | --: | --: | --: | ----: |
  | QueryA |   0 |   1 |  1  |  0  | 0   |  0  | 	0  |
  | QueryB | ... | ... | ... | ... | ... | ... |   ... |
 

