#ifndef __MINIKEY_CORE_MKGLOBALDEFINITION_H__
#define __MINIKEY_CORE_MKGLOBALDEFINITION_H__


//------------------------------------------------------------------------------------------------//

// 배열의 최대 크기
#define MKDEF_MAX_ARRAY_SIZE 0x7fffffff

// 배열의 에러 태그
#define MKDEF_ARRAY_ERROR 0x80000000

// float 앱실론(accuracy)
#define MKDEF_FLOAT_EPSILON 0.0001f

// PI
#define MKDEF_PI 3.1415927f
#define MKDEF_PI_DIVIDED_BY_180D 0.017453293f // PI / 180.f
#define MKDEF_180D_DIVIDED_BY_PI 57.29578f // 180.f / PI

// 스레드 프로파일링으로 인식할 key prefix
#define MKDEF_PROFILING_PREFIX_FOR_THREAD L"__#T_"

//------------------------------------------------------------------------------------------------//
// custom type definition
//------------------------------------------------------------------------------------------------//

// 64bit ID
typedef unsigned __int64 ID64;


//------------------------------------------------------------------------------------------------//
// system window property
//------------------------------------------------------------------------------------------------//

enum eSystemWindowProperty
{
	eSWP_None = 0,
	eSWP_Minimize = 1,
	eSWP_Maximize = 1 << 1,
	eSWP_Close = 1 << 2,
	eSWP_FixedSize = eSWP_Minimize | eSWP_Close,
	eSWP_All = eSWP_Minimize | eSWP_Maximize | eSWP_Close
};


//------------------------------------------------------------------------------------------------//
// rectangle간 정렬 타입/위치(alignment type/position)
//
// leftmost : left : middle : right : rightmost
// 
//			□　□　□　□　□		: over
//			　┼──┴──┼
//			□│□　□　□│□		: top
//			　│          │
//			□┤□　□　□├□		: center
//			　│          │
//			□│□　□　□│□		: bottom
//			　┼──┬──┼
//			□　□　□　□　□		: under
//
// vertical alignment (5)
// - leftmost, left, middle, right, rightmost
// horizontal alignment (5)
// - over, top, center, bottom, under
//------------------------------------------------------------------------------------------------//

enum eRectAlignmentType
{
	eRAT_NoneType = 0,

	// horizontal
	eRAT_LMost,
	eRAT_Left,
	eRAT_Middle,
	eRAT_Right,
	eRAT_RMost,

	// vertical
	eRAT_Over = eRAT_LMost << 4,
	eRAT_Top = eRAT_Left << 4,
	eRAT_Center = eRAT_Middle << 4,
	eRAT_Bottom = eRAT_Right << 4,
	eRAT_Under = eRAT_RMost << 4
};


enum eRectAlignmentPosition
{
	eRAP_NonePosition = 0,

	eRAP_LMostOver = eRAT_LMost | eRAT_Over,
	eRAP_LMostTop = eRAT_LMost | eRAT_Top,
	eRAP_LMostCenter = eRAT_LMost | eRAT_Center,
	eRAP_LMostBottom = eRAT_LMost | eRAT_Bottom,
	eRAP_LMostUnder = eRAT_LMost | eRAT_Under,

	eRAP_LeftOver = eRAT_Left | eRAT_Over,
	eRAP_LeftTop = eRAT_Left | eRAT_Top,
	eRAP_LeftCenter = eRAT_Left | eRAT_Center,
	eRAP_LeftBottom = eRAT_Left | eRAT_Bottom,
	eRAP_LeftUnder = eRAT_Left | eRAT_Under,

	eRAP_MiddleOver = eRAT_Middle | eRAT_Over,
	eRAP_MiddleTop = eRAT_Middle | eRAT_Top,
	eRAP_MiddleCenter = eRAT_Middle | eRAT_Center,
	eRAP_MiddleBottom = eRAT_Middle | eRAT_Bottom,
	eRAP_MiddleUnder = eRAT_Middle | eRAT_Under,

	eRAP_RightOver = eRAT_Right | eRAT_Over,
	eRAP_RightTop = eRAT_Right | eRAT_Top,
	eRAP_RightCenter = eRAT_Right | eRAT_Center,
	eRAP_RightBottom = eRAT_Right | eRAT_Bottom,
	eRAP_RightUnder = eRAT_Right | eRAT_Under,

	eRAP_RMostOver = eRAT_RMost | eRAT_Over,
	eRAP_RMostTop = eRAT_RMost | eRAT_Top,
	eRAP_RMostCenter = eRAT_RMost | eRAT_Center,
	eRAP_RMostBottom = eRAT_RMost | eRAT_Bottom,
	eRAP_RMostUnder = eRAT_RMost | eRAT_Under
};

//------------------------------------------------------------------------------------------------//
// 보간자
//------------------------------------------------------------------------------------------------//

// curve 보간 종류
enum eCurveInterpolationType
{
	eCIT_Constant = 0, // 해당 구간 값을 상수화
	eCIT_Linear, // 선형 보간
	eCIT_Cubic // monotonic cubic hermite spline
};

// curve 범위외 행동
enum eCurveOutOfRangeAction
{
	eCORA_NearValue = 0, // 가장 가까운 마지막 값으로 지정
	eCORA_Repetition, // 구간 반복
	eCORA_Mirror // 대칭형으로 구간 반복
};

//------------------------------------------------------------------------------------------------//

class MkVirtualInstance
{
public:

	MkVirtualInstance() {}
	virtual ~MkVirtualInstance() {}
};

//------------------------------------------------------------------------------------------------//

#endif

