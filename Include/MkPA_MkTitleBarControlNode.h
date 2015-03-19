#pragma once


//------------------------------------------------------------------------------------------------//
// window base node
// window system�� action part�� base class
//
// ex>
//	MkTitleBarControlNode* titleBar = MkTitleBarControlNode::CreateChildNode(NULL, L"TitleBar");
//
//	titleBar->SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
//	titleBar->SetIcon(MkWindowThemeData::eIT_Notice); // system icon
//	//titleBar->SetIcon(MkWindowThemeData::eIT_CustomTag, L"Default\\theme_default.png", L"BtnRI16N"); // custom icon
//	titleBar->SetCaption(L"�ڷ��� Ÿ��Ʋ����!!!", eRAP_LeftCenter);
//
//	winMgrNode->AttachWindow(titleBar, MkWindowManagerNode::eLT_Normal);
//	winMgrNode->ActivateWindow(L"TitleBar");
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowBaseNode.h"


class MkTitleBarControlNode : public MkWindowBaseNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_TitleBarControlNode; }

	// alloc child instance
	static MkTitleBarControlNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// control interface
	//------------------------------------------------------------------------------------------------//

	// title bar ����
	void SetTitleBar(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, bool useCloseBtn);

	// icon ����
	// iconType�� eIT_None�� ��� icon ����
	// iconType�� eIT_CustomTag�� ��� iconPath, iconSubsetOrSequenceName�� ���� icon ���� ����
	// (NOTE) ȣ�� �� SetTitleBar()�� �ùٸ��� ȣ��� �����̾�� ��
	void SetIcon(MkWindowThemeData::eIconType iconType, const MkHashStr& imagePath = MkHashStr::EMPTY, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY);

	// caption ����
	// caption�� ��� ���� ��� ����
	// (NOTE) ȣ�� �� SetTitleBar()�� �ùٸ��� ȣ��� �����̾�� ��
	void SetCaption(const MkStr& caption, eRectAlignmentPosition position = eRAP_LeftCenter);

	//------------------------------------------------------------------------------------------------//

	MkTitleBarControlNode(const MkHashStr& name);
	virtual ~MkTitleBarControlNode() {}

public:

	static const MkHashStr CloseButtonNodeName;
	static const MkHashStr IconNodeName;
	static const MkHashStr CaptionNodeName;
};