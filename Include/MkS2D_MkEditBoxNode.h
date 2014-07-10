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

	// 생성
	// 폰트 타입과 일반, 선택, 커서 폰트 상태를 선택할 수 있으며 비어있을 경우 window preset에 지정되어 있는 값을 사용
	// useHistory가 true일 경우 상하키로 이전, 이후 값을 불러 올 수 있음
	bool CreateEditBox(const MkHashStr& themeName, const MkFloat2& windowSize,
		const MkHashStr& fontType = MkHashStr::NullHash, const MkHashStr& normalFontState = MkHashStr::NullHash,
		const MkHashStr& selectionFontState = MkHashStr::NullHash, const MkHashStr& cursorFontState = MkHashStr::NullHash,
		const MkStr& initMsg = MkStr::Null, bool useHistory = false);

	// 문자열 설정
	void SetText(const MkStr& msg);

	// 설정된 문자열 반환
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

	void __TakeCurrentText(void);

	inline DWORD __GetSetStart(void) const { return m_SelStart; }
	inline DWORD __GetSetEnd(void) const { return m_SelEnd; }

protected:

	void _SetFontInfo(const MkHashStr& fontType, const MkHashStr& normalFontState, const MkHashStr& selectionFontState, const MkHashStr& cursorFontState);

	void _UpdateCursorAndSelection(void);

	void _DeleteTextRect(void);
	void _UpdateNormalCursor(MkSRect* cursorRect);
	void _UpdateSelectionRegion(MkSRect* selRect);

	float _GetTextWidth(DWORD beginPos, DWORD endPos, const MkStr& text) const;
	inline float _GetTextWidth(DWORD beginPos, DWORD endPos) const { return _GetTextWidth(beginPos, endPos, m_Text); }

	const MkHashStr& _GetFontType(void) const;
	const MkHashStr& _GetNormalFontState(void) const;
	const MkHashStr& _GetSelectionFontState(void) const;
	const MkHashStr& _GetCursorFontState(void) const;

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

	DWORD m_WindowOffset;
};
