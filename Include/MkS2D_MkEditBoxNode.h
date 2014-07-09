#pragma once


//------------------------------------------------------------------------------------------------//
// single line edit box node : MkBaseWindowNode
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMessageHistory.h"
#include "MkS2D_MkBaseWindowNode.h"


class MkEditBoxNode : public MkBaseWindowNode
{
public:

	virtual eS2D_SceneNodeType GetNodeType(void) const { return eS2D_SNT_EditBoxNode; }

	//------------------------------------------------------------------------------------------------//
	// control
	//------------------------------------------------------------------------------------------------//

	bool CreateEditBox(const MkHashStr& themeName, const MkFloat2& windowSize,
		const MkHashStr& fontType = L"", const MkHashStr& normalFontState = L"", const MkHashStr& selectionFontState = L"", const MkHashStr& cursorFontState = L"",
		const MkStr& initMsg = L"", bool useHistory = false);

	void SetText(const MkStr& msg);

	inline const MkStr& GetText(void) const { return m_Text; }

	//------------------------------------------------------------------------------------------------//
	// 구성
	//------------------------------------------------------------------------------------------------//

	// MkDataNode로 구성. MkSceneNode()의 구성 규칙과 동일
	virtual void Load(const MkDataNode& node);

	// MkDataNode로 출력
	virtual void Save(MkDataNode& node);

	//------------------------------------------------------------------------------------------------//
	// event call
	//------------------------------------------------------------------------------------------------//

	//virtual bool CheckCursorHitCondition(const MkFloat2& position) const;

	//virtual bool HitEventMouseRelease(unsigned int button, const MkFloat2& position);

	//------------------------------------------------------------------------------------------------//

	MkEditBoxNode(const MkHashStr& name);
	virtual ~MkEditBoxNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	bool __StepBackMsgHistory(void);
	bool __StepForwardMsgHistory(void);

	void __BindingLost(void);
	void __SetFocus(void);

	bool __UpdateTextInfo(const MkStr& msg, DWORD setStart, DWORD setEnd);

	inline DWORD __GetSetStart(void) const { return m_SelStart; }
	inline DWORD __GetSetEnd(void) const { return m_SelEnd; }

protected:

	void _SetFontInfo(const MkHashStr& fontType, const MkHashStr& normalFontState, const MkHashStr& selectionFontState, const MkHashStr& cursorFontState);

	void _UpdateCursor(void);

	const MkHashStr& _GetFontType(void) const;
	const MkHashStr& _GetFontState(const MkHashStr& fontState) const;

protected:

	MkStr m_Text;
	bool m_UseHistory;
	MkHashStr m_FontType;
	MkHashStr m_NormalFontState;
	MkHashStr m_SelectionFontState;
	MkHashStr m_CursorFontState;

	MkMessageHistory m_MessageHistory;
	DWORD m_SelStart;
	DWORD m_SelEnd;
};
