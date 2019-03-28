#pragma once


//------------------------------------------------------------------------------------------------//
// MkSoundInterface의 그룹화 객체
// - 동시성, 최대 개수 제한
//
// 어플리케이션에서는 목적마다 하나를 할당해 사용하길 권장
// ex> 배경음악, 이펙트, 캐릭터 대사 등
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDeque.h"


class MkHashStr;
class MkSoundInterface;

class MkSoundChannel
{
public:

	// 동일 음원 재생의 최소 텀(초단위). 기본은 0.4초
	// 동일 음원은 해당 시간 안에는 연속적으로 재생되지 않음
	// ex> 다수의 부대원이 동시에 총을 쏜 경우 굳이 인원 수 만큼 발사음을 재생 할 필요가 없음
	void SetTimeGapAtSameSource(double gap);

	// 채널에서 동시에 재생될 수 있는 최대 음원 개수. 기본은 16개
	// 모자랄 경우 가장 예전에 재생된 음원을 삭제
	void SetMaxInterfaceSize(unsigned int size);

	// 재생
	void Play(const MkHashStr& filePath, bool loop = false);

	// 채널 모든 음원 정지
	void Stop(void);

	// 채널 모든 음원 볼륨 조정
	void SetVolume(float volume);

	// 채널 모든 재생 음원 삭제(시드는 남아 있음)
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
