#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"
#include "MkCore_MkArray.h"
#include "MkCore_MkHashStr.h"
#include "MkS2D_MkGlobalDefinition.h"
#include "MkS2D_MkProjectDefinition.h"


class MkSceneNode;
class MkBaseWindowNode;
class MkSpreadButtonNode;
class MkCheckButtonNode;
class MkEditBoxNode;

class MkWindowOpHelper
{
public:

	static MkBaseWindowNode* CreateWindowPreset
		(MkSceneNode* parentNode, const MkHashStr& nodeName, const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& componentSize,
		const MkStr& caption, const MkFloat2& localPos, float localDepth = -MKDEF_BASE_WINDOW_DEPTH_GRID, bool enable = true);

	static MkSpreadButtonNode* CreateDownwardSelectionSpreadButton
		(MkSceneNode* parentNode, const MkHashStr& nodeName, const MkHashStr& themeName, const MkFloat2& size,
		const MkFloat2& localPos, float localDepth = -MKDEF_BASE_WINDOW_DEPTH_GRID, bool enable = true);

	static bool AddItemListToSpreadButton(MkSpreadButtonNode* winNode, const MkArray<MkHashStr>& keyList, const MkArray<MkStr>& descList);

	static MkCheckButtonNode* CreateCheckButton
		(MkSceneNode* parentNode, const MkHashStr& nodeName, const MkHashStr& themeName, bool checked,
		const MkStr& caption, const MkFloat2& localPos, float localDepth = -MKDEF_BASE_WINDOW_DEPTH_GRID, bool enable = true);

	static MkEditBoxNode* CreateEditBox
		(MkSceneNode* parentNode, const MkHashStr& nodeName, const MkHashStr& themeName, const MkFloat2& size,
		const MkStr& initMsg, const MkFloat2& localPos, float localDepth = -MKDEF_BASE_WINDOW_DEPTH_GRID, bool enable = true);

protected:

	static void _SetWindowInfo(MkBaseWindowNode* winNode, MkSceneNode* parentNode, const MkFloat2& localPos, float localDepth, bool enable);
};
