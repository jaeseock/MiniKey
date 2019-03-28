
#include "MkCore_MkTimeManager.h"

#include "MkSound_MkSoundInterface.h"
#include "MkSound_MkSoundChannel.h"


//------------------------------------------------------------------------------------------------//

void MkSoundChannel::SetTimeGapAtSameSource(double gap)
{
	m_TimeGapAtSameSource = GetMax<double>(0., gap);
}

void MkSoundChannel::SetMaxInterfaceSize(unsigned int size)
{
	m_MaxInterfaceSize = size;
	_CutSize(m_MaxInterfaceSize);
}

void MkSoundChannel::Play(const MkHashStr& filePath, bool loop)
{
	if (m_MaxInterfaceSize > 0)
	{
		MkTimeState ts;
		MK_TIME_MGR.GetCurrentTimeState(ts);

		// 동일 음원이면 시간 제한 점검
		if (m_TimeGapAtSameSource > 0.)
		{
			MK_INDEXING_RLOOP(m_Queue, i)
			{
				double gap = ts.fullTime - m_Queue[i].timeStamp;
				if (gap < m_TimeGapAtSameSource)
				{
					if (filePath == m_Queue[i].sound->GetFilePath())
						return; // 제한시간내 동일 음원이 존재
				}
				else
					break; // ok
			}
		}

		// 추가
		_CutSize(m_MaxInterfaceSize - 1);
		
		_PlayUnit& pu = m_Queue.PushBack();
		pu.sound = new MkSoundInterface;
		pu.sound->Play(filePath, loop);
		pu.timeStamp = ts.fullTime;
	}
}

void MkSoundChannel::Stop(void)
{
	MK_INDEXING_LOOP(m_Queue, i)
	{
		m_Queue[i].sound->Stop();
	}
}

void MkSoundChannel::SetVolume(float volume)
{
	volume = Clamp<float>(volume, 0.f, 1.f);
	if (volume != m_Volume)
	{
		m_Volume = volume;

		MK_INDEXING_LOOP(m_Queue, i)
		{
			m_Queue[i].sound->SetVolume(m_Volume);
		}
	}
}

void MkSoundChannel::Clear(void)
{
	MK_INDEXING_LOOP(m_Queue, i)
	{
		delete m_Queue[i].sound;
	}
	m_Queue.Clear();
}

MkSoundChannel::MkSoundChannel()
{
	m_TimeGapAtSameSource = 0.4;
	m_MaxInterfaceSize = 16;
	m_Volume = 1.f;
}

//------------------------------------------------------------------------------------------------//

void MkSoundChannel::_CutSize(unsigned int targetSize)
{
	if (targetSize < m_Queue.GetSize())
	{
		// 재생중이지 않은 인터페이스 정리
		MkDeque<_PlayUnit> buffer;
		while (!m_Queue.Empty())
		{
			if (m_Queue[0].sound->IsPlaying())
			{
				buffer.PushBack(m_Queue[0]); // 가장 앞이 재생중이면 뒤로 돌리고
			}
			else
			{
				delete m_Queue[0].sound; // 아니면 삭제
			}
			m_Queue.PopFront();
		}
		m_Queue = buffer;

		// 그래도 부족하면 그만큼 앞부터 삭제
		while (targetSize < m_Queue.GetSize())
		{
			delete m_Queue[0].sound;
			m_Queue.PopFront();
		}
	}
}

//------------------------------------------------------------------------------------------------//
