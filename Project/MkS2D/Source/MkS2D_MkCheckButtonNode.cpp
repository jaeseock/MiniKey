
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
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
		const float MARGIN = MK_WR_PRESET.GetMargin();

		MkFloat2 iconPos(MARGIN, MARGIN);
		unchkNode->SetLocalPosition(iconPos);
		unchkNode->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
		unchkNode->SetAttribute(eIgnoreMovement, true);
		checkNode->SetLocalPosition(iconPos);
		checkNode->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
		checkNode->SetAttribute(eIgnoreMovement, true);

		m_OnCheck = onCheck;
		unchkNode->SetVisible(!m_OnCheck);
		checkNode->SetVisible(m_OnCheck);

		if (CreateWindowPreset(themeName, eS2D_WPC_BackgroundWindow, checkNode->GetPresetFullSize())) // icon size -> body size
		{
			m_CaptionDesc = captionDesc;
			m_CaptionDesc.__Check(GetNodeName().GetString());
			float captionPos = GetPresetFullSize().x + MARGIN * 2.f; // blank size
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

		_PushWindowEvent(MkSceneNodeFamilyDefinition::eCheckOff);
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

		_PushWindowEvent(MkSceneNodeFamilyDefinition::eCheckOn);
	}

	m_OnCheck = enable;
}

void MkCheckButtonNode::Load(const MkDataNode& node)
{
	MkArray<MkStr> captions;
	node.GetData(MkSceneNodeFamilyDefinition::CheckButton::CaptionKey, captions);
	m_CaptionDesc.__Load(GetNodeName().GetString(), captions);

	node.GetData(MkSceneNodeFamilyDefinition::CheckButton::OnCheckKey, m_OnCheck, 0);

	// MkBaseWindowNode
	MkBaseWindowNode::Load(node);
}

void MkCheckButtonNode::Save(MkDataNode& node)
{
	// 기존 템플릿이 없으면 템플릿 적용
	_ApplyBuildingTemplateToSave(node, MkSceneNodeFamilyDefinition::CheckButton::TemplateName);

	MkArray<MkStr> captions;
	m_CaptionDesc.__Save(captions);
	node.SetData(MkSceneNodeFamilyDefinition::CheckButton::CaptionKey, captions);

	node.SetData(MkSceneNodeFamilyDefinition::CheckButton::OnCheckKey, m_OnCheck, 0);

	// MkBaseWindowNode
	MkBaseWindowNode::Save(node);
}

bool MkCheckButtonNode::InputEventMouseRelease(unsigned int button, const MkFloat2& position, bool managedRoot)
{
	if ((button == 0) && GetWindowRect().CheckGridIntersection(position)) // left release
	{
		SetCheck(!m_OnCheck); // toggle
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

	tNode->CreateUnit(MkSceneNodeFamilyDefinition::CheckButton::CaptionKey, MkStr::Null);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::CheckButton::OnCheckKey, false);

	tNode->DeclareToTemplate(true);
}

bool MkCheckButtonNode::_CheckCursorHitCondition(const MkFloat2& position) const
{
	return (IsBackgroundStateType(MkWindowPreset::GetWindowPresetComponentEnum(m_PresetComponentName)) && GetWindowRect().CheckGridIntersection(position));
}

//------------------------------------------------------------------------------------------------//