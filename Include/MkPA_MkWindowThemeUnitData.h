#pragma once


//------------------------------------------------------------------------------------------------//
// theme�� component�� �����ϴ� �⺻ ����
// piece(image subset/sequence)�� ����
// ���¿� ���� 3������ �з���
// - eUT_Image : �� ���� piece�� ����. ũ�� ���� �Ұ�
// - eUT_Edge : �߾��� ������ 8���� piece�� ����
// - eUT_Table : �߾Ӱ� 8���� �̹���, �� 9���� piece�� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashStr.h"


class MkBaseTexture;
class MkSceneNode;

class MkWindowThemeUnitData
{
public:

	enum eUnitType
	{
		eUT_None = -1,
		eUT_Image = 0,
		eUT_Edge,
		eUT_Table
	};

	enum ePosition // eUT_Edge & eUT_Table
	{
		eP_LT = 0, // left-top
		eP_MT, // middle-Top
		eP_RT, // right-top
		eP_LC, // left-center
		eP_MC, // middle-center
		eP_RC, // right-center
		eP_LB, // left-bottom
		eP_MB, // middle-bottom
		eP_RB // right-bottom
	};

	typedef struct _PieceData
	{
		MkHashStr subsetOrSequenceName;
		MkFloat2 size;
		MkFloat2 pivot;
	}
	PieceData;

public:

	// �ʱ�ȭ
	// subsetOrSequenceNameList�� ���¿� ���� unit type�� ���� ��
	// filledUnit�� false �� ��� panel�� ������ size ������ ����
	bool SetUp(const MkStr& imagePath, const MkBaseTexture* texture, const MkArray<MkHashStr>& subsetOrSequenceNameList, bool filledUnit);

	// unit type ��ȯ
	inline eUnitType GetUnitType(void) const { return m_UnitType; }

	// data ���� ��ȯ
	inline const MkArray<PieceData>& GetPieceData(void) const { return m_PieceDatas; }

	//------------------------------------------------------------------------------------------------//
	// scene node ������ interface
	//------------------------------------------------------------------------------------------------//

	// scene node�� ����� unit type ��ȯ
	static eUnitType GetUnitType(const MkSceneNode* sceneNode);

	// �� data�� ����� unit type panel���� sceneNode�� ����
	void CreateUnit(MkSceneNode* sceneNode, const MkHashStr& imagePath, double startTime) const;

	// scene node�� ����� unit type panel���� �� data�� ����
	void SetUnit(MkSceneNode* sceneNode, const MkHashStr& imagePath) const;

	// ����� ��� unit type panel���� sceneNode���� ����
	static void DeleteUnit(MkSceneNode* sceneNode);

	// sceneNode�� ���� �ش� unit type panel���� ũ�⸦ client size�� ���߾� ����
	// - eUT_Image type�� ũ�� ������ ������� �����Ƿ� �ݿ����� ����
	//   clientSize�� windowSize�� image size, clientPosition���� zero�� �־� ��ȯ
	// - eUT_Edge, eUT_Table type�� ��� edge �κ��� ����� ���� clientSize < windowSize�� ��
	//   clientSize�� �ݿ��ϰ� windowSize�� clientPosition�� ����� ��ȯ
	// (NOTE) �Ϲ����� window ������ 2��и�(top-down)�� �������� minikey�� 1��и�(down-top)�� ����
	void SetClientSize(MkSceneNode* sceneNode, MkFloat2& clientSize, MkFloat2& clientPosition, MkFloat2& windowSize) const;

	//------------------------------------------------------------------------------------------------//

	MkWindowThemeUnitData();
	~MkWindowThemeUnitData() {}

protected:

	void _ApplyUnit(MkSceneNode* sceneNode, bool createOrGet, const MkHashStr& imagePath, double startTime) const;

	static void _SetImageToPiece
		(MkSceneNode* sceneNode, bool createOrGet, const MkHashStr& pieceName,
		const MkHashStr& imagePath, const MkHashStr& subsetOrSequenceName, bool keepSrcSize, double startTime);

	static void _SetPiecePosition(MkSceneNode* sceneNode, ePosition piecePosition, const MkFloat2& panelPosition);
	static void _SetPieceSizeAndPosition(MkSceneNode* sceneNode, ePosition piecePosition, const MkFloat2& size, const MkFloat2& panelPosition);

protected:

	MkArray<PieceData> m_PieceDatas;

public:

	eUnitType m_UnitType;

	static const MkStr WindowThemeTagPrefix;

	// eUT_Image
	static const MkHashStr ImagePositionName;

	// eUT_Edge & eUT_Table : ePosition
	static const MkHashStr EdgeAndTablePositionName[9];
};
