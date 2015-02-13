#pragma once


//------------------------------------------------------------------------------------------------//
// overlay
// - window system�� ��� �ܺη� ǥ���Ǵ� ������ ������ �� ���� �ʴ´�
// - �������� ���� ���̵��� �ݺ�� �����̹Ƿ� �������� ����ȭ�� ���������μ� ���� ���� �Ǽ��� ������
//   ���� ���� ȯ���� ��ǥ�� �Ѵ�
// - ���������δ� scene node�� panel���� �����ϰ� �����Ǿ� ������ �ܺ��� �ü����� ��� �� ������
//   ���� �� �ϳ��� �νĵǰ� �Ǵµ�, �� ������ ������ overlay�� �Ѵ�
// - minikey window system�� �ܺ� ǥ�� ��Ҵ� ��� �� overlay�� ���յȴ�
//
// �������
// - piece set : 3*3������ �������� �̷���� ����. ePA_PiecePosition ����
//   - edge : �ٱ��� 8������ �����ϴ� ������
//   - center : ��� ����
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkSceneNode.h"
#include "MkWindowViewArgument.h"


class MkOverlay : public MkSceneNode
{
public:

	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_Overlay; }

	// 8���� edge ����
	//void SetEdge(const MkInt2& clientSize, ePA_PieceSetType type, bool useShadow);

	// Ÿ��Ʋ�ٸ� ���� ���(LT, MT, RT)�� ������ edge ����
	//void SetEdgeUnderTitleBar(const MkInt2& clientSize, ePA_PieceSetType type);

	//
	//void SetPieceSetClient()

protected:
};
