
#include "MkCore_MkDataNode.h"
#include "MkS2D_MkWindowEventManager.h"

#include "GameSUI_Thumbnail.h"
#include "GameDataNode.h"
#include "GameSystemManager.h"
#include "GamePageWizardLab.h"

//------------------------------------------------------------------------------------------------//

class GP_WizardLabWindow : public GP_IslandBaseWindow
{
public:

	virtual void UseWindowEvent(WindowEvent& evt)
	{
		if (evt.type == MkSceneNodeFamilyDefinition::eCursorLeftRelease)
		{
			if (evt.node->GetPresetComponentType() == eS2D_WPC_NormalButton)
			{
				_ShowTargetWizard(evt.node->GetNodeName());

				GAME_SYSTEM.GetMasterPlayer().GetWizardGroup().SetTargetWizard(evt.node->GetNodeName().GetString().ToUnsignedInteger());
			}
		}
	}

	GP_WizardLabWindow(const MkHashStr& name) : GP_IslandBaseWindow(name, L"Scene\\wizard_lab_page.msd")
	{
		const MkMap<unsigned int, GameWizardUnitInfo>& members = GAME_SYSTEM.GetMasterPlayer().GetWizardGroup().GetMembers();
		unsigned int targetWizard = GAME_SYSTEM.GetMasterPlayer().GetWizardGroup().GetTargetWizard();
		if ((!members.Empty()) && members.Exist(targetWizard))
		{
			MkConstMapLooper<unsigned int, GameWizardUnitInfo> looper(members);
			MK_STL_LOOP(looper)
			{
				const GameWizardUnitInfo& wui = looper.GetCurrentField();
				unsigned int wizardID = wui.GetWizardID();
				MkHashStr wizardKey = MkStr(wizardID);
				float iPos = static_cast<float>(wizardID - 1);

				const MkDataNode* wizardData = GameDataNode::WizardSet->GetChildNode(wizardKey);
				if (wizardData != NULL)
				{
					MkStr imagePath;
					wizardData->GetData(L"ImagePath", imagePath, 0);

					MkStr wizardName;
					wizardData->GetData(L"Name", wizardName, 0);

					GameSUI_WizardThumbnail* btn = new GameSUI_WizardThumbnail(wizardKey);
					btn->CreateWizardThumbnail(imagePath, L"TN_WL", wui.GetWizardLevel(), wizardName);
					btn->SetLocalPosition(MkVec3(100.f + iPos * 30.f, 600.f - iPos * 130.f, -MKDEF_BASE_WINDOW_DEPTH_GRID));
					AttachChildNode(btn);

					if (wizardID == targetWizard)
					{
						_ShowTargetWizard(wizardKey);
					}
				}
			}
		}
	}

	virtual ~GP_WizardLabWindow() {}

protected:

	void _ShowTargetWizard(const MkHashStr& wizardKey)
	{
		const MkDataNode* wizardData = GameDataNode::WizardSet->GetChildNode(wizardKey);
		if (wizardData != NULL)
		{
			MkStr imagePath;
			wizardData->GetData(L"ImagePath", imagePath, 0);

			const MkHashStr rectName = L"__#BigView";
			MkSRect* rect = ExistSRect(rectName) ? GetSRect(rectName) : CreateSRect(rectName);
			rect->SetTexture(imagePath, L"Full");
			rect->AlignRect(GetPresetComponentSize(), eRAP_LeftCenter, MkFloat2(500.f, 0.f), 0.f);
			rect->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
		}
	}
};

//------------------------------------------------------------------------------------------------//

bool GamePageWizardLab::SetUp(MkDataNode& sharingNode)
{
	MK_WIN_EVENT_MGR.RegisterWindow(new GP_WizardLabWindow(GetPageName()), true);

	return true;
}

void GamePageWizardLab::Update(const MkTimeState& timeState)
{
	
}

void GamePageWizardLab::Clear(MkDataNode* sharingNode)
{
	GamePageIslandBase::Clear(sharingNode);
}

GamePageWizardLab::GamePageWizardLab(const MkHashStr& name) : GamePageIslandBase(name)
{
	
}

//------------------------------------------------------------------------------------------------//
