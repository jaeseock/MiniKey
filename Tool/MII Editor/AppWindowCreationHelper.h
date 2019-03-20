#pragma once

//------------------------------------------------------------------------------------------------//
// window - app base
//------------------------------------------------------------------------------------------------//


class MkStr;
class MkHashStr;
class MkSceneNode;
class MkWindowThemedNode;
class MkDropDownListControlNode;

class AppWindowCreationHelper
{
public:

	static MkWindowThemedNode* AddStaticGuide(MkSceneNode* parentNode, const MkHashStr& name, float posX, float posY, float width, float height);
	static void AddNormalButton(MkSceneNode* parentNode, const MkHashStr& name, const MkStr& message, float posX, float posY, float minWidth);
	static void AddOkButton(MkSceneNode* parentNode, const MkStr& message, float posX, float posY, float minWidth, float minHeight);
	static void AddEditBoxWindow(MkSceneNode* parentNode, const MkHashStr& name, const MkStr& message, float posX, float posY, float length);
	static void AddStaticTextPanel(MkSceneNode* parentNode, const MkHashStr& name, const MkStr& message, float posX, float posY);
	static void AddCheckBoxWindow(MkSceneNode* parentNode, const MkHashStr& name, const MkStr& message, float posX, float posY, bool defCheck);
	static MkDropDownListControlNode* AddDropdownListWindow(MkSceneNode* parentNode, const MkHashStr& name, float posX, float posY, float length, int onePageItemSize);
	static void AddListBoxWindow(MkSceneNode* parentNode, const MkHashStr& name, float posX, float posY, float length, int onePageItemSize);
	static void AddSlideBarWindow(MkSceneNode* parentNode, const MkHashStr& name, float posX, float posY, float length);
};
