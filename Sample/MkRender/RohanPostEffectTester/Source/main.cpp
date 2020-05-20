
//------------------------------------------------------------------------------------------------//
// single/multi-thread rendering application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkCheck.h"
#include "MkCore_MkPageManager.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkWin32Application.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkKeywordFilter.h"
#include "MkCore_MkFloatOp.h"

#include "MkCore_MkDataNode.h"

#include "MkPA_MkRenderFramework.h"

#include "MkCore_MkUniformDice.h"

//#include "MkS2D_MkTexturePool.h"
#include "MkPA_MkFontManager.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkRenderer.h"
#include "MkPA_MkBitmapPool.h"

#include "MkPA_MkTextNode.h"

#include "MkPA_MkSceneNode.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkWindowThemedNode.h"
#include "MkPA_MkWindowBaseNode.h"
#include "MkPA_MkWindowManagerNode.h"
#include "MkPA_MkTitleBarControlNode.h"
#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkCheckBoxControlNode.h"
#include "MkPA_MkScrollBarControlNode.h"
#include "MkPA_MkScenePortalNode.h"
#include "MkPA_MkEditBoxControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkListBoxControlNode.h"
#include "MkPA_MkDropDownListControlNode.h"
#include "MkPA_MkProgressBarNode.h"

#include "MkPA_MkDrawSceneNodeStep.h"
#include "MkPA_MkStaticResourceContainer.h"

#include "MkCore_MkDevPanel.h"

#include "MkPA_MkWindowFactory.h"
#include "MkCore_MkTimeCounter.h"

#include "MkPA_MkShaderEffect.h"

#include "SceneSelectionWindowNode.h"
#include "MainControlWindowNode.h"
#include "ColorEditWindowNode.h"
#include "DepthFogEditWindowNode.h"
#include "EdgeDetectionEditWindowNode.h"
#include "HDREditWindowNode.h"
#include "DepthOfFieldEditWindowNode.h"
#include "RadialBlurEditWindowNode.h"

#include "AppDefinition.h"
#include "SettingRepository.h"

//------------------------------------------------------------------------------------------------//

// TestPage 선언
class TestPage : public MkBasePage
{
public:

	virtual bool SetUp(MkDataNode& sharingNode)
	{
		// setting
		{
			MkPathName settingFilePath;
			settingFilePath.ConvertToRootBasisAbsolutePath(L"AppSetting.txt");
			if (settingFilePath.CheckAvailable())
			{
				m_AppSettingNode.Load(settingFilePath);
			}

			// screen shot path
			_UpdateDirPath(L"ScreenShotPath", L"Default\\Scenes", L"게임 스크린샷 디렉토리를 지정해주세요.");
		}

		SceneSelectionWindowNode* sceneSelectionWN = NULL;
		MainControlWindowNode* mainControlWN = NULL;

		// scene root
		m_RootNode.Reserve(eDST_Max);
		for (int i=0; i<eDST_Max; ++i)
		{
			m_RootNode.PushBack(new MkSceneNode(L"<Root>"));

			if (i < eDST_Final)
			{
				m_RootNode[i]->SetLocalDepth(9999.f);

				if (i == eDST_MainScene)
				{
					m_RootNode[i]->CreatePanel(L"Intro").SetTexture(L"Default\\empty_bg.png");
				}
			}
			else // i == eDST_Final
			{
				m_RootNode[i]->CreateChildNode(L"<Scene>")->SetLocalDepth(9999.f);

				// window mgr
				m_WindowMgr = MkWindowManagerNode::CreateChildNode(m_RootNode[i], L"<WinMgr>");
				m_WindowMgr->SetDepthBandwidth(1000.f);

				// 장면 선택 창
				sceneSelectionWN = new SceneSelectionWindowNode(L"SceneSelection");
				sceneSelectionWN->SetUp(m_AppSettingNode, m_RootNode);
				m_WindowMgr->AttachWindow(sceneSelectionWN);

				// 메인 조작 창
				mainControlWN = new MainControlWindowNode(L"MainControl");
				mainControlWN->SetUp(m_RootNode);
				m_WindowMgr->AttachWindow(mainControlWN);

				// edit - color
				{
					ColorEditWindowNode* editWN = new ColorEditWindowNode(L"Edit_Color");
					editWN->SetUp(m_RootNode[eDST_MainScene]);
					m_WindowMgr->AttachWindow(editWN);
				}

				// edit - depth fog
				{
					DepthFogEditWindowNode* editWN = new DepthFogEditWindowNode(L"Edit_DepthFog");
					editWN->SetUp(m_RootNode[eDST_DepthFog]);
					m_WindowMgr->AttachWindow(editWN);
				}

				// edit - edge detection
				{
					EdgeDetectionEditWindowNode* editWN = new EdgeDetectionEditWindowNode(L"Edit_EdgeDetection");
					editWN->SetUp(m_RootNode[eDST_EdgeDetection]);
					m_WindowMgr->AttachWindow(editWN);
				}

				// edit - HDR
				{
					HDREditWindowNode* editWN = new HDREditWindowNode(L"Edit_HDR");
					editWN->SetUp(m_RootNode[eDST_HDR_Blend]);
					m_WindowMgr->AttachWindow(editWN);
				}

				// edit - depth of field
				{
					DepthOfFieldEditWindowNode* editWN = new DepthOfFieldEditWindowNode(L"Edit_DepthOfField");
					editWN->SetUp(m_RootNode[eDST_DepthOfField]);
					m_WindowMgr->AttachWindow(editWN);
				}

				// edit - radial blur
				{
					RadialBlurEditWindowNode* editWN = new RadialBlurEditWindowNode(L"Edit_RadialBlur");
					editWN->SetUp(m_RootNode[eDST_RadialBlur]);
					m_WindowMgr->AttachWindow(editWN);
				}
			}
		}

		//------------------------------------------------------------------------------------------------//
		// draw step
		//------------------------------------------------------------------------------------------------//

		// main scene
		MkDrawSceneNodeStep* mainStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep
			(L"MainScene", eDST_MainScene, MkRenderTarget::eTexture, 1, MkInt2(1024, 768), MkFloat2::Zero, MkRenderToTexture::eRGBA);
		mainStep->SetSceneNode(m_RootNode[eDST_MainScene]);

		// depth fog
		m_RootNode[eDST_DepthFog]->CreatePanel(L"Main").SetTexture(mainStep->GetTargetTexture());

		MkDrawSceneNodeStep* fogStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep
			(L"Fog", eDST_DepthFog, MkRenderTarget::eTexture, 1, MkInt2(1024, 768), MkFloat2::Zero, MkRenderToTexture::eRGBA);
		fogStep->SetSceneNode(m_RootNode[eDST_DepthFog]);

		// edge detection
		m_RootNode[eDST_EdgeDetection]->CreatePanel(L"Main").SetTexture(fogStep->GetTargetTexture());

		MkDrawSceneNodeStep* edgeStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep
			(L"Edge", eDST_EdgeDetection, MkRenderTarget::eTexture, 1, MkInt2(1024, 768), MkFloat2::Zero, MkRenderToTexture::eRGBA);
		edgeStep->SetSceneNode(m_RootNode[eDST_EdgeDetection]);

		// HDR
		{
			MkPanel& mainPanel = m_RootNode[eDST_HDR_Reduction]->CreatePanel(L"Main");
			mainPanel.SetTexture(edgeStep->GetTargetTexture());
			mainPanel.SetPanelSize(MkFloat2(128, 96));
			mainPanel.SetBiggerSourceOp(MkPanel::eReduceSource);
			mainPanel.SetShaderEffect(L"HDR_Reduction");
		}

		MkDrawSceneNodeStep* hdrReductionStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep
			(L"HDR_Reduction", eDST_HDR_Reduction, MkRenderTarget::eTexture, 1, MkInt2(128, 96), MkFloat2::Zero, MkRenderToTexture::eRGBA);
		hdrReductionStep->SetSceneNode(m_RootNode[eDST_HDR_Reduction]);

		{
			MkPanel& mainPanel = m_RootNode[eDST_HDR_Blend]->CreatePanel(L"Main");
			mainPanel.SetTexture(edgeStep->GetTargetTexture());
			mainPanel.SetEffectTexture1(hdrReductionStep->GetTargetTexture());
		}

		MkDrawSceneNodeStep* hdrBlendStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep
			(L"HDR_Blend", eDST_HDR_Blend, MkRenderTarget::eTexture, 1, MkInt2(1024, 768), MkFloat2::Zero, MkRenderToTexture::eRGBA);
		hdrBlendStep->SetSceneNode(m_RootNode[eDST_HDR_Blend]);

		// depth of field
		m_RootNode[eDST_DepthOfField]->CreatePanel(L"Main").SetTexture(hdrBlendStep->GetTargetTexture());

		MkDrawSceneNodeStep* dofStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep
			(L"DOF", eDST_DepthOfField, MkRenderTarget::eTexture, 1, MkInt2(1024, 768), MkFloat2::Zero, MkRenderToTexture::eRGBA);
		dofStep->SetSceneNode(m_RootNode[eDST_DepthOfField]);

		// radial blur
		m_RootNode[eDST_RadialBlur]->CreatePanel(L"Main").SetTexture(dofStep->GetTargetTexture());

		MkDrawSceneNodeStep* rbStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep
			(L"RadialBlur", eDST_RadialBlur, MkRenderTarget::eTexture, 1, MkInt2(1024, 768), MkFloat2::Zero, MkRenderToTexture::eRGBA);
		rbStep->SetSceneNode(m_RootNode[eDST_RadialBlur]);

		// final
		m_RootNode[eDST_Final]->GetChildNode(L"<Scene>")->CreatePanel(L"Main").SetTexture(rbStep->GetTargetTexture());

		MkDrawSceneNodeStep* finalStep = MK_RENDERER.GetDrawQueue().CreateDrawSceneNodeStep(L"Final");
		finalStep->SetSceneNode(m_RootNode[eDST_Final]);

		//------------------------------------------------------------------------------------------------//

		SREPO.SetUp(sceneSelectionWN);

		MkPathName autoSaveFilePath;
		autoSaveFilePath.ConvertToRootBasisAbsolutePath(L"AutoSave.txt");
		if (autoSaveFilePath.CheckAvailable() &&
			SREPO.LoadEffectSetting(autoSaveFilePath))
		{
			mainControlWN->UpdateEffectSetting();
			m_WindowMgr->ActivateWindow(L"MainControl");
		}
		else
		{
			m_WindowMgr->ActivateWindow(L"SceneSelection", true);
		}

		MK_INDEXING_LOOP(m_RootNode, i)
		{
			m_RootNode[i]->Update();
		}

		MK_DEV_PANEL.MsgToFreeboard(0, L"F2 : 오버레이(원본 -> 깊이버퍼 -> 끔)");
		MK_DEV_PANEL.MsgToFreeboard(1, L"F3 : 모든 윈도우창 닫음");
		
		MK_DEV_PANEL.MsgToFreeboard(3, L"F5 : 장면 선택 윈도우 토글");
		MK_DEV_PANEL.MsgToFreeboard(4, L"F6 : 메인 설정 윈도우 토글");

		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		// show origin -> depth
		if (MK_INPUT_MGR.GetKeyReleased(VK_F2) && (!m_WindowMgr->IsModalWindowActivating()))
		{
			MkSceneNode* sceneRoot = m_RootNode[eDST_Final]->GetChildNode(L"<Scene>");
			MkPanel* originPanel = sceneRoot->GetPanel(L"Origin");
			MkPanel* depthPanel = sceneRoot->GetPanel(L"Depth");

			// none -> origin
			if ((originPanel == NULL) && (depthPanel == NULL))
			{
				const MkPanel* mainPanel = m_RootNode[eDST_MainScene]->GetPanel(L"Main");
				if (mainPanel != NULL)
				{
					MkPanel& panel = sceneRoot->CreatePanel(L"Origin");
					panel.SetTexture(mainPanel->GetTexturePtr()->GetPoolKey());
					panel.SetLocalDepth(-1.f);

					MK_DEV_PANEL.MsgToFreeboard(7, L"[원본 이미지]");
				}
			}
			// origin -> depth
			else if ((originPanel != NULL) && (depthPanel == NULL))
			{
				const MkPanel* mainPanel = m_RootNode[eDST_DepthFog]->GetPanel(L"Main");
				if (mainPanel != NULL)
				{
					sceneRoot->DeletePanel(L"Origin");

					MkPanel& panel = sceneRoot->CreatePanel(L"Depth");
					panel.SetTexture(mainPanel->GetEffectTexturePtr1()->GetPoolKey());
					panel.SetShaderEffect(L"_DepthOnly");
					panel.SetLocalDepth(-1.f);

					MK_DEV_PANEL.MsgToFreeboard(7, L"[깊이 버퍼]");
				}
			}
			// depth -> none
			else if ((originPanel == NULL) && (depthPanel != NULL))
			{
				sceneRoot->DeletePanel(L"Depth");

				MK_DEV_PANEL.MsgToFreeboard(7, MkStr::EMPTY);
			}
		}

		// 모든 창 닫기
		if (MK_INPUT_MGR.GetKeyReleased(VK_F3) && (!m_WindowMgr->IsModalWindowActivating()))
		{
			m_WindowMgr->DeactivateWindow(L"SceneSelection");
			m_WindowMgr->DeactivateWindow(L"MainControl");
			m_WindowMgr->DeactivateWindow(L"Edit_Color");
			m_WindowMgr->DeactivateWindow(L"Edit_DepthFog");
			m_WindowMgr->DeactivateWindow(L"Edit_EdgeDetection");
			m_WindowMgr->DeactivateWindow(L"Edit_HDR");
			m_WindowMgr->DeactivateWindow(L"Edit_DepthOfField");
			m_WindowMgr->DeactivateWindow(L"Edit_RadialBlur");
		}

		// SceneSelection 창 토글
		if (MK_INPUT_MGR.GetKeyReleased(VK_F5))
		{
			if (m_WindowMgr->IsActivating(L"SceneSelection"))
			{
				m_WindowMgr->DeactivateWindow(L"SceneSelection");
			}
			else
			{
				m_WindowMgr->ActivateWindow(L"SceneSelection", true);
			}
		}

		// MainControl 창 토글
		if (MK_INPUT_MGR.GetKeyReleased(VK_F6) && (!m_WindowMgr->IsModalWindowActivating()))
		{
			if (m_WindowMgr->IsActivating(L"MainControl"))
			{
				m_WindowMgr->DeactivateWindow(L"MainControl");
			}
			else if (m_RootNode[eDST_MainScene]->GetPanel(L"Main") != NULL)
			{
				m_WindowMgr->ActivateWindow(L"MainControl");
			}
		}

		if (MK_INPUT_MGR.GetMouseRightButtonPushing() &&
			MK_INPUT_MGR.GetMousePointerAvailable() &&
			m_WindowMgr->IsActivating(L"Edit_RadialBlur"))
		{
			MkVec2 pos = MK_INPUT_MGR.GetRelativeMousePosition();
			SREPO.SetRadialBlurPosition(MkFloat2(pos.x, pos.y));

			MkPanel* mainPanel = m_RootNode[eDST_RadialBlur]->GetPanel(L"Main");
			if (mainPanel != NULL)
			{
				mainPanel->SetUserDefinedProperty(L"g_RadialBlurFactor", SREPO.GetRadialBlurPosition().x, SREPO.GetRadialBlurPosition().y, SREPO.GetRadialBlurStartRange(), SREPO.GetRadialBlurPower());
			}
		}
		
		MK_INDEXING_LOOP(m_RootNode, i)
		{
			m_RootNode[i]->Update(timeState.fullTime);
		}
	}

	virtual void Clear(MkDataNode* sharingNode = NULL)
	{
		if (m_AppSettingNode.GetUnitCount() != 0)
		{
			m_AppSettingNode.SaveToText(L"AppSetting.txt");
			m_AppSettingNode.Clear();

			SREPO.SaveEffectSetting(L"AutoSave.txt");
		}

		MK_INDEXING_LOOP(m_RootNode, i)
		{
			delete m_RootNode[i];
		}
		m_RootNode.Clear();
		
		MK_RENDERER.GetDrawQueue().Clear();
	}

	TestPage(const MkHashStr& name) : MkBasePage(name)
	{
		
	}

	virtual ~TestPage() { Clear(); }

protected:

	void _UpdateDirPath(const MkHashStr& key, const MkPathName& defPath, const MkStr& msg)
	{
		MkPathName fullPath;

		MkStr buffer;
		if (m_AppSettingNode.GetData(key, buffer, 0) && (!buffer.Empty()))
		{
			MkPathName tmpPath = buffer;
			tmpPath.CheckAndAddBackslash();
			if (tmpPath.CheckAvailable())
			{
				fullPath = tmpPath;
			}
		}

		if (fullPath.Empty())
		{
			MkPathName tmpPath, defFullPath;
			defFullPath.ConvertToRootBasisAbsolutePath(defPath);
			fullPath = tmpPath.GetDirectoryPathFromDialog(msg, NULL, defFullPath) ? tmpPath : defFullPath;
		}

		fullPath.BackSpace(1); // 마지막 '\' 제거
		if (m_AppSettingNode.IsValidKey(key))
		{
			m_AppSettingNode.SetData(key, fullPath, 0);
		}
		else
		{
			m_AppSettingNode.CreateUnit(key, fullPath);
		}
	}

protected:

	MkDataNode m_AppSettingNode;

	MkArray<MkSceneNode*> m_RootNode;
	MkWindowManagerNode* m_WindowMgr;
};

class TestFramework : public MkRenderFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		MK_PAGE_MGR.SetUp(new MkBasePage(L"Root"));
		MK_PAGE_MGR.RegisterChildPage(L"Root", new TestPage(L"TestPage"));
		MK_PAGE_MGR.ChangePageDirectly(L"TestPage");
		
		return MkRenderFramework::SetUp(clientWidth, clientHeight, fullScreen, cmdLine);
	}

	virtual ~TestFramework() {}
};

// TestApplication 선언
class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

//------------------------------------------------------------------------------------------------//

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	//application.Run(hI, L"떼스또", L"..\\FileRoot", true, eSWP_FixedSize, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768);
	application.Run(hI, L"로한 후처리 테스터", L"", true, eSWP_FixedSize, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768);

	return 0;
}

//------------------------------------------------------------------------------------------------//

