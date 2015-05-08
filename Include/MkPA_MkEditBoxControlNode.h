#pragma once


//------------------------------------------------------------------------------------------------//
// window control node : edit box
//
// ex>
//	MkEditBoxControlNode* ebNode = MkEditBoxControlNode::CreateChildNode(sbodyFrame, L"EB");
//	ebNode->SetSingleLineEditBox(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 210.f, L"�Ʊ� �ٸ�", true);
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMessageHistory.h"
#include "MkPA_MkWindowBaseNode.h"


class MkEditBoxControlNode : public MkWindowBaseNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_EditBoxControlNode; }

	// alloc child instance
	static MkEditBoxControlNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// control interface
	//------------------------------------------------------------------------------------------------//

	// single line edit box ����
	void SetSingleLineEditBox(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, const MkStr& initMsg, bool useHistory = false);

	// text ����
	void SetText(const MkStr& msg);

	// �� text ��ȯ
	inline const MkStr& GetText(void) const { return m_Text; }

	// �� text�� ���� ����
	// ePA_SNE_TextCommitted�� �߻��ϰ� history�� �߰���
	void CommitText(void);

	//------------------------------------------------------------------------------------------------//

	MkEditBoxControlNode(const MkHashStr& name);
	virtual ~MkEditBoxControlNode() {}

	void __GainInputFocus(void);
	void __LostInputFocus(void);

	void __UpdateText(const MkStr& msg, int selStart, int selEnd);

	void __StepBackMsgHistory(void);
	void __StepForwardMsgHistory(void);

	void __ToggleNormalCursor(void);

	inline int __GetSelStart(void) const { return m_SelStart; }
	inline int __GetSelEnd(void) const { return m_SelEnd; }

protected:

	MkFloat2 _GetFormMargin(void) const;

	float _GetTextLength(const MkHashStr& fontType, const MkStr& text, int beginPos, int endPos) const;

	MkPanel* _SetPanelEnable(const MkHashStr& name, bool enable);

	void _UpdateCursorAndSelection(void);

protected:

	MkStr m_Text;
	bool m_UseHistory;

	MkMessageHistory m_MessageHistory;
	int m_SelStart;
	int m_SelEnd;

public:

	static const MkHashStr SelectionPanelName;
	static const MkHashStr TextPanelName;
	static const MkHashStr CursorPanelName;
	
	static const MkHashStr ArgKey_Text;
};
