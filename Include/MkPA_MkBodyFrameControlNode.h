#pragma once


//------------------------------------------------------------------------------------------------//
// window control node : body frame
// �θ� MkTitleBarControlNode�� �Ļ� ��ü���� ���ο� ���� ������ �ٸ�
//
// ex>
//	MkBodyFrameControlNode* titleBar = MkBodyFrameControlNode::CreateChildNode(NULL, L"TitleBar");
//	...
//	MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(titleBar, L"BodyFrame");
//	bodyFrame->SetBodyFrame
//		(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true,
//		MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(350.f, 250.f));
//------------------------------------------------------------------------------------------------//

#include "MkPA_MkWindowBaseNode.h"


class MkBodyFrameControlNode : public MkWindowBaseNode
{
public:

	// node type
	virtual ePA_SceneNodeType GetNodeType(void) const { return ePA_SNT_BodyFrameControlNode; }

	// alloc child instance
	static MkBodyFrameControlNode* CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName);

	//------------------------------------------------------------------------------------------------//
	// control interface
	//------------------------------------------------------------------------------------------------//

	// �θ� node�� �����Ǵ� ���
	enum eHangingType
	{
		eHT_None = 0,
		eHT_OverParentWindow, // �θ� window rect ��ܿ� ��ġ
		eHT_UnderParentWindow, // �θ� window rect �ϴܿ� ��ġ. �θ� title bar�� ��� �Ϲ����� ���
		eHT_IncludeParentAtTop, // �θ� node�� body frame�� client rect ��ܿ� ��ġ
		eHT_IncludeParentAtBottom // �θ� node�� body frame�� client rect �ϴܿ� ��ġ
	};

	// regular component�� ����
	void SetBodyFrame
		(const MkHashStr& themeName, MkWindowThemeData::eComponentType componentType,
		bool useShadow, eHangingType hangingType, const MkFloat2& clientSize);

	// custom form���� ����
	void SetBodyFrame
		(const MkHashStr& themeName, const MkHashStr& customFormName,
		bool useShadow, eHangingType hangingType, const MkFloat2& clientSize);

	//------------------------------------------------------------------------------------------------//
	// region
	//------------------------------------------------------------------------------------------------//

	// �θ� MkTitleBarControlNode�� �Ļ� ��ü�̰� SetBodyFrame()�� ���� ȣ��� �����̸� �θ��� ũ�⵵ ���� ��
	virtual void SetClientSize(const MkFloat2& clientSize);

	//------------------------------------------------------------------------------------------------//

	MKDEF_DECLARE_SCENE_OBJECT_HEADER; // MkSceneObject

	MkBodyFrameControlNode(const MkHashStr& name);
	virtual ~MkBodyFrameControlNode() {}

protected:

	void _SetBodyFrame
		(const MkHashStr& themeName, MkWindowThemeData::eComponentType componentType, const MkHashStr& customFormName,
		bool useShadow, eHangingType hangingType, const MkFloat2& clientSize);

	void _ApplyHangingType(void);

protected:

	eHangingType m_HangingType;

public:

	static const MkHashStr ObjKey_HangingType;
};
