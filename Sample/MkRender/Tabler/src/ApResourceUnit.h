#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkPathName.h"
#include "MkCore_MkTimeCounter.h"

#include "ApResourceType.h"


class MkSceneNode;

class ApResourceUnit
{
public:

	enum eBackgroundType
	{
		eBT_None = 0,
		eBT_Disable,
		eBT_Neutral,
		eBT_Owned,
		eBT_Combo
	};

	enum eRowColumnType
	{
		eRCT_None = 0,
		eRCT_Light,
		eRCT_Dark
	};

	enum eTeamColor
	{
		eTC_None = 0,
		eTC_Red,
		eTC_Green,
		eTC_Yellow,
		eTC_Cyan
	};

	enum eSetType
	{
		eST_None = 0,
		eST_2Set,
		eST_3Set,
		eST_4Set,
		eST_5Set
	};

	bool SetUp(const ApResourceType& type, MkSceneNode* sceneNode, const MkPathName& imagePath);

	bool SetBackgroundType(eBackgroundType bgType); // L"BG"
	inline eBackgroundType GetBackgroundType(void) const { return m_BackgroundType; }

	bool SetRowColumnType(eRowColumnType rcType); // L"Row" / L"Column"
	inline eRowColumnType GetRowColumnType(void) const { return m_RowColumnType; }

	bool SetTeamColor(eTeamColor teamColor); // L"TeamColor"
	inline eTeamColor GetTeamColor(void) const { return m_TeamColor; }

	bool SetSetType(eSetType setType); // L"Set"
	inline eSetType GetSetType(void) const { return m_SetType; }

	// effect
	void ApplyCreationEffect(const MkTimeState& timeState);

	// field command
	bool FieldCommand_Disable(void);
	bool FieldCommand_Neutral(void);
	bool FieldCommand_Own(eTeamColor teamColor);
	bool FieldCommand_Upgrade(void);
	bool FieldCommand_Combo(void);

	// player command
	bool PlayerCommand_Wait(void);
	bool PlayerCommand_Pick(void);

	inline const ApResourceType& GetType(void) const { return m_Type; }
	//inline MkSceneNode* GetSceneNode(void) const { return m_SceneNode; }

	bool HitTest(const MkFloat2& hitPos) const;

	void Update(const MkTimeState& timeState);

	ApResourceUnit();

protected:

	bool _AddImagePanel(const MkHashStr& name, const MkPathName& imgPath, const MkHashStr& ssName, float localDepth);
	bool _AddImagePanelToCenter(const MkHashStr& name, const MkPathName& imgPath, const MkHashStr& ssName, float localDepth);
	bool _AddImagePanelToLeftTop(const MkHashStr& name, const MkPathName& imgPath, const MkHashStr& ssName, float localDepth);
	bool _AddImagePanelToRightBottom(const MkHashStr& name, const MkPathName& imgPath, const MkHashStr& ssName, float localDepth);
	bool _UpdateImagePanelToCenter(const MkHashStr& name, const MkHashStr& ssName, float localDepth);

protected:

	ApResourceType m_Type;
	MkPathName m_ImagePath;
	MkSceneNode* m_SceneNode;
	
	eBackgroundType m_BackgroundType;
	eRowColumnType m_RowColumnType;
	eTeamColor m_TeamColor;
	eSetType m_SetType;

	MkTimeCounter m_TimeCounter;
};
