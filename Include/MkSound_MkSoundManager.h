#pragma once


//------------------------------------------------------------------------------------------------//
// [������ ��� ��]
//
// 0. ���α׷� �ʱ�ȭ �� ������ ����(�����ӿ�ũ �¾��� ���� ����)
//	ex>
//		m_InstanceDeallocator.RegisterInstance(new MkSoundManager());
//		MK_SOUND_MGR.SetUp(m_MainWindow.GetWindowHandle());
//
// 1. ������ ä�� �� ����
//	ex>
//		MkSoundChannel& bgChannel = MK_SOUND_MGR.GetChannel(L"BackgroundMusic");
//		bgChannel.SetMaxInterfaceSize(1); // ������� �ϳ��� ����
//
//		MkSoundChannel& effectChannel = MK_SOUND_MGR.GetChannel(L"Effect");
//		effectChannel.SetTimeGapAtSameSource(0.4);
//		effectChannel.SetMaxInterfaceSize(16);
//
// 2. ����� ����Ǵ� ������ �ε� Ÿ�Ӷ� �̸� �����͸� �о���⸦ ����
//	ex>
//		MK_SOUND_MGR.LoadSeedBuffer(L"[ȿ����] AHHHHH.WAV");
//
// 3. �ʿ� �� �� ���
//	ex>
//		MK_SOUND_MGR.GetChannel(L"BackgroundMusic").Play(L"���X���X.ogg", true);
//		...
//		MK_SOUND_MGR.GetChannel(L"Effect").Play(L"[ȿ����] AHHHHH.WAV");
//
// 4. �ʿ� �� �� ������ ����
//	ex>
//		MK_SOUND_MGR.GetChannel(L"Effect").Clear();
//		...
//		MK_SOUND_MGR.ClearAllPlayBuffer();
//		MK_SOUND_MGR.ClearAllSeedBuffer();
//------------------------------------------------------------------------------------------------//

#include <dsound.h>

#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkSingletonPattern.h"

#include "MkSound_MkSoundChannel.h" // sound manager�� �� ��� �����̹Ƿ� ���� ���� ���� ����

#define MK_SOUND_MGR MkSoundManager::Instance()


class MkSoundBuffer;

class MkSoundManager : public MkSingletonPattern<MkSoundManager>
{
public:

	// �ʱ�ȭ
	bool SetUp(HWND hWnd);

	// ���� ������ �ε�
	// �������� �ʾƵ� ��� ������ ������ �ε� Ÿ�Ӷ� �̸� �Ϸ��� ���⸦ ����
	bool LoadSeedBuffer(const MkHashStr& filePath);

	// ä�� ��ȯ. �ش� ä���� ������ ������ ��ȯ
	MkSoundChannel& GetChannel(const MkHashStr& channelName);

	// ������� ���� ����(�õ� ���۴� ����)
	void ClearAllPlayBuffer(void);

	// �õ� ���� ����. ���� ���� � ������ �ٽ� ��� ����� ������ �ش� ���� �õ� ���۴� �ٽ� �ε� ��
	void ClearAllSeedBuffer(void);

	// ���� ����
	void Clear(void);

	MkSoundManager() : MkSingletonPattern<MkSoundManager>() { m_DirectSound = NULL; }
	virtual ~MkSoundManager() { Clear(); }

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	unsigned int __CreatePlayBuffer(const MkHashStr& filePath); // bufferId ��ȯ(���н� MKDEF_EMPTY_SOUND_BUFFER_ID)
	MkSoundBuffer& __GetPlayBuffer(unsigned int bufferID);
	void __RemovePlayBuffer(unsigned int bufferID);

protected:

	void _UpdateSystemMessage(void) const;

protected:

	LPDIRECTSOUND m_DirectSound;

	// buffer management
	MkHashMap<MkHashStr, MkSoundBuffer*> m_SeedBuffers;
	MkMap<unsigned int, MkSoundBuffer*> m_PlayBuffers;
	unsigned int m_AutoIncrementalBufferIndex;

	// channel
	MkHashMap<MkHashStr, MkSoundChannel> m_Channels;
};
