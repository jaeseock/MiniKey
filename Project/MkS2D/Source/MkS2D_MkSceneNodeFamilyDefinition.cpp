
#include "MkCore_MkCheck.h"

#include "MkS2D_MkProjectDefinition.h"

#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkBaseWindowNode.h"
#include "MkS2D_MkSpreadButtonNode.h"
#include "MkS2D_MkCheckButtonNode.h"
#include "MkS2D_MkScrollBarNode.h"

#include "MkS2D_MkSceneNodeFamilyDefinition.h"

//------------------------------------------------------------------------------------------------//

const MkHashStr MkSceneNodeFamilyDefinition::Scene::TemplateName(MKDEF_S2D_BT_SCENENODE_TEMPLATE_NAME);
const MkHashStr MkSceneNodeFamilyDefinition::Scene::PositionKey(L"Position");
const MkHashStr MkSceneNodeFamilyDefinition::Scene::RotationKey(L"Rotation");
const MkHashStr MkSceneNodeFamilyDefinition::Scene::ScaleKey(L"Scale");
const MkHashStr MkSceneNodeFamilyDefinition::Scene::AlphaKey(L"Alpha");
const MkHashStr MkSceneNodeFamilyDefinition::Scene::VisibleKey(L"Visible");

const MkHashStr MkSceneNodeFamilyDefinition::BaseWindow::TemplateName(MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME);
const MkHashStr MkSceneNodeFamilyDefinition::BaseWindow::EnableKey(L"Enable");
const MkHashStr MkSceneNodeFamilyDefinition::BaseWindow::PresetThemeNameKey(L"PThemeName");
const MkHashStr MkSceneNodeFamilyDefinition::BaseWindow::PresetComponentKey(L"PComponent");
const MkHashStr MkSceneNodeFamilyDefinition::BaseWindow::PresetComponentSizeKey(L"PCompSize");
const MkHashStr MkSceneNodeFamilyDefinition::BaseWindow::AttributeKey(L"Attribute");

const MkHashStr MkSceneNodeFamilyDefinition::SpreadButton::TemplateName(MKDEF_S2D_BT_SPREADBUTTON_TEMPLATE_NAME);
const MkHashStr MkSceneNodeFamilyDefinition::SpreadButton::ButtonTypeKey(L"ButtonType");
const MkHashStr MkSceneNodeFamilyDefinition::SpreadButton::OpeningDirKey(L"OpeningDir");
const MkHashStr MkSceneNodeFamilyDefinition::SpreadButton::IconPathKey(L"IconPath");
const MkHashStr MkSceneNodeFamilyDefinition::SpreadButton::IconSubsetKey(L"IconSubset");
const MkHashStr MkSceneNodeFamilyDefinition::SpreadButton::CaptionKey(L"Caption");
const MkHashStr MkSceneNodeFamilyDefinition::SpreadButton::ItemSeqKey(L"ItemSeq");
const MkHashStr MkSceneNodeFamilyDefinition::SpreadButton::TargetButtonKey(L"TargetButton");

const MkHashStr MkSceneNodeFamilyDefinition::CheckButton::TemplateName(MKDEF_S2D_BT_CHECKBUTTON_TEMPLATE_NAME);
const MkHashStr MkSceneNodeFamilyDefinition::CheckButton::CaptionKey(L"Caption");
const MkHashStr MkSceneNodeFamilyDefinition::CheckButton::OnCheckKey(L"OnCheck");

const MkHashStr MkSceneNodeFamilyDefinition::ScrollBar::TemplateName(MKDEF_S2D_BT_SCROLLBAR_TEMPLATE_NAME);
const MkHashStr MkSceneNodeFamilyDefinition::ScrollBar::BarDirectionKey(L"BarDirection");


void MkSceneNodeFamilyDefinition::GenerateBuildingTemplate(void)
{
	MkSceneNode::__GenerateBuildingTemplate();
	MkBaseWindowNode::__GenerateBuildingTemplate();
	MkSpreadButtonNode::__GenerateBuildingTemplate();
	MkCheckButtonNode::__GenerateBuildingTemplate();
	MkScrollBarNode::__GenerateBuildingTemplate();
}

MkSceneNode* MkSceneNodeFamilyDefinition::Alloc(const MkHashStr& templateName, const MkHashStr& nodeName)
{
	MkSceneNode* instance = NULL;

	if (templateName == Scene::TemplateName)
	{
		instance = new MkSceneNode(nodeName);
	}
	else if (templateName == BaseWindow::TemplateName)
	{
		instance = new MkBaseWindowNode(nodeName);
	}
	else if (templateName == SpreadButton::TemplateName)
	{
		instance = new MkSpreadButtonNode(nodeName);
	}
	else if (templateName == CheckButton::TemplateName)
	{
		instance = new MkCheckButtonNode(nodeName);
	}
	else if (templateName == ScrollBar::TemplateName)
	{
		instance = new MkScrollBarNode(nodeName);
	}
	
	MK_CHECK(instance != NULL, templateName.GetString() + L" 템플릿을 사용하는 " + nodeName.GetString() + L" 노드 alloc 실패") {}

	return instance;
}

//------------------------------------------------------------------------------------------------//