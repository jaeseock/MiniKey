
#include "MkS2D_MkWindowResourceManager.h"

#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkBaseWindowNode.h"
#include "MkS2D_MkSpreadButtonNode.h"
#include "MkS2D_MkCheckButtonNode.h"
#include "MkS2D_MkEditBoxNode.h"

#include "MkS2D_MkWindowOpHelper.h"

//------------------------------------------------------------------------------------------------//

MkBaseWindowNode* MkWindowOpHelper::CreateWindowPreset
(MkSceneNode* parentNode, const MkHashStr& nodeName, const MkHashStr& themeName, eS2D_WindowPresetComponent component, const MkFloat2& componentSize,
 const MkStr& caption, const MkFloat2& localPos, float localDepth, bool enable)
{
	const MkHashStr& tName = themeName.Empty() ? MK_WR_PRESET.GetDefaultThemeName() : themeName;

	MkBaseWindowNode* winNode = MkBaseWindowNode::__CreateWindowPreset(parentNode, nodeName, tName, component, componentSize);
	if (winNode != NULL)
	{
		winNode->SetLocalPosition(localPos);
		winNode->SetLocalDepth(localDepth);

		if (!caption.Empty())
		{
			winNode->SetPresetComponentCaption(tName, MkBaseWindowNode::CaptionDesc(caption));
		}

		winNode->SetEnable(enable);
	}
	return winNode;
}

MkSpreadButtonNode* MkWindowOpHelper::CreateDownwardSelectionSpreadButton
(MkSceneNode* parentNode, const MkHashStr& nodeName, const MkHashStr& themeName, const MkFloat2& size, const MkFloat2& localPos, float localDepth, bool enable)
{
	MkSpreadButtonNode* winNode = new MkSpreadButtonNode(nodeName);
	if (winNode != NULL)
	{
		const MkHashStr& tName = themeName.Empty() ? MK_WR_PRESET.GetDefaultThemeName() : themeName;
		winNode->CreateSelectionRootTypeButton(tName, size, MkSpreadButtonNode::eDownward);
		_SetWindowInfo(winNode, parentNode, localPos, localDepth, enable);
	}
	return winNode;
}

bool MkWindowOpHelper::AddItemListToSpreadButton(MkSpreadButtonNode* winNode, const MkArray<MkHashStr>& keyList, const MkArray<MkStr>& descList)
{
	if ((winNode != NULL) && (!keyList.Empty()))
	{
		MkArray<MkStr> tmpDesc;
		if (descList.Empty())
		{
			tmpDesc.Reserve(keyList.GetSize());
			MK_INDEXING_LOOP(keyList, i)
			{
				tmpDesc.PushBack(keyList[i].GetString());
			}
		}
		else if (keyList.GetSize() != descList.GetSize())
			return false;

		const MkArray<MkStr>& descs = descList.Empty() ? tmpDesc : descList;

		MkBaseWindowNode::ItemTagInfo ti;
		MK_INDEXING_LOOP(keyList, i)
		{
			ti.captionDesc.SetString(descs[i]);
			winNode->AddItem(keyList[i], ti);
		}

		winNode->SetTargetItem(keyList[0]);
		return true;
	}
	return false;
}

MkCheckButtonNode* MkWindowOpHelper::CreateCheckButton
(MkSceneNode* parentNode, const MkHashStr& nodeName, const MkHashStr& themeName, bool checked,
 const MkStr& caption, const MkFloat2& localPos, float localDepth, bool enable)
{
	MkCheckButtonNode* winNode = new MkCheckButtonNode(nodeName);
	if (winNode != NULL)
	{
		const MkHashStr& tName = themeName.Empty() ? MK_WR_PRESET.GetDefaultThemeName() : themeName;
		winNode->CreateCheckButton(tName, MkBaseWindowNode::CaptionDesc(caption), checked);
		_SetWindowInfo(winNode, parentNode, localPos, localDepth, enable);
	}
	return winNode;
}

MkEditBoxNode* MkWindowOpHelper::CreateEditBox
(MkSceneNode* parentNode, const MkHashStr& nodeName, const MkHashStr& themeName, const MkFloat2& size,
 const MkStr& initMsg, const MkFloat2& localPos, float localDepth, bool enable)
{
	MkEditBoxNode* winNode = new MkEditBoxNode(nodeName);
	if (winNode != NULL)
	{
		const MkHashStr& tName = themeName.Empty() ? MK_WR_PRESET.GetDefaultThemeName() : themeName;
		winNode->CreateEditBox(tName, size, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkHashStr::NullHash, MkStr::Null, false);
		_SetWindowInfo(winNode, parentNode, localPos, localDepth, enable);

		if (!initMsg.Empty())
		{
			winNode->SetText(initMsg, false);
		}
	}
	return winNode;
}

void MkWindowOpHelper::_SetWindowInfo(MkBaseWindowNode* winNode, MkSceneNode* parentNode, const MkFloat2& localPos, float localDepth, bool enable)
{
	if (winNode != NULL)
	{
		winNode->SetLocalPosition(localPos);
		winNode->SetLocalDepth(localDepth);
		winNode->SetEnable(enable);

		if (parentNode != NULL)
		{
			parentNode->AttachChildNode(winNode);
		}
	}
}

//------------------------------------------------------------------------------------------------//