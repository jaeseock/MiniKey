#pragma once


//------------------------------------------------------------------------------------------------//
// 하나의 이미지를 여러개의 부분으로 쪼개 사용하기 위한 정보
// - Subset은 static image
// - Sequence는 time table에 대한 Subset list, 즉 이미지 교체 애니메이션임
// 외부 시점에서는 Subset과 Sequence는 구분 할 의미가 없음
//
// ex> subset
//	Node "PRIFIX(single일 경우 NAME)"
//	{
//		int2 Position = (0, 0); // (opt)위치. 기본값은 (0, 0)
//		int2 Size = (101, 101); // 크기
//		int2 Table = (1, 1); // (opt)테이블의 행렬(xy). 기본값은 (1, 1). 기본값이 (1, 1)이 아닐 경우(복수의 subset 의미) PRIFIX 뒤에 numbering이 붙음
//		int2 Offset = (101, 101); // (opt)Table에서 복수의 subset을 의미할 경우 다음 subset 탐색시의 이동 거리. 수평이동시 x, 수직이동시 y 사용. 기본값은 Size
//	}
//
// ex> sequence
//	Node "NAME"
//	{
//		float TotalRange = 4.5; // 총 시간 구간의 길이(sec)
//		str SubsetList = "P_0" / "P_1" / "P_2"; // subset list
//		//str SubsetList = "~" / "P_" / "0" / "2"; // 위의 리스트를 이런 식으로도 표현 가능. 형식은 ("~" / "PREFIX" / "1st FIRST_INDEX" / "1st LAST_INDEX" / "2nd FIRST_INDEX" / "2nd LAST_INDEX" / ...)
//		float TimeList = 0 / 1.5 / 3; // (opt)subset list와 1:1 대응되는 등장 시간. 없으면 Range를 Subset 갯수만큼 균일하게 쪼개서 사용
//		bool Loop = yes; // (opt)루핑 여부. 기본값은 true
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"

class MkSceneNode;
class MkWindowBaseNode;

class MkWindowOpHelper
{
public:

	static MkWindowBaseNode* CreateTitleBar(
		const MkHashStr& name,
		const MkHashStr& themeName,
		float length,
		bool useCloseBtn,
		const MkHashStr& iconPath,
		const MkHashStr& iconSubsetOrSequenceName,
		const MkStr& titleCaption,
		bool alignCaptionToCenter
		);
};
