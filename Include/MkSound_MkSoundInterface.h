#pragma once


//------------------------------------------------------------------------------------------------//
// ���� ��� ���� ��ɿ� �������̽�
// - ���ۿ� 1:1 ������ �ƴ����� �ϳ��� �������̽��� �ϳ��� ���۸� ������
//   (���� ���� ������ ����ǰ� ������ ���� �� �ű� ������ ���)
//
// ���ø����̼ǿ��� ���� ����� ���� �ּ� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"

#define MKDEF_EMPTY_SOUND_BUFFER_ID 0xffffffff


class MkSoundInterface
{
public:

	// ���
	void Play(const MkHashStr& filePath, bool loop = false);

	// ������ ���� Ű ��ȯ
	inline const MkHashStr& GetFilePath(void) const { return m_CurrentKey; }

	// ����� ���� ��ȯ
	bool IsPlaying(void) const;

	// ���� ��� ����(�ʴ���) ��ȯ
	// (NOTE) ����Ʈ frequency ����
	float GetPlayPosition(void) const;

	// ���� ��� ����(�ʴ���) ����
	// (NOTE) ����߿��� ��ȿ
	void SetPlayPosition(float position);

	// �� ��� �ð�(�ʴ���) ��ȯ
	// (NOTE) ����Ʈ frequency ����
	float GetTotalPlayTime(void) const;

	// ����
	void Stop(void);

	// ���� ����. 0.f(�Ҹ� ����) ~ 1.f(�Ҹ� �ִ�)
	void SetVolume(float volume);

	// ���� ��ȯ
	float GetVolume(void) const;

	// ���ļ� ����
	// frequency : 1.f(�⺻ �ӵ�)�� �������� �ӵ��� ����
	void SetFrequency(float frequency);

	// ���� ���� ����. 0.f�� �⺻
	void SetSemitone(float semitone);

	// ���ļ� ��ȯ
	float GetFrequency(void) const;

	// �д� ��ġ ����. -1.f(���� ����) ~ 0.f(�߾�) ~ 1.f(���� ������)
	void SetPan(float position);

	// �д� ��ġ ��ȯ
	float GetPan(void) const;

	// ����� ���� ����
	void Clear(void);

	MkSoundInterface();
	virtual ~MkSoundInterface() { Clear(); }

protected:

	MkHashStr m_CurrentKey;
	unsigned int m_BufferID;
};
