
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkWindowResourceManager.h"

#include "GameDataNode.h"
#include "GameSUI_Thumbnail.h"


//------------------------------------------------------------------------------------------------//

bool GameSUI_WizardThumbnail::CreateWizardThumbnail(const MkPathName& iconPath, const MkHashStr& iconSubset, int initLevel, const MkStr& name)
{
	if (CreateWindowPreset(MK_WR_PRESET.GetDefaultThemeName(), eS2D_WPC_NormalButton, MkFloat2(200.f, 10.f + 16.f + 10.f + 12.f + 10.f)))
	{
		const float MARGIN = MK_WR_PRESET.GetMargin();
		const MkHashStr iconTagName = MKDEF_S2D_BASE_WND_ICON_TAG_NAME;

		if (SetPresetComponentIcon(iconTagName, eRAP_LeftBottom, MkFloat2(MARGIN, MARGIN), 0.f, iconPath, iconSubset))
		{
			const float capPos = GetSRect(iconTagName)->GetLocalSize().x + 10.f;

			MkSRect* lvTag = SetWizardLevel(initLevel);
			lvTag->AlignRect(GetPresetComponentSize(), eRAP_LeftTop, MkFloat2(capPos, 10.f), 0.f);
			lvTag->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);

			CaptionDesc caption;
			caption.SetString(name);
			return SetPresetComponentCaption(MK_WR_PRESET.GetDefaultThemeName(), caption, eRAP_LeftTop, MkFloat2(capPos, 10.f + 16.f + 10.f));
		}
	}
	return false;
}

MkSRect* GameSUI_WizardThumbnail::SetWizardLevel(int level)
{
	const MkHashStr rectName = L"__#WizardLV";
	MkSRect* tag = ExistSRect(rectName) ? GetSRect(rectName) : CreateSRect(rectName);
	MkStr buffer;
	MkDecoStr::Convert(L"°íµñ16", MK_FONT_MGR.OrangeFS(), 0, L"Lv. " + MkStr(level), buffer);
	tag->SetDecoString(buffer);
	return tag;
}

//------------------------------------------------------------------------------------------------//

bool GameSUI_SmallAgentThumbnail::CreateSmallAgentThumbnail(unsigned int agentID, int initLevel, bool toBattle)
{
	const MkHashStr agentKey = MkStr(agentID);
	const MkDataNode* agentData = GameDataNode::AgentSet->GetChildNode(agentKey);
	if (agentData != NULL)
	{
		MkStr imagePath;
		agentData->GetData(L"ImagePath", imagePath, 0);

		//MkStr subset;
		//agentData->GetData(L"Subset", subset, 0);
		MkHashStr subset = agentKey;

		MkStr agentName;
		agentData->GetData(L"Name", agentName, 0);

		if (CreateWindowPreset(MK_WR_PRESET.GetDefaultThemeName(), eS2D_WPC_NormalButton, MkFloat2(300.f, 34.f)))
		{
			const MkHashStr iconTagName = MKDEF_S2D_BASE_WND_ICON_TAG_NAME;

			if (SetPresetComponentIcon(iconTagName, eRAP_LeftBottom, MkFloat2(2.f, 2.f), 0.f, MkPathName(imagePath), subset))
			{
				MkSRect* iconTag = GetSRect(iconTagName);
				iconTag->SetLocalSize(MkFloat2(30.f, 30.f));
				
				SetAgentLevel(initLevel);
				SetToBattle(toBattle);

				CaptionDesc caption;
				caption.SetString(agentName);
				return SetPresetComponentCaption(MK_WR_PRESET.GetDefaultThemeName(), caption, eRAP_LeftTop, MkFloat2(80.f, 11.f));
			}
		}
	}

	return false;
}

void GameSUI_SmallAgentThumbnail::SetAgentLevel(int level)
{
	const MkHashStr rectName = L"__#AgentLV";
	MkSRect* tag = ExistSRect(rectName) ? GetSRect(rectName) : CreateSRect(rectName);
	MkStr buffer;
	MkDecoStr::Convert(L"°íµñ12", MK_FONT_MGR.OrangeFS(), 0, L"Lv. " + MkStr(level), buffer);
	tag->SetDecoString(buffer);
	tag->AlignRect(GetPresetComponentSize(), eRAP_LeftTop, MkFloat2(40.f, 11.f), 0.f);
	tag->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
}

void GameSUI_SmallAgentThumbnail::SetToBattle(bool enable)
{
	const MkHashStr rectName = L"__#State";
	MkSRect* tag = NULL;
	if (enable)
	{
		tag = ExistSRect(rectName) ? GetSRect(rectName) : CreateSRect(rectName);
		MkStr buffer;
		MkDecoStr::Convert(L"°íµñ12", MK_FONT_MGR.RedFS(), 0, L"[ÀüÅõ´ë±â]", buffer);
		tag->SetDecoString(buffer);
		tag->AlignRect(GetPresetComponentSize(), eRAP_LeftTop, MkFloat2(230.f, 11.f), 0.f);
		tag->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
	}
	else
	{
		if (ExistSRect(rectName))
		{
			DeleteSRect(rectName);
		}
	}
}

//------------------------------------------------------------------------------------------------//
