BioInfoLibrary
=========================

BioInfoLibrary
Tools and library for bioinformatic analysis

[こちら](https://wyink.github.io/BioInfoLibDoc/annotated.html)にライブラリの内容をドキュメント形式で詳説しています．</br>
リンク先のドキュメントは[Doxygen](https://www.doxygen.nl/index.html)（version 1.8.20）で作成しました。ライブラリの内容については順次追加していく予定です。</br>

</br>

## Table of content
- [Pajek IO](#pajek-io)
- [Blast Analysis](#blast-analysis)


</br>

## Pajek IO
- [背景と目的](#background1)
- [参考情報](#supinfo1)
- [概要](#summary1)
    * [UML図による概要](#uml1)
    * [各クラス・インタフェースの役割](#role1)
	   - [ILabel](#ilabel)
	   - [LabelSingle](#labelsingle)
	   - [LabelDouble](#labeldouble)
	   - [Node](#node)
	   - [Vertices](#vertices)
	   - [Edges](#edges)
	   - [Pajek](#pajek)
	   - [PajekParser](#pajekparser)
	   - [CreateFromText](#createfromtext)

<a id="background1"></a>
### 背景と目的
[Pajek](http://mrvar.fdv.uni-lj.si/pajek/)は大規模なネットワークデータの分析や可視化に利用されるソフトウェアです。このソフトウェアが読み込むことができる形式は決まっており、また、その形式は独自のものです。従ってPajek IO では2つの関連するノードを1つのレコードとするファイルからPajek指定の形式に変換したり、もしくは出力されたPajekファイルをパースすることができます。

<a id="supinfo1"></a>
### 参考情報
Pajekファイルの構成例

```
*Vertices 3
1 "plantA"
2 "plantB"
3 "plantC"
*Edges
1 3
2 3

```

<a id="summary1"></a>
### 概要
<a id="uml1"></a>
#### 1. UML図による概要
Blastの出力ファイルを利用して行う解析で利用するクラス・インタフェース<sup>[1](#sup1)</sup>の関係性をUML図で示しています。
ここではデータと処理内容を具体的に示しています。実際は変数・メソッドであり、こちらに関してはドキュメントをご参照ください。

![PajekのUML図](https://github.com/wyink/BioInfoLibrary/blob/master/BioInfoLibrary/Picture/Pajek.png) </br>

</br>

<a id="role1"></a>
#### 2. 各クラス・インタフェース<sup>[1](#sup1)</sup>の役割

<a id="ilabel"></a>
#### ILabel [[Document]](https://wyink.github.io/BioInfoLibDoc/class_i_label.html) </br>
Pajekファイルの一意の文字列ラベルを示すインタフェース

<a id="labelsingle"></a>
#### LabelSingle [[Document]](https://wyink.github.io/BioInfoLibDoc/class_label_single.html) </br>
Pajekファイルの一意のラベルを扱うクラス

<a id="labeldouble"></a>
#### LabelDoubl [[Document]](https://wyink.github.io/BioInfoLibDoc/class_label_double.html) </br>
Pajekファイルの階層ラベル("upperGroup/lowerGroup")を扱うクラス

<a id="node"></a>
#### Node [[Document]](https://wyink.github.io/BioInfoLibDoc/class_node.html) </br>
PajekファイルのVerticesの一行分を構成するNodeに関するクラス

<a id="vertices"></a>
#### Vertices [[Document]](https://wyink.github.io/BioInfoLibDoc/class_vertices.html) </br>
Pajekの.netファイルを構成する各Nodeの定義

<a id="edges"></a>
#### Edges [[Document]](https://wyink.github.io/BioInfoLibDoc/class_edges.html) </br>
Pajekファイルの相関のある2つのノードの集合を扱うクラス

<a id="pajek"></a>
#### Pajek [[Document]](https://wyink.github.io/BioInfoLibDoc/class_pajek.html) </br>
Pajekファイルの要素(Vertices,Edges)を保持するオブジェクト

<a id="pajekparser"></a>
#### PajekParser [[Document]](https://wyink.github.io/BioInfoLibDoc/class_pajek_parser.html) </br>
Pajekアプリケーションで利用するファイルの解析

<a id="createfromtext"></a>
#### CreateFromText [[Document]](https://wyink.github.io/BioInfoLibDoc/class_create_from_text.html) </br>
拡張子.net以外のファイルからPajekファイル（.net) を作成するクラス

</br>




## Blast Analysis
- [背景](#background2)
- [目的](#purpose2)
- [参考情報](#supinfo2)
- [概要](#summary2)
    * [UML図による概要](#uml2)
    * [各クラス・インタフェースの役割](#role2)
	   - [IBlastParser](#iblastparser)
	   - [BlastParserPt1Imple](#blastparserpt1imple)
	   - [BlastParserPt2Imple](#blastparserpt2imple)
	   - [BlastParser](#blastparser)
	   
<a id="background2"></a>
### 背景
BLASTとはNational Center for Biotechnology Informationが提供しているBasic Local Alignment Search Toolの略語です。これを利用することでユーザーが指定した塩基配列もしくはアミノ酸配列とデータベース中の配列を比較し、有意に類似していると判定した塩基配列もしくはアミノ酸配列を導き出すことができます。様々な出力方法を指定することができますが、本プログラムでは結果をタブ区切りのテキストで出力した場合に対応しています。このようにして得られたデータには類似度のみではなく、どの配列領域が類似していると判定されたのか、類似度の妥当性などの情報が含まれています。加えて、その配列と紐づく分類群情報を利用することで解析の幅は広げることができます。例えばどの生物に由来するか不明な配列Aがこのツールを通して配列Bに最も有意に類似すると判定された場合、配列Bは既知であることから配列Aは配列Bと同じ生物種である可能性が高い、というような使い方です。この手法は一般的にDNAバーコーディングと呼ばれています。とりわけ大規模な入力データを利用する場合には本プログラムを利用することで解析を効率的に行うことができます。

<a id="purpose2"></a>
### 目的
BLAST出力ファイルを解析して分類群情報と紐づけを行う。（主にDNAバーコーディング向け）

<a id="supinfo2"></a>
### 参考情報
Blast出力ファイルの一例

|  Query ID |  Reference ID  |  Identity  |  Alignment length  |  Mismatches  |  Gap openings  |  Query Start |  Query End  |  Reference Start   |  Reference End  | Evalue | bit Score|
| :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
| AB000302.1 | AFJ96244.2 | 96.124 | 129 | 5 | 0 | 154 | 282 | 1 | 129 | 1.72e-54 | 211 |
| AB000302.1 | SCA40105.1 | 94.574 | 129 | 7 | 0 | 154 | 282 | 1 | 129 | 3.72e-51 | 200 |
| AB000302.1 | AAB01117.1 | 93.798 | 129 | 8 | 0 | 154 | 282 | 1 | 129 | 1.34e-50 | 198 |
| AB000302.1 | AAD00317.1 | 93.798 | 129 | 8 | 0 | 154 | 282 | 1 | 129 | 4.81e-50 | 196 |

<a id="summary2"></a>
### 概要
<a id="uml2"></a>
#### 1. UML図による概要
Blastの出力ファイルを利用して行う解析で利用するクラス・インタフェース<sup>[1](#sup1)</sup>の関係性をUML図で示しています。
ここではデータと処理内容を具体的に示しています。実際は変数・メソッドであり、こちらに関してはドキュメントをご参照ください。

![BlastParserのUML図](https://github.com/wyink/BioInfoLibrary/blob/master/BioInfoLibrary/Picture/BlastParser.png) </br>

</br>

<a id="role2"></a>
#### 2. クラス・インタフェース<sup>[1](#sup1)</sup>の役割
</br>

<a id="iblastparser"></a>
#### IBlastParser   [[Document]](https://wyink.github.io/BioInfoLibDoc/class_i_blast_parser.html) </br>
BlastParserPt1ImpleとBlastParPt2Impleのインタフェース<sup>[1](#sup1)</sup>であり、結果データの具体的な処理内容を規定します。
 
<a id="blastparserpt1imple"></a>
#### BlastParserPt1Imple [[Document]](https://wyink.github.io/BioInfoLibDoc/class_blast_parser_pt1_imple.html) </br>
Blast結果ファイルのクエリーと参照およびその参照が属するtaxidを対応付けます。[[詳解]](https://wyink.github.io/BioInfoLibDoc/class_blast_parser_pt1_imple.html#details)

<a id="blastparserpt2imple"></a>
#### BlastParserPt2Imple [[Document]](https://wyink.github.io/BioInfoLibDoc/class_blast_parser_pt2_imple.html) </br>
各クエリに対してヒットした参照のスコア（群）を対応付けます。[[詳解]](https://wyink.github.io/BioInfoLibDoc/class_blast_parser_pt2_imple.html#details)

<a id="blastparser"></a>
#### BlastParser [[Document]](https://wyink.github.io/BioInfoLibDoc/class_blast_parser.html) </br>
IBlastParserクラスを実現したBlastParserPt1ImpleもしくはBlastParserPt2Impleをハンドラとして必要とします。Blast出力ファイルの解析処理はこのハンドラに依存します。このクラスはBlast出力ファイルの入力、解析結果の出力フォーマット、およびその出力に関する基本的なメソッドを保持しています。

</br>
<a id="sup1"></a>

* * *

脚注
1 : C++ではインタフェースという機能はありませんが、純粋仮想関数のみで構成されるクラスを「インタフェース」と呼ぶこととします。
