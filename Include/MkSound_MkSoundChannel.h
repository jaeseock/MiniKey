#pragma once


//------------------------------------------------------------------------------------------------//
// MkSoundInterface�� �׷�ȭ ��ü
// - ���ü�, �ִ� ���� ����
//
// ���ø����̼ǿ����� �������� �ϳ��� �Ҵ��� ����ϱ� ����
// ex> �������, ����Ʈ, ĳ���� ��� ��
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDeque.h"


class MkHashStr;
class MkSoundInterface;

class MkSoundChannel
{
public:

	// ���� ���� ����� �ּ� ��(�ʴ���). �⺻�� 0.4��
	// ���� ������ �ش� �ð� �ȿ��� ���������� ������� ����
	// ex> �ټ��� �δ���� ���ÿ� ���� �� ��� ���� �ο� �� ��ŭ �߻����� ��� �� �ʿ䰡 ����
	void SetTimeGapAtSameSource(double gap);

	// ä�ο��� ���ÿ� ����� �� �ִ� �ִ� ���� ����. �⺻�� 16��
	// ���ڶ� ��� ���� ������ ����� ������ ����
	void SetMaxInterfaceSize(unsigned int size);

	// ���
	void Play(const MkHashStr& filePath, bool loop = false);

	// ä�� ��� ���� ����
	void Stop(void);

	// ä�� ��� ���� ���� ����
	void SetVolume(float volume);

	// ä�� ��� ��� ���� ����(�õ�� ���� ����)
	void Clear(void);

	MkSoundChannel();
	virtual ~MkSoundChannel() { Clear(); }

protected:

	void _CutSize(unsigned int targetSize);

	typedef struct __PlayUnit
	{
		MkSoundInterface* sound;
		double timeStamp;
	}
	_PlayUnit;

protected:

	MkDeque<_PlayUnit> m_Queue;

	double m_TimeGapAtSameSource;
	unsigned int m_MaxInterfaceSize;
	float m_Volume;
};
