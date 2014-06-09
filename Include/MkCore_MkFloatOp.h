#ifndef __MINIKEY_CORE_MKFLOATOP_H__
#define __MINIKEY_CORE_MKFLOATOP_H__

//------------------------------------------------------------------------------------------------//
// float ����
// �⺻ �ڷ����� ������ ���ϸ� �����ϹǷ� pure functionȭ
//------------------------------------------------------------------------------------------------//


class MkFloatOp
{
public:

	//------------------------------------------------------------------------------------------------//

	// ����. grid�� �ִ� floor()
	// ex> SnapToLowerBound(4.8f, 2.f) -> 4.f
	// ex> SnapToLowerBound(-4.8f, 2.f) -> -6.f
	static float SnapToLowerBound(float source, float grid);

	// �ø�. grid�� �ִ� ceil()
	// ex> SnapToUpperBound(4.8f, 2.f) -> 6.f
	// ex> SnapToUpperBound(-4.8f, 2.f) -> -4.f
	static float SnapToUpperBound(float source, float grid);

	//------------------------------------------------------------------------------------------------//

	// �������� ���� (���������� % ����� ����)
	// ���� ��� �Լ��� fmod()�� ������ ����ð��� �ι���
	// ex> GetRemainder(4.8f, 2.f) -> 0.8f
	// ex> GetRemainder(-4.8f, 2.f) -> 1.2f
	static float GetRemainder(float source, float divider);

	// �ݿø�
	// ex> RoundingOff(4.8f) -> 5.f
	// ex> RoundingOff(-4.8f) -> -5.f
	static float RoundingOff(float source);

	// ���� �� ��������ȯ
	static int FloatToInt(float source);

	//------------------------------------------------------------------------------------------------//

	// ���� üũ (minRange <= source <= maxRange)
	static bool IsBetween(float minRange, float source, float maxRange);

	// ���� ���� ������ (minRange <= source <= maxRange, minRange != maxRange)
	// ex> GetLinearPosition(4.f, 7.5f, 18.f) -> 0.25f
	static float GetLinearPosition(float minRange, float source, float maxRange);

	// ���� ���� ������ (lastX <= currX <= nextX, lastX != nextX)
	// currX�� �ش��ϴ� currY ��ȯ
	// ex> GetLinearPosition(2.f, 1.f, 3.f, 6.f, 3.f) -> 1.5f
	static float GetLinearPosition(float lastX, float lastY, float currX, float nextX, float nextY);

	// source�� target���� �۽Ƿ� ���� �ȿ� �ִ��� �Ǻ�
	static bool CloseToNear(float source, float target);

	// source�� zero���� �۽Ƿ� ���� �ȿ� �ִ��� �Ǻ�
	static bool CloseToZero(float source);

	//------------------------------------------------------------------------------------------------//

	// ��������
	// fraction(0.f ~ 1.f)
	static float LinearInterpolate(float a, float b, float fraction);

	// cosine ����
	// fraction(0.f ~ 1.f)
	static float CosineInterpolate(float a, float b, float fraction);

	//------------------------------------------------------------------------------------------------//

	// �ǻ� ���� ������
	// -1.f ~ 1.f ���̿��� ��ȯ
	static float GenerateRandomNumber(int seed);

	//------------------------------------------------------------------------------------------------//

	// �ʴ��� �ð��� ��:��:��:�и��� ������ ��ȯ
	static void ConvertSecondsToClockTime
		(float seconds, unsigned int& hour, unsigned int& minute, unsigned int& second, unsigned int& millisec);

	//------------------------------------------------------------------------------------------------//
};

#endif
