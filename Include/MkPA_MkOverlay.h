#pragma once


//------------------------------------------------------------------------------------------------//
// overlay
// - window system의 경우 외부로 표현되는 형식의 종류는 얼마 되지 않는다
// - 자유도와 구현 난이도는 반비례 관계이므로 자유도를 패턴화해 제한함으로서 보다 쉽고 실수의 여지가
//   적은 구현 환경을 목표로 한다
// - 내부적으로는 scene node와 panel들이 복잡하게 구성되어 있지만 외부의 시선에는 모두 몇 종류의
//   패턴 중 하나로 인식되게 되는데, 이 패턴의 집합을 overlay라 한다
// - minikey window system의 외부 표현 요소는 모두 이 overlay로 통합된다
//
// 구성요소
// - piece set : 3*3방향의 조각으로 이루어진 집합. ePA_PiecePosition 참조
//   - edge : 바깥쪽 8방향을 구성하는 조각들
//   - center : 가운데 조각
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkSceneNode.h"
#include "MkWindowViewArgument.h"


class MkOverlay : public MkSceneNode
{
public:

	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_Overlay; }

	// 8방향 edge 설정
	//void SetEdge(const MkInt2& clientSize, ePA_PieceSetType type, bool useShadow);

	// 타이틀바를 위한 상방(LT, MT, RT)을 제외한 edge 설정
	//void SetEdgeUnderTitleBar(const MkInt2& clientSize, ePA_PieceSetType type);

	//
	//void SetPieceSetClient()

protected:
};
