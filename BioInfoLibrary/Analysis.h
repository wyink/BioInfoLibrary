#ifndef ANALYSIS_H_
#define ANALYSIS_H_

#include <filesystem>
#include <iostream>
#include <fstream>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>
#include <set>
#include <map>

namespace fs = std::filesystem;


/*
* @brief fmeasure���v�Z����N���X�̓��o�͂��K�肷��C���^�t�F�[�X
*         
*/
class IFmeasure {
public:

	IFmeasure() = default;
	virtual ~IFmeasure() = default;

	/*
	* @brief ���̓t�@�C���Ɉˑ����鏈��
	* @details
	*        map[idA]={idA:count,idB:count,...} �̌`����
	*        �ϊ�����
	* @param[path] ���̓t�@�C��/���̓f�B���N�g��
	*/
	virtual std::unordered_map<std::string, std::unordered_map<std::string, int> > 
		informat(const fs::path& path) = 0;

	/*
	* @brief �o�͂ɗ��p����t�H�[�}�b�g�������K��
	* @param[fmeasureMap] ���ɂ�f-measure���v�Z�����g�����A�l�ɂ��̒l���i�[
	*                     
	* @param[out] �o�̓t�@�C����
	*/
	virtual void outformat(
		const std::map<std::string, float>& fmeasureMap,
		const fs::path out
	) = 0;

};

/**
* @details ���̓t�@�C���`���� �FstrainA
*                           �@>abc123.1 strainA:1,strainD:3,strainC:2
*                           �@>abc124.2 strainA:1,strainD:4
*                             ��L�̌`��4�t�@�C��(strainA~strainD)
* 
*        �o�̓t�@�C���`���� : strainA/strainB f-measure value
*/

class FmeasurePt1 :public IFmeasure{
public:
	/*
	* @brief ����id�Ǝ��g���܂ނ��ׂĂ�id�Ƃ̊֘A�l�̏W�v���@
	* @details
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
	enum class CountUpWay{
		Amount, //���ݗʂ����̂܂ܐώZ
		Exist   //���ݗʂ�1or0�ŐώZ
	};

	/**
	* @param[cuway] �W�v���@���I�u�W�F�N�g�������ɑI��
	*/
	FmeasurePt1(
		const CountUpWay cuway
	) :cuway(cuway){}

	/**
	* @details ���̓t�@�C�����ȉ��̎��ɑΉ�
	* 
	*  �E���ꂼ��̃t�@�C����id�ɑΉ����Ă���1�s�ڂɂ���id���̋L�q����
	*  �E�Y���t�@�C����id�Ƒ���id�ɑ΂���l�̋L�q����s���ƂɈȉ��̂悤
	*    �Ƀt�H�[�}�b�g����ďo�͂���Ă���D
	*  
	*  fileA ------------------------------------
	*  id_A
	*  >abc123.1 id_A:1,id_D:3,id_C:2 
	*  >abc123.1 id_A:1,id_C:3
	*  ...
	*  EOF
	* 
	*  fileB------------------------------------
	*  id_B
	*  >abc123.1 id_A:1,id_D:3,id_C:2
	*  ...
	*  EOF
	* 
	* id_C,id_D�����l
	*  -----------------------------------------
	* 
	* @param[indir] �S�Ă�id�̃t�@�C��(id_A~id_D)�����݂���f�B���N�g��
	* @return {id_A:{id_A:fma,�`id_D:fmd},...id_D:{id_A:fma,�`id_D:fmd}}
	*/
	std::unordered_map<std::string, std::unordered_map<std::string, int> > 
		informat(const fs::path& indir) override;

	/**
	* @brief idA,idB,f-measure value�ŏo��
	* @param[fmeasureMap] �^�O�ƑΉ�����f-measure�l�̘A�z�z��B�o�͂��邽�߁Amap���g�p
	* @param[outfile] �o�̓t�@�C���p�X
	*/
	void outformat(
		const std::map<std::string,float>& fmeasureMap,
		fs::path outfile
	) override;

private:
	const CountUpWay cuway;

};

/**
 * @brief f-measure�̌v�Z���ʂ𑗏o����N���X
 *        ���́C�o�͂̃t�H�[�}�b�g��IFmeasure�ŋK��
 */
class Fmeasure {
private:
	const fs::path indir;
	const fs::path outfile;
	std::unique_ptr<IFmeasure> fh;

	/**
	 * @brief f-measure�̌v�Z���s���irun�֐��Ŏ��s�����j
	 */
	const std::map<std::string, float> calcFmeasure(
		const std::unordered_map<std::string, std::unordered_map<std::string, int> >& idCounterMap
	);

public:
	explicit inline Fmeasure(
		const fs::path indir,
		const fs::path outfile,
		std::unique_ptr<IFmeasure> fh
	) :indir(indir), outfile(outfile), fh(std::move(fh)){}

	void setHandler(std::unique_ptr<IFmeasure> fh) {
		this->fh = std::move(fh);
	}

	void run();

};

#endif // !ANALYSIS_H_
