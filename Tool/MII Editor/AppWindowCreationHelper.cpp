
#include "MkPA_MkEditBoxControlNode.h"
#include "MkPA_MkCheckBoxControlNode.h"
#include "MkPA_MkListBoxControlNode.h"
#include "MkPA_MkDropDownListControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkWindowFactory.h"

#include "AppWindowCreationHelper.h"


//------------------------------------------------------------------------------------------------//

MkWindowThemedNode* AppWindowCreationHelper::AddStaticGuide(MkSceneNode* parentNode, const MkHashStr& name, float posX, float posY, float width, float height)
{
	MkWindowThemedNode* guideNode = MkWindowThemedNode::CreateChildNode(parentNode, name);
	guideNode->SetThemeName(MkWindowThemeData::DefaultThemeName);
	guideNode->SetComponentType(MkWindowThemeData::eCT_GuideBox);
	guideNode->SetClientSize(MkFloat2(width, height));
	guideNode->SetFormState(MkWindowThemeFormData::eS_Default);
	guideNode->SetLocalPosition(MkFloat2(posX, posY));
	guideNode->SetLocalDepth(-1.f);
	return guideNode;
}

void AppWindowCreationHelper::AddNormalButton(MkSceneNode* parentNode, const MkHashStr& name, const MkStr& message, float posX, float posY, float minWidth)
{
	MkWindowFactory wndFactory;
	wndFactory.SetMinClientSizeForButton(MkFloat2(minWidth, 20.f));
	MkWindowBaseNode* winNode = wndFactory.CreateNormalButtonNode(name, message);
	winNode->SetLocalPosition(MkFloat2(posX, posY));
	parentNode->AttachChildNode(winNode);
}

void AppWindowCreationHelper::AddOkButton(MkSceneNode* parentNode, const MkStr& message, float posX, float posY, float minWidth, float minHeight)
{
	MkWindowFactory wndFactory;
	wndFactory.SetMinClientSizeForButton(MkFloat2(minWidth, minHeight));
	MkWindowBaseNode* winNode = wndFactory.CreateOkButtonNode(message);
	winNode->SetLocalPosition(MkFloat2(posX, posY));
	parentNode->AttachChildNode(winNode);
}

void AppWindowCreationHelper::AddEditBoxWindow(MkSceneNode* parentNode, const MkHashStr& name, const MkStr& message, float posX, float posY, float length)
{
	MkEditBoxControlNode* winNode = MkEditBoxControlNode::CreateChildNode(parentNode, name);
	winNode->SetSingleLineEditBox(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, length, message, false);
	winNode->SetLocalPosition(MkFloat2(posX, posY));
	winNode->SetLocalDepth(-1.f);
}

void AppWindowCreationHelper::AddStaticTextPanel(MkSceneNode* parentNode, const MkHashStr& name, const MkStr& message, float posX, float posY)
{
	MkPanel& panel = parentNode->CreatePanel(name);
	panel.SetTextMsg(message);
	panel.SetLocalPosition(MkFloat2(posX, posY));
	panel.SetLocalDepth(-1.f);
}

void AppWindowCreationHelper::AddCheckBoxWindow(MkSceneNode* parentNode, const MkHashStr& name, const MkStr& message, float posX, float posY, bool defCheck)
{
	MkCheckBoxControlNode* winNode = MkCheckBoxControlNode::CreateChildNode(parentNode, name);
	winNode->SetCheckBox(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, message, defCheck);
	winNode->SetLocalPosition(MkFloat2(posX, posY));
	winNode->SetLocalDepth(-1.f);
}

MkDropDownListControlNode* AppWindowCreationHelper::AddDropdownListWindow(MkSceneNode* parentNode, const MkHashStr& name, float posX, float posY, float length, int onePageItemSize)
{
	MkDropDownListControlNode* winNode = MkDropDownListControlNode::CreateChildNode(parentNode, name);
	winNode->SetDropDownList(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, length, onePageItemSize);
	winNode->SetLocalPosition(MkFloat2(posX, posY));
	winNode->SetLocalDepth(-1.f);
	return winNode;
}

void AppWindowCreationHelper::AddListBoxWindow(MkSceneNode* parentNode, const MkHashStr& name, float posX, float posY, float length, int onePageItemSize)
{
	MkListBoxControlNode* winNode = MkListBoxControlNode::CreateChildNode(parentNode, name);
	winNode->SetListBox(MkWindowThemeData::DefaultThemeName, onePageItemSize, length, MkWindowThemeData::eFT_Small);
	winNode->SetLocalPosition(MkFloat2(posX, posY));
	winNode->SetLocalDepth(-1.f);
}

void AppWindowCreationHelper::AddSlideBarWindow(MkSceneNode* parentNode, const MkHashStr& name, float posX, float posY, float length)
{
	MkSliderControlNode* winNode = MkSliderControlNode::CreateChildNode(parentNode, name);
	winNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, length, 0, 10, 0);
	winNode->SetLocalPosition(MkFloat2(posX, posY));
	winNode->SetLocalDepth(-1.f);
}

//------------------------------------------------------------------------------------------------//