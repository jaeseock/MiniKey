#ifndef __MINIKEY_CORE_MKFLOATOP_H__
#define __MINIKEY_CORE_MKFLOATOP_H__

//------------------------------------------------------------------------------------------------//
// float 연산
// 기본 자료형의 래핑은 부하만 유발하므로 pure function화
//------------------------------------------------------------------------------------------------//


class MkFloatOp
{
public:

	//------------------------------------------------------------------------------------------------//

	// 내림. grid가 있는 floor()
	// ex> SnapToLowerBound(4.8f, 2.f) -> 4.f
	// ex> SnapToLowerBound(-4.8f, 2.f) -> -6.f
	static float SnapToLowerBound(float source, float grid);

	// 올림. grid가 있는 ceil()
	// ex> SnapToUpperBound(4.8f, 2.f) -> 6.f
	// ex> SnapToUpperBound(-4.8f, 2.f) -> -4.f
	static float SnapToUpperBound(float source, float grid);

	//------------------------------------------------------------------------------------------------//

	// 나머지를 구함 (정수에서의 % 연산과 동일)
	// 동일 기능 함수로 fmod()가 있지만 연산시간이 두배임
	// ex> GetRemainder(4.8f, 2.f) -> 0.8f
	// ex> GetRemainder(-4.8f, 2.f) -> 1.2f
	static float GetRemainder(float source, float divider);

	// 반올림
	// ex> RoundingOff(4.8f) -> 5.f
	// ex> RoundingOff(-4.8f) -> -5.f
	static float RoundingOff(float source);

	// 내림 후 정수형변환
	static int FloatToInt(float source);

	//------------------------------------------------------------------------------------------------//

	// 범위 체크 (minRange <= source <= maxRange)
	static bool IsBetween(float minRange, float source, float maxRange);

	// 선형 범위 포지션 (minRange <= source <= maxRange, minRange != maxRange)
	// ex> GetLinearPosition(4.f, 7.5f, 18.f) -> 0.25f
	static float GetLinearPosition(float minRange, float source, float maxRange);

	// 선형 범위 포지션 (lastX <= currX <= nextX, lastX != nextX)
	// currX에 해당하는 currY 반환
	// ex> GetLinearPosition(2.f, 1.f, 3.f, 6.f, 3.f) -> 1.5f
	static float GetLinearPosition(float lastX, float lastY, float currX, float nextX, float nextY);

	// source가 target에서 앱실론 범위 안에 있는지 판별
	static bool CloseToNear(float source, float target);

	// source가 zero에서 앱실론 범위 안에 있는지 판별
	static bool CloseToZero(float source);

	//------------------------------------------------------------------------------------------------//

	// 선형보간
	// fraction(0.f ~ 1.f)
	static float LinearInterpolate(float a, float b, float fraction);

	// cosine 보간
	// fraction(0.f ~ 1.f)
	static float CosineInterpolate(float a, float b, float fraction);

	//------------------------------------------------------------------------------------------------//

	// 의사 난수 생성기
	// -1.f ~ 1.f 사이에서 반환
	static float GenerateRandomNumber(int seed);

	//------------------------------------------------------------------------------------------------//

	// 초단위 시간을 시:분:초:밀리초 단위로 변환
	static void ConvertSecondsToClockTime
		(float seconds, unsigned int& hour, unsigned int& minute, unsigned int& second, unsigned int& millisec);

	//------------------------------------------------------------------------------------------------//
};

#endif
