//#include "SoundManager.h"
//
//static void user_error_callback_func(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray)
//{
//	const CriChar8 *errmsg;
//	// �G���[�R�[�h����G���[������ɕϊ����ăR���\�[���o�͂��� 
//	errmsg = criErr_ConvertIdToMessage(errid, p1, p2);
//	OutputDebugString(TEXT("%s", errmsg));
//	return;
//}
//// �������m�ۊ֐��̃��[�U���� 
//void *user_alloc_func(void *obj, CriUint32 size)
//{
//	void *ptr;
//	ptr = malloc(size);
//	return ptr;
//}
//// ����������֐��̃��[�U���� 
//void user_free_func(void *obj, void *ptr)
//{
//	free(ptr);
//}
//
//std::map<const CriChar8*, CriAtomExAcbHn>::iterator
//SoundManager::SoundIterator(const char* name) {
//	std::map<const CriChar8*, CriAtomExAcbHn>::iterator ite;
//	for (int h = 0; h < acb_hn.size(); h++) {
//
//		ite = soundIndex.find(GetQueueName(acb_hn[h], criAtomExAcb_GetCueIdByName(acb_hn[h], name)));
//
//		if (ite != soundIndex.end()) {
//			break;
//		}
//	}
//	return ite;
//}
//
//SoundManager::SoundManager() {
//	voice_pool = nullptr;   // �{�C�X�v�[���n���h�� 
//	player_bgm = nullptr;
//	player_se = nullptr;	//�v���C���[�n���h��
//	playbackId_bgm = 0;
//	playbackId_se = 0;
//	volumeBGM = 1.0f;
//	volumeSE = 1.0f;
//}
//
//SoundManager::~SoundManager()
//{
//}
//
//void
//SoundManager::Initialize(const char* acfPath, const char* acbPath, const char* awcPath) {
//
//
//	// �G���[�R�[���o�b�N�֐��̓o�^
//	criErr_SetCallback(user_error_callback_func);
//
//	// �������A���P�[�^�̓o�^ 
//	criAtomEx_SetUserAllocator(user_alloc_func, user_free_func, nullptr);
//
//	// ���C�u���������� 
//	criAtomEx_Initialize_WASAPI(nullptr, nullptr, 0);
//
//	// �X�g���[�~���O�p�o�b�t�@�̍쐬 
//	CriAtomDbasId dbas;	// D-BAS�n���h�� 
//	dbas = criAtomDbas_Create(nullptr, nullptr, 0);
//
//	// �S�̐ݒ�t�@�C���̓o�^ 
//	CriBool criresult;
//	criresult = criAtomEx_RegisterAcfFile(nullptr, acfPath, nullptr, 0);
//
//	// DSP�o�X�ݒ�̓o�^ 
//	criAtomEx_AttachDspBusSetting("DspBusSetting_0", nullptr, 0);
//
//	// �{�C�X�v�[���̍쐬�i�X�g���[���Đ��Ή��j 
//	CriAtomExStandardVoicePoolConfig vpconfig;
//	criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&vpconfig);
//	vpconfig.player_config.streaming_flag = CRI_TRUE;
//	voice_pool = criAtomExVoicePool_AllocateStandardVoicePool(&vpconfig, nullptr, 0);
//
//	// �T�E���h�f�[�^�̓ǂݍ��� 
//	acb_hn.push_back(criAtomExAcb_LoadAcbFile(nullptr, acbPath, nullptr, awcPath, nullptr, 0));
//	/*if (acb_hn == nullptr) {
//	MessageBox(nullptr, TEXT("The path is incorrect"), TEXT("Error"), MB_OK);
//	}*/
//	// �v���[���n���h���̍쐬 
//	criAtomExPlayer_AttachFader(player_bgm, nullptr, nullptr, 0);
//	criAtomExPlayer_AttachFader(player_se, nullptr, nullptr, 0);
//	player_se = criAtomExPlayer_Create(nullptr, nullptr, 0);
//	player_bgm = criAtomExPlayer_Create(nullptr, nullptr, 0);
//	for (int h = 0; h < acb_hn.size(); h++) {
//		for (int i = 0; i < criAtomExAcb_GetNumCues(acb_hn[h]); i++) {
//			soundIndex[GetQueueName(acb_hn[h], i)] = acb_hn[h];
//		}
//	}
//}
//
//void
//SoundManager::LoadSound(const char* acbPath, const char* awcPath) {
//	acb_hn.push_back(criAtomExAcb_LoadAcbFile(nullptr, acbPath, nullptr, awcPath, nullptr, 0));
//	for (int h = 0; h < acb_hn.size(); h++) {
//		for (int i = 0; i < criAtomExAcb_GetNumCues(acb_hn[h]); i++) {
//			soundIndex[GetQueueName(acb_hn[h], i)] = acb_hn[h];
//		}
//	}
//}
//
//void
//SoundManager::PlayBGM(const char* name, CriSint32 fadeInTime, CriSint32 fadeOutTime) {
//	soundIt = SoundIterator(name);
//
//	if (soundIt == soundIndex.end()) {
//		MessageBox(nullptr, TEXT("No Sound"), TEXT("Error"), MB_OK);
//	}
//	else
//	{
//		// �L���[�Đ��ƃv���C�o�b�NID�̋L�^ 
//		criAtomExPlayer_SetFadeInTime(player_bgm, fadeInTime);
//		criAtomExPlayer_SetFadeInTime(player_bgm, fadeOutTime);
//		criAtomExPlayer_SetCueName(player_bgm, soundIt->second, name);
//		playbackId_bgm = criAtomExPlayer_Start(player_bgm);
//		// �Đ��I���̔��� 
//		if (criAtomExPlayback_GetStatus(playbackId_bgm) == CRIATOMEXPLAYBACK_STATUS_REMOVED) {
//		}
//	}
//
//}
//
//void
//SoundManager::PlaySE(const char* name, CriSint32 fadeInTime, CriSint32 fadeOutTime) {
//
//	soundIt = SoundIterator(name);
//
//	if (soundIt == soundIndex.end()) {
//		MessageBox(nullptr, TEXT("No Sound"), TEXT("Error"), MB_OK);
//	}
//	else
//	{
//		// �L���[�Đ��ƃv���C�o�b�NID�̋L�^ 
//		criAtomExPlayer_SetFadeInTime(player_se, fadeInTime);
//		criAtomExPlayer_SetFadeInTime(player_se, fadeOutTime);
//		criAtomExPlayer_SetCueName(player_se, soundIt->second, name);
//		playbackId_se = criAtomExPlayer_Start(player_se);
//		// �Đ��I���̔��� 
//		if (criAtomExPlayback_GetStatus(playbackId_se) == CRIATOMEXPLAYBACK_STATUS_REMOVED) {
//		}
//	}
//
//}
//
//int
//SoundManager::GetStatePlayerBGM() {
//	return criAtomExPlayer_GetStatus(player_bgm);
//}
//
//int
//SoundManager::GetStatePlayerSE() {
//	return criAtomExPlayer_GetStatus(player_se);
//}
//
//void
//SoundManager::PauseBGM(CriBool sw) {
//
//	criAtomExPlayer_Pause(player_bgm, sw);
//
//}
//
//void
//SoundManager::PauseSE(CriBool sw) {
//
//	criAtomExPlayer_Pause(player_se, sw);
//
//}
//
//void
//SoundManager::PauseAll(CriBool sw) {
//	criAtomExPlayer_Pause(player_bgm, sw);
//	criAtomExPlayer_Pause(player_se, sw);
//}
//
//void
//SoundManager::SetVolumeBGM(CriFloat32 volume) {
//	volumeBGM = volume;
//	criAtomExPlayer_SetVolume(player_bgm, volumeBGM);
//	criAtomExPlayer_Update(player_bgm, playbackId_bgm);
//}
//
//void
//SoundManager::SetVolumeSE(CriFloat32 volume) {
//	volumeSE = volume;
//	criAtomExPlayer_SetVolume(player_se, volume);
//	criAtomExPlayer_Update(player_se, playbackId_se);
//}
//
//CriAtomExPlayerHn
//SoundManager::GetPlayerBGM() {
//	return player_bgm;
//}
//
//
//CriAtomExPlayerHn
//SoundManager::GetPlayerSE() {
//	return player_se;
//}
//
//
//float
//SoundManager::GetVolumeBGM() {
//	return volumeBGM;
//}
//
//float
//SoundManager::GetVolumeSE() {
//	return volumeSE;
//}
//
//void
//SoundManager::MuteBGM(CriBool sw) {
//	if (sw == true) {
//		criAtomExPlayer_SetVolume(player_bgm, volumeBGM);
//		criAtomExPlayer_UpdateAll(player_bgm);
//	}
//	else {
//		criAtomExPlayer_SetVolume(player_bgm, 0.0);
//		criAtomExPlayer_UpdateAll(player_bgm);
//	}
//}
//
//void
//SoundManager::MuteSE(CriBool sw) {
//	if (sw == true) {
//		criAtomExPlayer_SetVolume(player_se, volumeBGM);
//		criAtomExPlayer_UpdateAll(player_se);
//	}
//	else {
//		criAtomExPlayer_SetVolume(player_se, 0.0);
//		criAtomExPlayer_UpdateAll(player_se);
//	}
//}
//
//void
//SoundManager::MuteALL(CriBool sw) {
//	if (sw == true) {
//		criAtomExPlayer_SetVolume(player_bgm, volumeBGM);
//		criAtomExPlayer_UpdateAll(player_bgm);
//		criAtomExPlayer_SetVolume(player_se, volumeSE);
//		criAtomExPlayer_UpdateAll(player_se);
//	}
//	else {
//		criAtomExPlayer_SetVolume(player_bgm, 0.0);
//		criAtomExPlayer_UpdateAll(player_bgm);
//		criAtomExPlayer_SetVolume(player_se, 0.0);
//		criAtomExPlayer_UpdateAll(player_se);
//	}
//}
//
//void
//SoundManager::DeleteSound(const char* name) {
//	for (int h = 0; h < acb_hn.size(); h++) {
//		soundIndex.erase(GetQueueName(acb_hn[h], criAtomExAcb_GetCueIdByName(acb_hn[h], name)));
//	}
//}
//
//void
//SoundManager::ExecuteMain() {
//	criAtomEx_ExecuteMain();
//}
//
//const char*
//SoundManager::GetQueueName(CriAtomExAcbHn acb_hn, int queueID) {
//	return criAtomExAcb_GetCueNameById(acb_hn, queueID);
//}
//
//void
//SoundManager::Finalize() {
//	// �Đ����̃T�E���h��~ 
//
//	// �v���[���n���h���̔j�� 
//	criAtomExPlayer_Destroy(player_bgm);
//	criAtomExPlayer_Destroy(player_se);
//
//	// ACB�n���h���̉�� 
//	criAtomExAcb_ReleaseAll();
//
//	// �{�C�X�v�[���̉�� 
//	criAtomExVoicePool_Free(voice_pool);
//
//	// �S�̐ݒ�t�@�C���̓o�^���� 
//	criAtomEx_UnregisterAcf();
//
//	// ���C�u�����̏I�� 
//	criAtomEx_Finalize_WASAPI();
//
//}