#pragma once


//------------------------------------------------------------------------------------------------//
// [간단한 사용 법]
//
// 0. 프로그램 초기화 중 적절히 선언(프레임워크 셋업에 삽입 권장)
//	ex>
//		m_InstanceDeallocator.RegisterInstance(new MkSoundManager());
//		MK_SOUND_MGR.SetUp(m_MainWindow.GetWindowHandle());
//
// 1. 적절한 채널 별 설정
//	ex>
//		MkSoundChannel& bgChannel = MK_SOUND_MGR.GetChannel(L"BackgroundMusic");
//		bgChannel.SetMaxInterfaceSize(1); // 배경음은 하나로 족함
//
//		MkSoundChannel& effectChannel = MK_SOUND_MGR.GetChannel(L"Effect");
//		effectChannel.SetTimeGapAtSameSource(0.4);
//		effectChannel.SetMaxInterfaceSize(16);
//
// 2. 재생이 예상되는 음원은 로딩 타임때 미리 데이터를 읽어놓기를 권장
//	ex>
//		MK_SOUND_MGR.LoadSeedBuffer(L"[효과음] AHHHHH.WAV");
//
// 3. 필요 할 때 재생
//	ex>
//		MK_SOUND_MGR.GetChannel(L"BackgroundMusic").Play(L"으쌰으쌰.ogg", true);
//		...
//		MK_SOUND_MGR.GetChannel(L"Effect").Play(L"[효과음] AHHHHH.WAV");
//
// 4. 필요 할 때 적절히 해제
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

#include "MkSound_MkSoundChannel.h" // sound manager의 주 사용 목적이므로 전방 선언 하지 않음

#define MK_SOUND_MGR MkSoundManager::Instance()


class MkSoundBuffer;

class MkSoundManager : public MkSingletonPattern<MkSoundManager>
{
public:

	// 초기화
	bool SetUp(HWND hWnd);

	// 음원 데이터 로딩
	// 실행하지 않아도 상관 없지만 가급적 로딩 타임때 미리 완료해 놓기를 권장
	bool LoadSeedBuffer(const MkHashStr& filePath);

	// 채널 반환. 해당 채널이 없으면 생성해 반환
	MkSoundChannel& GetChannel(const MkHashStr& channelName);

	// 재생중인 음원 삭제(시드 버퍼는 무관)
	void ClearAllPlayBuffer(void);

	// 시드 버퍼 삭제. 만약 이후 어떤 음원에 다시 재생 명령이 들어오면 해당 음원 시드 버퍼는 다시 로딩 됨
	void ClearAllSeedBuffer(void);

	// 완전 종료
	void Clear(void);

	MkSoundManager() : MkSingletonPattern<MkSoundManager>() { m_DirectSound = NULL; }
	virtual ~MkSoundManager() { Clear(); }

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	unsigned int __CreatePlayBuffer(const MkHashStr& filePath); // bufferId 반환(실패시 MKDEF_EMPTY_SOUND_BUFFER_ID)
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
