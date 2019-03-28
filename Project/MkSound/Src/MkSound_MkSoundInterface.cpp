
#include "MkSound_MkSoundBuffer.h"
#include "MkSound_MkSoundManager.h"
#include "MkSound_MkSoundInterface.h"


//------------------------------------------------------------------------------------------------//

void MkSoundInterface::Play(const MkHashStr& filePath, bool loop)
{
	if (filePath == m_CurrentKey)
	{
		MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).Play(loop);
	}
	else
	{
		Clear();

		m_BufferID = MK_SOUND_MGR.__CreatePlayBuffer(filePath);
		if (m_BufferID != MKDEF_EMPTY_SOUND_BUFFER_ID)
		{
			MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).Play(loop);
			m_CurrentKey = filePath;
		}
	}
}

bool MkSoundInterface::IsPlaying(void) const
{
	return MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).IsPlaying();
}

float MkSoundInterface::GetPlayPosition(void) const
{
	return MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).GetPlayPosition();
}

void MkSoundInterface::SetPlayPosition(float position)
{
	MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).SetPlayPosition(position);
}

float MkSoundInterface::GetTotalPlayTime(void) const
{
	return MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).GetTotalPlayTime();
}

void MkSoundInterface::Stop(void)
{
	MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).Stop();
}

void MkSoundInterface::SetVolume(float volume)
{
	MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).SetVolume(volume);
}

float MkSoundInterface::GetVolume(void) const
{
	return MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).GetVolume();
}

void MkSoundInterface::SetFrequency(float frequency)
{
	MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).SetFrequency(frequency);
}

void MkSoundInterface::SetSemitone(float semitone)
{
	MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).SetSemitone(semitone);
}

float MkSoundInterface::GetFrequency(void) const
{
	return MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).GetFrequency();
}

void MkSoundInterface::SetPan(float position)
{
	MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).SetPan(position);
}

float MkSoundInterface::GetPan(void) const
{
	return MK_SOUND_MGR.__GetPlayBuffer(m_BufferID).GetPan();
}

void MkSoundInterface::Clear(void)
{
	if ((MkSoundManager::InstancePtr() != NULL) && (m_BufferID != MKDEF_EMPTY_SOUND_BUFFER_ID))
	{
		MK_SOUND_MGR.__RemovePlayBuffer(m_BufferID);
		m_CurrentKey.Clear();
		m_BufferID = MKDEF_EMPTY_SOUND_BUFFER_ID;
	}
}

MkSoundInterface::MkSoundInterface()
{
	m_BufferID = MKDEF_EMPTY_SOUND_BUFFER_ID;
}

//------------------------------------------------------------------------------------------------//
