#pragma once


//------------------------------------------------------------------------------------------------//
// direct sound buffer
// - wav, ogg
// - blocking
//------------------------------------------------------------------------------------------------//

#include <dsound.h>
#include "MkCore_MkArray.h"


class MkPathName;

class MkSoundBuffer
{
public:

	// ���Ϸκ��� �ʱ�ȭ
	bool SetUp(LPDIRECTSOUND directSound, const MkPathName& filePath);

	// ���� ���۷κ��� �ʱ�ȭ
	bool SetUp(LPDIRECTSOUND directSound, LPDIRECTSOUNDBUFFER srcBuffer);

	// �����͸� �����ϴ� ���۸� ������ ��ȯ
	MkSoundBuffer* GetClone(LPDIRECTSOUND directSound);

	// ���
	void Play(bool loop = false);

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
	inline float GetVolume(void) const { return m_Volume; }

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

	// ����
	void Clear(void);

	MkSoundBuffer();
	virtual ~MkSoundBuffer() { Clear(); }

protected:

	bool _CreateBufferFromWaveFile(LPDIRECTSOUND directSound, const MkByteArray& srcData);
	bool _CreateBufferFromOggFile(LPDIRECTSOUND directSound, const MkByteArray& srcData);

	static LPDIRECTSOUNDBUFFER _CreateSoundBuffer(LPDIRECTSOUND directSound, const WAVEFORMATEX& waveFormatEx, const MkByteArray& dataBuffer);

	void _UpdateInformation(void);

protected:

	// ����
	LPDIRECTSOUNDBUFFER m_SoundBuffer;

	// ä��. 0:unload, 1:mono, 2:stereo
	int m_Channels;

	// �⺻ ���ļ�
	float m_DefaultFrequency;

	// �ʴ� �Ҹ� ����Ʈ �� : frequency * channels * sampling(byte)
	float m_BytesPerSec;

	// ���� �� ����ð�
	float m_TotalPlayTime;

	// ����
	float m_Volume;
};
