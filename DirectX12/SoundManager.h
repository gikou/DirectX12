//#pragma once
//#include "cri_adx2le.h"
//#include <map>
//#include <vector>
//
//class SoundManager
//{
//private:
//	CriAtomExVoicePoolHn voice_pool;	// ボイスプールハンドル
//	CriAtomExPlayerHn player_bgm;		//プレイヤーハンドル(BGM)
//	CriAtomExPlaybackId playbackId_bgm;	//再生されたプレイヤーのID(BGM)
//	CriAtomExPlayerHn player_se;		//プレイヤーハンドル(SE)
//	CriAtomExPlaybackId playbackId_se;	//再生されたプレイヤーのID(SE)
//	std::vector<CriAtomExAcbHn> acb_hn;	//ACBとAWBのハンドル
//	std::map<const CriChar8*, CriAtomExAcbHn> soundIndex;
//	std::map<const CriChar8*, CriAtomExAcbHn>::iterator soundIt;	//サウンドインデックスのイテレーター
//	CriFloat32 volumeBGM;	//音量(BGM)
//	CriFloat32 volumeSE;	//音量(SE)
//	SoundManager();
//	~SoundManager();
//
//	//キューIDからACBの中にあるサウンド名を取得
//	const char* GetQueueName(CriAtomExAcbHn acb_hn, int queueID);
//	//適応したサウンド名から対応した情報をイテレーターに渡して取得
//	std::map<const CriChar8*, CriAtomExAcbHn>::iterator SoundIterator(const char* name);
//public:
//	static SoundManager & GetInstance() {
//		static SoundManager Instance;
//		return Instance;
//	}
//	//初期化
//	void Initialize(const char* acfPath, const char* acbPath, const char* awcPath);
//	//終了
//	void Finalize();
//	//新しいACBとAWBを読み込む
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
Unload　ACBの解放
回数指定
指定した位置からループ
３D移動
発生源とカメラの距離を検出して音を出す
Fadein
*/

