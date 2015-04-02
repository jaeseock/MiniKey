#pragma once


//------------------------------------------------------------------------------------------------//
// window control node : edit box
//
// ex>
//	MkTitleBarControlNode* titleBar = MkTitleBarControlNode::CreateChildNode(NULL, L"TitleBar");
//
//	titleBar->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Medium, 300.f, true);
//	titleBar->SetIcon(MkWindowThemeData::eIT_Notice); // system icon
//	//titleBar->SetIcon(MkWindowThemeData::eIT_CustomTag, L"Default\\theme_default.png", L"BtnRI16N"); // custom icon
//	titleBar->SetCaption(L"코레가 타이틀데스!!!", eRAP_LeftCenter);
//
//	winMgrNode->AttachWindow(titleBar, MkWindowManagerNode::eLT_Normal);
//	winMgrNode->ActivateWindow(L"TitleBar");
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

	void SetSingleLineEditBox(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, const MkStr& initMsg, bool useHistory = false);

	void SetText(const MkStr& msg);
	inline const MkStr& GetText(void) const { return m_Text; }

	void CommitText(void);

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);

	//------------------------------------------------------------------------------------------------//

	MkEditBoxControlNode(const MkHashStr& name);
	virtual ~MkEditBoxControlNode() {}

	bool __UpdateText(const MkStr& msg, int selStart, int selEnd);

	bool __StepBackMsgHistory(void);
	bool __StepForwardMsgHistory(void);

protected:

	float _GetTextLength(const MkHashStr& fontType, const MkStr& text, unsigned int beginPos, unsigned int endPos) const;

protected:

	MkStr m_Text;
	bool m_UseHistory;

	MkMessageHistory m_MessageHistory;
	int m_SelStart;
	int m_SelEnd;

public:

	static const MkHashStr TextPanelName;
	static const MkHashStr CursorPanelName;
	static const MkHashStr SelectionPanelName;

	static const MkHashStr ArgKey_Text;
};
