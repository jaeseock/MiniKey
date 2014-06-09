#ifndef __MINIKEY_CORE_MKANGLEOP_H__
#define __MINIKEY_CORE_MKANGLEOP_H__

//------------------------------------------------------------------------------------------------//
// ���� ���� api
// degree <-> radian ���� ��ȯ
//------------------------------------------------------------------------------------------------//


class MkVec2;

class MkAngleOp
{
public:

	//------------------------------------------------------------------------------------------------//
	
	// degree ����ȭ (0.f <= return < 360.f)
	static float Unitize360(float degree);

	// degree ����ȭ (-180.f <= return < 180.f)
	static float Unitize180(float degree);

	// radian ����ȭ (0.f <= return < 2*PI)
	static float Unitize2PI(float radian);

	// radian ����ȭ (-PI <= return < PI)
	static float UnitizePI(float radian);

	// from���� to�� �̸��� ���� (from + addition = to)
	// degree & radian
	static float GetAddition(float from, float to);

	// degree -> radian
	static float DegreeToRadian(float degree);

	// radian -> degree
	static float RadianToDegree(float radian);

	// circle�� �������� radius�� movementDelta��ŭ �������� ����� ȸ����(radian)
	static float GetRotatationDelta(float radius, float movementDelta);

	//------------------------------------------------------------------------------------------------//

	// �ﰢ�Լ�
	static float Sin(float radian);
	static float Cos(float radian);
	static float Tan(float radian);

	// -1.f <= value <= 1.f (������ �Ѿ�� Ŭ����)
	// radian ��ȯ
	static float ASin(float value);
	static float ACos(float value);
	static float ATan(float value);

	//------------------------------------------------------------------------------------------------//

	// ����(unit vector) -> radian ��ȯ
	// ex> MkVec2(0.f, 1.f) -> 0.f
	// ex> MkVec2(1.f, 0.f) -> 0.5*PI
	static float DirectionToRadian(const MkVec2& direction);

	// radian -> ����(unit vector) ��ȯ
	// ex> 0.f -> MkVec2(0.f, 1.f)
	// ex> 0.5*PI -> MkVec2(1.f, 0.f)
	static MkVec2 RadianToDirection(float radian);

	//------------------------------------------------------------------------------------------------//
};

#endif
