//#pragma once
//#include "cri_adx2le.h"
//#include <map>
//#include <vector>
//
//class SoundManager
//{
//private:
//	CriAtomExVoicePoolHn voice_pool;	// �{�C�X�v�[���n���h��
//	CriAtomExPlayerHn player_bgm;		//�v���C���[�n���h��(BGM)
//	CriAtomExPlaybackId playbackId_bgm;	//�Đ����ꂽ�v���C���[��ID(BGM)
//	CriAtomExPlayerHn player_se;		//�v���C���[�n���h��(SE)
//	CriAtomExPlaybackId playbackId_se;	//�Đ����ꂽ�v���C���[��ID(SE)
//	std::vector<CriAtomExAcbHn> acb_hn;	//ACB��AWB�̃n���h��
//	std::map<const CriChar8*, CriAtomExAcbHn> soundIndex;
//	std::map<const CriChar8*, CriAtomExAcbHn>::iterator soundIt;	//�T�E���h�C���f�b�N�X�̃C�e���[�^�[
//	CriFloat32 volumeBGM;	//����(BGM)
//	CriFloat32 volumeSE;	//����(SE)
//	SoundManager();
//	~SoundManager();
//
//	//�L���[ID����ACB�̒��ɂ���T�E���h�����擾
//	const char* GetQueueName(CriAtomExAcbHn acb_hn, int queueID);
//	//�K�������T�E���h������Ή����������C�e���[�^�[�ɓn���Ď擾
//	std::map<const CriChar8*, CriAtomExAcbHn>::iterator SoundIterator(const char* name);
//public:
//	static SoundManager & GetInstance() {
//		static SoundManager Instance;
//		return Instance;
//	}
//	//������
//	void Initialize(const char* acfPath, const char* acbPath, const char* awcPath);
//	//�I��
//	void Finalize();
//	//�V����ACB��AWB��ǂݍ���
//	void LoadSound(const char* acbPath, const char* awcPath);
//
//	void PlayBGM(const char* name, CriSint32 fadeInTime, CriSint32 fadeOutTime);
//	void PlaySE(const char* name, CriSint32 fadeInTime, CriSint32 fadeOutTime);
//
//	int GetStatePlayerBGM();
//	int GetStatePlayerSE();
//
//	void PauseBGM(CriBool sw);
//	void PauseSE(CriBool sw);
//	void PauseAll(CriBool sw);
//
//	void SetVolumeBGM(CriFloat32 volume);
//	void SetVolumeSE(CriFloat32 volume);
//
//	float GetVolumeBGM();
//	float GetVolumeSE();
//
//	void MuteBGM(CriBool sw);
//	void MuteSE(CriBool sw);
//	void MuteALL(CriBool sw);
//
//	CriAtomExPlayerHn GetPlayerBGM();
//	CriAtomExPlayerHn GetPlayerSE();
//
//	void DeleteSound(const char* name);
//	void ExecuteMain();
//};

/*
Unload�@ACB�̉��
�񐔎w��
�w�肵���ʒu���烋�[�v
�RD�ړ�
�������ƃJ�����̋��������o���ĉ����o��
Fadein
*/

