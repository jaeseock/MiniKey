#pragma once


//------------------------------------------------------------------------------------------------//
// 음원 재생 관련 명령용 인터페이스
// - 버퍼와 1:1 대응은 아니지만 하나의 인터페이스는 하나의 버퍼만 점유함
//   (만약 이전 음원이 재생되고 있으면 삭제 후 신규 음원을 재생)
//
// 어플리케이션에서 음원 재생을 위한 최소 단위
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"

#define MKDEF_EMPTY_SOUND_BUFFER_ID 0xffffffff


class MkSoundInterface
{
public:

	// 재생
	void Play(const MkHashStr& filePath, bool loop = false);

	// 설정된 파일 키 반환
	inline const MkHashStr& GetFilePath(void) const { return m_CurrentKey; }

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
	float GetVolume(void) const;

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

	// 연결된 버퍼 삭제
	void Clear(void);

	MkSoundInterface();
	virtual ~MkSoundInterface() { Clear(); }

protected:

	MkHashStr m_CurrentKey;
	unsigned int m_BufferID;
};
