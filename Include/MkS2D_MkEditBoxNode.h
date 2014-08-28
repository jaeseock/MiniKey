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

	// ����
	// ��Ʈ Ÿ�԰� �Ϲ�, ����, Ŀ�� ��Ʈ ���¸� ������ �� ������ ������� ��� window preset�� �����Ǿ� �ִ� ���� ���
	// useHistory�� true�� ��� ����Ű�� ����, ���� ���� �ҷ� �� �� ����
	bool CreateEditBox(const MkHashStr& themeName, const MkFloat2& windowSize,
		const MkHashStr& fontType = MkHashStr::NullHash, const MkHashStr& normalFontState = MkHashStr::NullHash,
		const MkHashStr& selectionFontState = MkHashStr::NullHash, const MkHashStr& cursorFontState = MkHashStr::NullHash,
		const MkStr& initMsg = MkStr::Null, bool useHistory = false);

	// history ��� ���� ����
	void SetHistoryUsage(bool enable);

	// history ��� ���� ��ȯ
	inline bool SetHistoryUsage(void) const { return m_UseHistory; }

	// ��Ʈ ���� ����
	void SetFontInfo(const MkHashStr& fontType, const MkHashStr& normalFontState, const MkHashStr& selectionFontState, const MkHashStr& cursorFontState);

	// ���ڿ� ����. pushEvent�� MkSceneNodeFamilyDefinition::eModifyText �߻� ����
	void SetText(const MkStr& msg, bool pushEvent = true);

	// ���ڿ� ���� �̺�Ʈ �߻�. history�� ���
	void CommitText(void);

	// ������ ���ڿ� ��ȯ
	inline const MkStr& GetText(void) const { return m_Text; }

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// MkDataNode�� ����. MkSceneNode()�� ���� ��Ģ�� ����
	virtual void Load(const MkDataNode& node);

	// MkDataNode�� ���
	virtual void Save(MkDataNode& node);

	//------------------------------------------------------------------------------------------------//

	MkEditBoxNode(const MkHashStr& name);
	virtual ~MkEditBoxNode() {}

public:

	static void __GenerateBuildingTemplate(void);

	bool __StepBackMsgHistory(void);
	bool __StepForwardMsgHistory(void);

	void __BindingLost(void);
	void __SetFocus(void);

	bool __UpdateTextInfo(const MkStr& msg, DWORD selStart, DWORD selEnd, bool pushEvent);

	void __ToggleCursorRect(void);

	inline DWORD __GetSetStart(void) const { return m_SelStart; }
	inline DWORD __GetSetEnd(void) const { return m_SelEnd; }

protected:

	void _UpdateCursorAndSelection(void);

	void _DeleteTextRect(void);

	float _GetTextWidth(DWORD beginPos, DWORD endPos, const MkStr& text) const;

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

	DWORD m_CharStart;
	DWORD m_CharEnd;

	bool m_ShowCursorRect;
};
