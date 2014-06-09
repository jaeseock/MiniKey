#ifndef __MINIKEY_CORE_MKANGLEOP_H__
#define __MINIKEY_CORE_MKANGLEOP_H__

//------------------------------------------------------------------------------------------------//
// 각도 관련 api
// degree <-> radian 관련 변환
//------------------------------------------------------------------------------------------------//


class MkVec2;

class MkAngleOp
{
public:

	//------------------------------------------------------------------------------------------------//
	
	// degree 정규화 (0.f <= return < 360.f)
	static float Unitize360(float degree);

	// degree 정규화 (-180.f <= return < 180.f)
	static float Unitize180(float degree);

	// radian 정규화 (0.f <= return < 2*PI)
	static float Unitize2PI(float radian);

	// radian 정규화 (-PI <= return < PI)
	static float UnitizePI(float radian);

	// from에서 to에 이르는 각도 (from + addition = to)
	// degree & radian
	static float GetAddition(float from, float to);

	// degree -> radian
	static float DegreeToRadian(float degree);

	// radian -> degree
	static float RadianToDegree(float radian);

	// circle의 반지름이 radius고 movementDelta만큼 움직였을 경우의 회전각(radian)
	static float GetRotatationDelta(float radius, float movementDelta);

	//------------------------------------------------------------------------------------------------//

	// 삼각함수
	static float Sin(float radian);
	static float Cos(float radian);
	static float Tan(float radian);

	// -1.f <= value <= 1.f (범위를 넘어서면 클램프)
	// radian 반환
	static float ASin(float value);
	static float ACos(float value);
	static float ATan(float value);

	//------------------------------------------------------------------------------------------------//

	// 방향(unit vector) -> radian 변환
	// ex> MkVec2(0.f, 1.f) -> 0.f
	// ex> MkVec2(1.f, 0.f) -> 0.5*PI
	static float DirectionToRadian(const MkVec2& direction);

	// radian -> 방향(unit vector) 변환
	// ex> 0.f -> MkVec2(0.f, 1.f)
	// ex> 0.5*PI -> MkVec2(1.f, 0.f)
	static MkVec2 RadianToDirection(float radian);

	//------------------------------------------------------------------------------------------------//
};

#endif
