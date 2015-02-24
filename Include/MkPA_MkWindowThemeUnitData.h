#pragma once


//------------------------------------------------------------------------------------------------//
// theme의 component를 구성하는 기본 단위
// piece(image subset/sequence)의 집합
// 형태에 따라 3가지로 분류됨
// - eUT_Image : 한 장의 piece로 구성. 크기 변경 불가
// - eUT_Edge : 중앙을 제외한 8방향 piece로 구성
// - eUT_Table : 중앙과 8방향 이미지, 총 9개의 piece로 구성
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

	// 초기화
	// subsetOrSequenceNameList의 형태에 따라 unit type이 결정 됨
	// filledUnit이 false 일 경우 panel을 구성할 size 정보만 생성
	bool SetUp(const MkStr& imagePath, const MkBaseTexture* texture, const MkArray<MkHashStr>& subsetOrSequenceNameList, bool filledUnit);

	// unit type 반환
	inline eUnitType GetUnitType(void) const { return m_UnitType; }

	// data 참조 반환
	inline const MkArray<PieceData>& GetPieceData(void) const { return m_PieceDatas; }

	//------------------------------------------------------------------------------------------------//
	// scene node 구성용 interface
	//------------------------------------------------------------------------------------------------//

	// scene node에 적용된 unit type 반환
	static eUnitType GetUnitType(const MkSceneNode* sceneNode);

	// 현 data가 적용된 unit type panel들을 sceneNode에 생성
	void CreateUnit(MkSceneNode* sceneNode, const MkHashStr& imagePath, double startTime) const;

	// scene node에 적용된 unit type panel들을 현 data로 변경
	void SetUnit(MkSceneNode* sceneNode, const MkHashStr& imagePath) const;

	// 적용된 모든 unit type panel들을 sceneNode에서 제거
	static void DeleteUnit(MkSceneNode* sceneNode);

	// sceneNode에 속한 해당 unit type panel들의 크기를 client size에 맞추어 설정
	// - eUT_Image type은 크기 변경이 허락되지 않으므로 반영되지 않음
	//   clientSize와 windowSize에 image size, clientPosition에는 zero를 넣어 반환
	// - eUT_Edge, eUT_Table type의 경우 edge 부분의 존재로 인해 clientSize < windowSize가 됨
	//   clientSize는 반영하고 windowSize과 clientPosition는 계산해 반환
	// (NOTE) 일반적인 window 구성은 2사분면(top-down)을 따르지만 minikey는 1사분면(down-top)을 따름
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
