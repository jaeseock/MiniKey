
#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkFontManager.h"
#include "MkS2D_MkWindowResourceManager.h"

#include "GameSUI_Thumbnail.h"


//------------------------------------------------------------------------------------------------//

bool GameSUI_ThumbnailWizard::CreateThumbnailWizard(const MkPathName& iconPath, const MkHashStr& iconSubset, int initLevel, const MkStr& name)
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

MkSRect* GameSUI_ThumbnailWizard::SetWizardLevel(int level)
{
	const MkHashStr rectName = L"__#WizardLV";
	MkSRect* tag = ExistSRect(rectName) ? GetSRect(rectName) : CreateSRect(rectName);
	MkStr buffer;
	MkDecoStr::Convert(L"°íµñ16", MK_FONT_MGR.OrangeFS(), 0, L"Lv. " + MkStr(level), buffer);
	tag->SetDecoString(buffer);
	return tag;
}

//------------------------------------------------------------------------------------------------//
