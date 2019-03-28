
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

		// ���� �����̸� �ð� ���� ����
		if (m_TimeGapAtSameSource > 0.)
		{
			MK_INDEXING_RLOOP(m_Queue, i)
			{
				double gap = ts.fullTime - m_Queue[i].timeStamp;
				if (gap < m_TimeGapAtSameSource)
				{
					if (filePath == m_Queue[i].sound->GetFilePath())
						return; // ���ѽð��� ���� ������ ����
				}
				else
					break; // ok
			}
		}

		// �߰�
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
		// ��������� ���� �������̽� ����
		MkDeque<_PlayUnit> buffer;
		while (!m_Queue.Empty())
		{
			if (m_Queue[0].sound->IsPlaying())
			{
				buffer.PushBack(m_Queue[0]); // ���� ���� ������̸� �ڷ� ������
			}
			else
			{
				delete m_Queue[0].sound; // �ƴϸ� ����
			}
			m_Queue.PopFront();
		}
		m_Queue = buffer;

		// �׷��� �����ϸ� �׸�ŭ �պ��� ����
		while (targetSize < m_Queue.GetSize())
		{
			delete m_Queue[0].sound;
			m_Queue.PopFront();
		}
	}
}

//------------------------------------------------------------------------------------------------//
