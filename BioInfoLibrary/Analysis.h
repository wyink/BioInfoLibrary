#pragma once
#include <filesystem>
#include <iostream>
#include <fstream>
#include <functional>
#include <vector>
#include <string>
#include <set>
#include <map>

namespace fs = std::filesystem;


/*
* @detail fmeasure���v�Z����N���X�̃C���^�t�F�[�X
*         
*/
class FmeasureHandler {
public:

	FmeasureHandler() = default;
	virtual ~FmeasureHandler() = default;

	/*
	* @brief ���̓t�@�C���Ɉˑ����鏈��
	*        map[idA]={idA:count,idB:count,...} �̌`����
	*        �ϊ�����
	* @param[infile] ���̓t�@�C��
	*/
	virtual std::map<std::string, std::map<std::string, int> > 
		strategy(const fs::path& indir) = 0;

	/*
	* @brief �o�͂ɗ��p����t�H�[�}�b�g�������K��
	* @param[fmeasureMap] ���ɂ�f-measure���v�Z�����g�����C
	*                     �l�ɂ��̒l���i�[
	* @param[out] �o�̓t�@�C����
	*/
	virtual void outformat(
		const std::map<std::string, float>& fmeasureMap,
		const fs::path out
	) = 0;

};

/**
* @brief ���̓t�@�C���`���� �FstrainA
*                           �@>abc123.1 strainA:1,strainD:3,strainC:2
*                           �@>abc124.2 strainA:1,strainD:4
*                             ��L�̌`��4�t�@�C��(strainA~strainD)
* 
*        �o�̓t�@�C���`���� : strainA\tstrainB f-measure value
*/
class FmeasurePt1 :public FmeasureHandler{
public:
	/*
	* @brief ����id�Ǝ��g���܂ނ��ׂĂ�id�Ƃ̊֘A�l�̏W�v���@
	*        example:
	*        
	*        strainA,strainB,strainC,straindD��4��id������
	*        -----------------------------------------------
	*        strainA
	*        >abc123.1 strainA:1,strainD:3,strainC:2
	*        >abc124.2 strainA:1,strainD:4
	*  
	*        EOF
	*        -----------------------------------------------
	* 
	*       �W�v���@�FAmount
	*       strainA-strainA 2  //1+1
	*       strainA-strainB 0  //0+0
	*       strainA-strainC 2  //2+0
	*       strainA-strainD 7  //3+4
	* 
	*       �W�v���@:Exist
	*       strainA-strainA 2  //1+1
	*       strainA-strainB 0  //0+0
	*       strainA-strainC 1  //1+0
	*       strainA-strainD 2  //1+1
	*/
	enum CountUpWay {
		Amount = 0, //���ݗʂ����̂܂ܐώZ
		Exist = 1  //���ݗʂ�1or0�ŐώZ
	};

	/**
	* @param[cuway] �W�v���@���I�u�W�F�N�g�������ɑI��
	*/
	inline FmeasurePt1(
		const CountUpWay cuway
	):cuway(cuway){}

	/**
	* @detail ���̓t�@�C�����ȉ��̎��ɑΉ�
	* 
	*  �E���ꂼ��̃t�@�C����id�ɑΉ����Ă���1�s�ڂɂ���id���̋L�q����
	*  �E�Y���t�@�C����id�Ƒ���id�ɑ΂���l�̋L�q����s���ƂɈȉ��̂悤
	*    �Ƀt�H�[�}�b�g����ďo�͂���Ă���D
	*         
	*  id_A
	*  >abc123.1 strainA:1,strainD:3,strainC:2 
	*  >abc123.1 strainA:1,strainC:3
	*  ...
	* 
	*  id_B
	*  >abc123.1 strainA:1,strainD:3,strainC:2
	*  ....
	*  -----------------------------------------
	* 
	* @param[indir] 
	* @return 
	*/
	std::map<std::string, std::map<std::string, int> > 
		informat(const fs::path& indir) override;

	/**
	* @brief idA\tidB f-measure value�ŏo��
	*/
	void outformat(
		const std::map<std::string,float>& fmeasureMap,
		fs::path out
	) override;

private:
	const CountUpWay cuway;


};

class Fmeasure {
private:
	const fs::path& indir	;
	const fs::path& outfile;
	FmeasureHandler& fh;

public:
	explicit inline Fmeasure(
		const fs::path indir,
		const fs::path outfile,
		FmeasureHandler& fh
	) :indir(indir),outfile(outfile),fh(fh) {}

	void setHandler(FmeasureHandler& fh) {
		this->fh = fh;
	}

	void run();
};