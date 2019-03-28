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

	// 파일로부터 초기화
	bool SetUp(LPDIRECTSOUND directSound, const MkPathName& filePath);

	// 사운드 버퍼로부터 초기화
	bool SetUp(LPDIRECTSOUND directSound, LPDIRECTSOUNDBUFFER srcBuffer);

	// 데이터를 공유하는 버퍼를 생성해 반환
	MkSoundBuffer* GetClone(LPDIRECTSOUND directSound);

	// 재생
	void Play(bool loop = false);

	// 재생중 여부 반환
	bool IsPlaying(void) const;

	// 현재 재생 지점(초단위) 반환
	// (NOTE) 디폴트 frequency 기준
	float GetPlayPosition(void) const;

	// 현재 재생 지점(초단위) 지정
	// (NOTE) 재생중에만 유효
	void SetPlayPosition(float position);

	// 총 재생 시간(초단위) 반환
	// (NOTE) 디폴트 frequency 기준
	float GetTotalPlayTime(void) const;

	// 정지
	void Stop(void);

	// 볼륨 지정. 0.f(소리 없음) ~ 1.f(소리 최대)
	void SetVolume(float volume);

	// 볼륨 반환
	inline float GetVolume(void) const { return m_Volume; }

	// 주파수 지정
	// frequency : 1.f(기본 속도)를 기준으로 속도를 조정
	void SetFrequency(float frequency);

	// 반음 단위 조정. 0.f가 기본
	void SetSemitone(float semitone);

	// 주파수 반환
	float GetFrequency(void) const;

	// 패닝 위치 지정. -1.f(가장 왼쪽) ~ 0.f(중앙) ~ 1.f(가장 오른쪽)
	void SetPan(float position);

	// 패닝 위치 반환
	float GetPan(void) const;

	// 해제
	void Clear(void);

	MkSoundBuffer();
	virtual ~MkSoundBuffer() { Clear(); }

protected:

	bool _CreateBufferFromWaveFile(LPDIRECTSOUND directSound, const MkByteArray& srcData);
	bool _CreateBufferFromOggFile(LPDIRECTSOUND directSound, const MkByteArray& srcData);

	static LPDIRECTSOUNDBUFFER _CreateSoundBuffer(LPDIRECTSOUND directSound, const WAVEFORMATEX& waveFormatEx, const MkByteArray& dataBuffer);

	void _UpdateInformation(void);

protected:

	// 버퍼
	LPDIRECTSOUNDBUFFER m_SoundBuffer;

	// 채널. 0:unload, 1:mono, 2:stereo
	int m_Channels;

	// 기본 주파수
	float m_DefaultFrequency;

	// 초당 소모 바이트 수 : frequency * channels * sampling(byte)
	float m_BytesPerSec;

	// 음원 총 재생시간
	float m_TotalPlayTime;

	// 볼륨
	float m_Volume;
};
