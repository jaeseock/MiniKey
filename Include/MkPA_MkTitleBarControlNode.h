#pragma once


//------------------------------------------------------------------------------------------------//
// window control node : title
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

	// title bar 설정
	void SetTitleBar(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, bool useCloseBtn);

	// close button 사용 설정
	void UseCloseButton(bool enable);

	// icon 설정
	// iconType이 eIT_None일 경우 icon 삭제
	// iconType이 eIT_CustomTag일 경우 iconPath, iconSubsetOrSequenceName로 전용 icon 설정 가능
	// (NOTE) 호출 전 SetTitleBar()가 올바르게 호출된 상태이어야 함
	void SetIcon(MkWindowThemeData::eIconType iconType, const MkHashStr& imagePath = MkHashStr::EMPTY, const MkHashStr& subsetOrSequenceName = MkHashStr::EMPTY);

	// static text node로 caption 설정
	// textNode가 비어 있을 경우 삭제
	// (NOTE) 호출 전 SetTitleBar()가 올바르게 호출된 상태이어야 함
	void SetCaption(const MkArray<MkHashStr>& textNode, eRectAlignmentPosition position = eRAP_LeftCenter);

	// text node는 theme를 따르고 caption을 삽입
	// (NOTE) 호출 전 SetTitleBar()가 올바르게 호출된 상태이어야 함
	void SetCaption(const MkStr& caption, eRectAlignmentPosition position = eRAP_LeftCenter);

	//------------------------------------------------------------------------------------------------//
	// event
	//------------------------------------------------------------------------------------------------//

	virtual void SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument);


	//------------------------------------------------------------------------------------------------//
	// MkSceneObject
	//------------------------------------------------------------------------------------------------//

	virtual void Save(MkDataNode& node) const;

	MKDEF_DECLARE_SCENE_OBJECT_HEADER;

	// 해제
	virtual void Clear(void);

	MkTitleBarControlNode(const MkHashStr& name);
	virtual ~MkTitleBarControlNode() {}

protected:

	void _SetCaption(const MkArray<MkHashStr>& textNode, const MkStr& caption, eRectAlignmentPosition position);

protected:

	MkWindowThemeData::eIconType m_IconType;

	MkStr m_CaptionString;

public:

	static const MkHashStr CloseButtonNodeName;
	static const MkHashStr IconNodeName;
	static const MkHashStr CaptionNodeName;

	static const MkHashStr ObjKey_UseCloseBtn;
	static const MkHashStr ObjKey_IconType;
	static const MkHashStr ObjKey_IconPath;
	static const MkHashStr ObjKey_IconSOSName;
	static const MkHashStr ObjKey_CaptionTextName;
	static const MkHashStr ObjKey_CaptionString;
	static const MkHashStr ObjKey_CaptionAlignPos;
};
