
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
//#include "MkS2D_MkWindowEventManager.h"
#include "MkS2D_MkCheckButtonNode.h"
#include "MkS2D_MkSceneNodeFamilyDefinition.h"


const static MkHashStr UNCHK_NODE_NAME = L"Uncheck";
const static MkHashStr CHECK_NODE_NAME = L"Check";
const static MkHashStr CAPTION_RECT_NAME = L"Caption";

//------------------------------------------------------------------------------------------------//

bool MkCheckButtonNode::CreateCheckButton(const MkHashStr& themeName, const CaptionDesc& captionDesc, bool onCheck)
{
	MkBaseWindowNode* unchkNode = __CreateWindowPreset(this, UNCHK_NODE_NAME, themeName, eS2D_WPC_UncheckButton, MkFloat2(0.f, 0.f));
	MkBaseWindowNode* checkNode = __CreateWindowPreset(this, CHECK_NODE_NAME, themeName, eS2D_WPC_CheckButton, MkFloat2(0.f, 0.f));
	if ((unchkNode != NULL) && (checkNode != NULL))
	{
		MkFloat2 iconPos(MKDEF_CHECK_BUTTON_SIZE_OFFSET / 2.f, MKDEF_CHECK_BUTTON_SIZE_OFFSET / 2.f);
		unchkNode->SetLocalPosition(iconPos);
		unchkNode->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
		unchkNode->SetAttribute(eIgnoreMovement, true);
		checkNode->SetLocalPosition(iconPos);
		checkNode->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
		checkNode->SetAttribute(eIgnoreMovement, true);

		m_OnCheck = onCheck;
		unchkNode->SetVisible(!m_OnCheck);
		checkNode->SetVisible(m_OnCheck);

		float sizeOffset = MKDEF_CHECK_BUTTON_SIZE_OFFSET - 2.f * MK_WR_PRESET.GetMargin();
		MkFloat2 bodySize = checkNode->GetPresetFullSize() + MkFloat2(sizeOffset, sizeOffset);
		if (CreateWindowPreset(themeName, eS2D_WPC_BackgroundWindow, bodySize))
		{
			m_CaptionDesc = captionDesc;
			m_CaptionDesc.__Check(GetNodeName().GetString());
			float captionPos = checkNode->GetPresetFullSize().x + MKDEF_CHECK_BUTTON_SIZE_OFFSET + MK_WR_PRESET.GetMargin();
			return SetPresetComponentIcon(CAPTION_RECT_NAME, eRAP_LeftCenter, MkFloat2(captionPos, 0.f), 0.f, MkHashStr::NullHash, m_CaptionDesc);
		}
	}

	return false;
}

void MkCheckButtonNode::SetCheck(bool enable)
{
	if (m_OnCheck && (!enable)) // on -> off
	{
		MkSceneNode* unchkNode = GetChildNode(UNCHK_NODE_NAME);
		MkSceneNode* checkNode = GetChildNode(CHECK_NODE_NAME);
		if ((unchkNode != NULL) && (checkNode != NULL))
		{
			unchkNode->SetVisible(true);
			checkNode->SetVisible(false);
		}
	}
	else if ((!m_OnCheck) && enable) // off -> on
	{
		MkSceneNode* unchkNode = GetChildNode(UNCHK_NODE_NAME);
		MkSceneNode* checkNode = GetChildNode(CHECK_NODE_NAME);
		if ((unchkNode != NULL) && (checkNode != NULL))
		{
			unchkNode->SetVisible(false);
			checkNode->SetVisible(true);
		}
	}

	m_OnCheck = enable;
}

void MkCheckButtonNode::Load(const MkDataNode& node)
{
	//unsigned int btnType = 0;
	//node.GetData(MkSceneNodeFamilyDefinition::SpreadButton::ButtonTypeKey, btnType, 0);
	//m_ButtonType = static_cast<eSpreadButtonType>(btnType);
	/*
	MkArray<MkStr> captions;
	node.GetData(MkSceneNodeFamilyDefinition::SpreadButton::CaptionKey, captions);
	if (!captions.Empty())
	{
		m_ItemTagInfo.captionStr = captions[0];

		unsigned int cnt = captions.GetSize();
		if (cnt > 1)
		{
			m_ItemTagInfo.captionList.Reserve(cnt - 1);
			for (unsigned int i=1; i<cnt; ++i)
			{
				m_ItemTagInfo.captionList.PushBack(captions[i]);
			}
		}
	}
	*/

	// MkBaseWindowNode
	MkBaseWindowNode::Load(node);
}

void MkCheckButtonNode::Save(MkDataNode& node)
{
	// 기존 템플릿이 없으면 템플릿 적용
	_ApplyBuildingTemplateToSave(node, MkSceneNodeFamilyDefinition::CheckButton::TemplateName);

	//node.SetData(MkSceneNodeFamilyDefinition::SpreadButton::ButtonTypeKey, static_cast<unsigned int>(m_ButtonType), 0);
	/*
	MkArray<MkStr> captions(1 + m_ItemTagInfo.captionList.GetSize());
	captions.PushBack(m_ItemTagInfo.captionStr);
	MK_INDEXING_LOOP(m_ItemTagInfo.captionList, i)
	{
		captions.PushBack(m_ItemTagInfo.captionList[i]);
	}
	node.SetData(MkSceneNodeFamilyDefinition::SpreadButton::CaptionKey, captions);
	*/

	// MkBaseWindowNode
	MkBaseWindowNode::Save(node);
}

bool MkCheckButtonNode::InputEventMouseRelease(unsigned int button, const MkFloat2& position, bool managedRoot)
{
	if ((button == 0) && GetWindowRect().CheckGridIntersection(position)) // left release
	{
		SetCheck(!m_OnCheck); // toggle
		return true;
	}

	return MkBaseWindowNode::InputEventMouseRelease(button, position, managedRoot);
}

MkCheckButtonNode::MkCheckButtonNode(const MkHashStr& name) : MkBaseWindowNode(name)
{
	m_OnCheck = false;
}

//------------------------------------------------------------------------------------------------//

void MkCheckButtonNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(MkSceneNodeFamilyDefinition::CheckButton::TemplateName);
	MK_CHECK(tNode != NULL, MkSceneNodeFamilyDefinition::CheckButton::TemplateName.GetString() + L" template node alloc 실패")
		return;

	tNode->ApplyTemplate(MkSceneNodeFamilyDefinition::BaseWindow::TemplateName); // MkBaseWindowNode의 template 적용

	//tNode->CreateUnit(MkSceneNodeFamilyDefinition::SpreadButton::IconPathKey, MkStr::Null);

	tNode->DeclareToTemplate(true);
}

//------------------------------------------------------------------------------------------------//