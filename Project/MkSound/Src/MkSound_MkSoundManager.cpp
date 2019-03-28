
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDevPanel.h"

#include "MkSound_MkSoundBuffer.h"
#include "MkSound_MkSoundInterface.h" // MKDEF_EMPTY_SOUND_BUFFER_ID
#include "MkSound_MkSoundManager.h"

#pragma comment(lib, "dsound")
#pragma comment(lib, "winmm")
#pragma comment(lib, "ogg")
#pragma comment(lib, "vorbis")
#pragma comment(lib, "vorbisfile")


#define MKDEF_SOUND_BUFFER_COUNT_MSG_TEXT L"존재하는 사운드 버퍼(시드+재생) : "

//------------------------------------------------------------------------------------------------//

bool MkSoundManager::SetUp(HWND hWnd)
{
	do
	{
		MK_CHECK(SUCCEEDED(::DirectSoundCreate(NULL, &m_DirectSound, NULL)), L"DirectSound 객체 생성 실패")
			break;

		MK_CHECK(SUCCEEDED(m_DirectSound->SetCooperativeLevel(hWnd, DSSCL_NORMAL)), L"DSSCL_NORMAL로 DirectSound 협력 레벨 설정 실패")
			break;

		m_AutoIncrementalBufferIndex = 0;

		MK_DEV_PANEL.MsgToLog(L"MkSoundManager 시작");
		return true;
	}
	while (false);

	Clear();
	return false;
}

bool MkSoundManager::LoadSeedBuffer(const MkHashStr& filePath)
{
	if (m_DirectSound == NULL)
		return false;

	if (m_SeedBuffers.Exist(filePath))
		return true; // 이미 존재

	MkSoundBuffer* seed = new MkSoundBuffer;
	if (!seed->SetUp(m_DirectSound, filePath.GetString()))
	{
		delete seed;
		return false;
	}

	m_SeedBuffers.Create(filePath, seed);

	_UpdateSystemMessage();
	return true;
}

void MkSoundManager::ClearAllSeedBuffer(void)
{
	if (!m_SeedBuffers.Empty())
	{
		MkHashMapLooper<MkHashStr, MkSoundBuffer*> looper(m_SeedBuffers);
		MK_STL_LOOP(looper)
		{
			delete looper.GetCurrentField();
		}
		m_SeedBuffers.Clear();
	}

	_UpdateSystemMessage();
}

MkSoundChannel& MkSoundManager::GetChannel(const MkHashStr& channelName)
{
	return m_Channels.Exist(channelName) ? m_Channels[channelName] : m_Channels.Create(channelName);
}

void MkSoundManager::ClearAllPlayBuffer(void)
{
	if (!m_SeedBuffers.Empty())
	{
		MkMapLooper<unsigned int, MkSoundBuffer*> looper(m_PlayBuffers);
		MK_STL_LOOP(looper)
		{
			delete looper.GetCurrentField();
		}
		m_PlayBuffers.Clear();
	}

	_UpdateSystemMessage();
}

void MkSoundManager::Clear(void)
{
	m_Channels.Clear();

	ClearAllPlayBuffer();
	ClearAllSeedBuffer();

	if (m_DirectSound != NULL)
	{
		m_DirectSound->Release();
		m_DirectSound = NULL;

		MK_DEV_PANEL.MsgToLog(L"MkSoundManager 종료");
	}
}

//------------------------------------------------------------------------------------------------//

unsigned int MkSoundManager::__CreatePlayBuffer(const MkHashStr& filePath)
{
	if ((m_AutoIncrementalBufferIndex == MKDEF_EMPTY_SOUND_BUFFER_ID) || (!LoadSeedBuffer(filePath)))
		return MKDEF_EMPTY_SOUND_BUFFER_ID;

	MkSoundBuffer* buffer = m_SeedBuffers[filePath]->GetClone(m_DirectSound);
	if (buffer == NULL)
		return MKDEF_EMPTY_SOUND_BUFFER_ID;

	m_PlayBuffers.Create(m_AutoIncrementalBufferIndex, buffer);

	_UpdateSystemMessage();
	return m_AutoIncrementalBufferIndex++;
}

MkSoundBuffer& MkSoundManager::__GetPlayBuffer(unsigned int bufferID)
{
	if (m_PlayBuffers.Exist(bufferID))
	{
		return *m_PlayBuffers[bufferID];
	}

	static MkSoundBuffer emptyBuffer;
	return emptyBuffer;
}

void MkSoundManager::__RemovePlayBuffer(unsigned int bufferID)
{
	if (m_PlayBuffers.Exist(bufferID))
	{
		delete m_PlayBuffers[bufferID];
		m_PlayBuffers.Erase(bufferID);

		_UpdateSystemMessage();
	}
}

void MkSoundManager::_UpdateSystemMessage(void) const
{
	MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_SOUNDBUFFER,
		MKDEF_SOUND_BUFFER_COUNT_MSG_TEXT + MkStr(m_SeedBuffers.GetSize()) + L" + " + MkStr(m_PlayBuffers.GetSize()));
}

//------------------------------------------------------------------------------------------------//