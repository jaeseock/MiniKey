
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkListBoxControlNode.h"

#include "MkPA_MkWindowManagerNode.h"
#include "MkPA_MkBitmapPool.h"

#include "AppDefinition.h"
#include "SettingRepository.h"
#include "SceneSelectionWindowNode.h"

#include "DepthFogEditWindowNode.h"

//------------------------------------------------------------------------------------------------//

void SceneSelectionWindowNode::SetUp(MkDataNode& setting, const MkArray<MkSceneNode*>& rootNode)
{
	m_RootNode = rootNode;

	// title bar
	SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
	SetIcon(MkWindowThemeData::eIT_Default);
	SetCaption(L"장면 선택", eRAP_LeftCenter);
	SetAlignmentPosition(eRAP_MiddleTop);
	SetAlignmentOffset(MkFloat2(0.f, -100.f));
	
	// body frame
	MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(this, L"BodyFrame");
	bodyFrame->SetBodyFrame
		(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(503.f, 440.f));

	// list
	MkListBoxControlNode* lbNode = MkListBoxControlNode::CreateChildNode(bodyFrame, L"ListBox");
	lbNode->SetListBox(MkWindowThemeData::DefaultThemeName, 22, 200.f, MkWindowThemeData::eFT_Small);
	lbNode->SetAlignmentPosition(eRAP_LeftTop);
	lbNode->SetAlignmentOffset(MkFloat2(10.f, -30.f));
	lbNode->SetLocalDepth(-1.f);

	// default scenes
	_UpdateScenes(L"Default\\Scenes\\");

	// setting node
	MkStr ssPath;
	setting.GetData(L"ScreenShotPath", ssPath, 0);
	MkPathName dirPath = ssPath;
	dirPath.CheckAndAddBackslash();
	_UpdateScenes(dirPath);
}

void SceneSelectionWindowNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (eventType == ePA_SNE_ItemLBtnPressed)
	{
		MkHashStr uniqueKey;
		if (argument->GetDataEx(MkListBoxControlNode::ArgKey_Item, uniqueKey, 0))
		{
			MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");
			if (bodyFrame != NULL)
			{
				_ItemInfo& fileInfo = m_ItemInfo[uniqueKey];
				fileInfo.loaded = true;

				// base thumbnail
				MkPanel& basePanel = bodyFrame->CreatePanel(L"Base");
				basePanel.SetLocalPosition(MkFloat2(240.f, 218.f));
				basePanel.SetLocalDepth(-1.f);
				basePanel.SetPanelSize(MkFloat2(256.f, 192.f));
				basePanel.SetBiggerSourceOp(MkPanel::eReduceSource);
				basePanel.SetTexture(fileInfo.baseKey);

				// depth thumbnail
				MkPanel& depthPanel = bodyFrame->CreatePanel(L"Depth");
				depthPanel.SetLocalPosition(MkFloat2(240.f, 15.f));
				depthPanel.SetLocalDepth(-1.f);
				depthPanel.SetPanelSize(MkFloat2(256.f, 192.f));
				depthPanel.SetBiggerSourceOp(MkPanel::eReduceSource);
				depthPanel.SetTexture(fileInfo.depthKey);
			}
			return;
		}
	}
	else if (eventType == ePA_SNE_ItemLBtnDBClicked)
	{
		MkHashStr uniqueKey;
		if (argument->GetDataEx(MkListBoxControlNode::ArgKey_Item, uniqueKey, 0))
		{
			if (SREPO.GetSceneKey() != uniqueKey)
			{
				SREPO.SetSceneKey(uniqueKey);

				SelectTargetScene();
			}

			// activate main control window
			MkWindowManagerNode* mgrNode = GetWindowManagerNode();
			if (!mgrNode->IsActivating(L"MainControl"))
			{
				mgrNode->ActivateWindow(L"MainControl");
			}

			StartNodeReportTypeEvent(ePA_SNE_CloseWindow, NULL); // 종료
			return;
		}
	}
	
	MkTitleBarControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void SceneSelectionWindowNode::Deactivate(void)
{
	MkHashMapLooper<MkHashStr, _ItemInfo> looper(m_ItemInfo);
	MK_STL_LOOP(looper)
	{
		if (looper.GetCurrentField().loaded)
		{
			if (MK_BITMAP_POOL.UnloadBitmapTexture(looper.GetCurrentField().baseKey) &&
				MK_BITMAP_POOL.UnloadBitmapTexture(looper.GetCurrentField().depthKey))
			{
				looper.GetCurrentField().loaded = false;
			}
		}
	}

	MkTitleBarControlNode::Deactivate();
}

void SceneSelectionWindowNode::SelectTargetScene(void)
{
	MK_DEV_PANEL.MsgToFreeboard(6, L"Scene : " + SREPO.GetSceneKey().GetString());

	_ItemInfo& fileInfo = m_ItemInfo[SREPO.GetSceneKey()];

	// intro image 삭제
	MkPanel* introPanel = m_RootNode[eDST_MainScene]->GetPanel(L"Intro");
	if (introPanel != NULL)
	{
		MkHashStr filePath = introPanel->GetTexturePtr()->GetPoolKey();
		m_RootNode[eDST_MainScene]->DeletePanel(L"Intro");
		MK_BITMAP_POOL.UnloadBitmapTexture(filePath);
	}

	// front image(origin, depth) 삭제
	MkSceneNode* sceneRoot = m_RootNode[eDST_Final]->GetChildNode(L"<Scene>");
	sceneRoot->DeletePanel(L"Origin");
	sceneRoot->DeletePanel(L"Depth");

	// eDST_MainScene 
	MkPanel& basePanel = m_RootNode[eDST_MainScene]->CreatePanel(L"Main");
	basePanel.SetTexture(fileInfo.baseKey);
	basePanel.SetShaderEffect(L"ColorEdit");
	basePanel.SetUserDefinedProperty(L"g_ColorFactor", SREPO.GetColorEditBrightness(), SREPO.GetColorEditSaturation(), SREPO.GetColorEditContrast());
	basePanel.SetUserDefinedProperty(L"g_ColorShift", SREPO.GetColorEditColorBalanceR(), SREPO.GetColorEditColorBalanceG(), SREPO.GetColorEditColorBalanceB());
	basePanel.SetShaderEffectEnable(SREPO.GetColorEditEnable());

	// eDST_DepthFog
	{
		MkPanel* mainPanel = m_RootNode[eDST_DepthFog]->GetPanel(L"Main");
		mainPanel->SetShaderEffect(L"DepthFog");
		mainPanel->SetUserDefinedProperty(L"g_FogMixer", SREPO.GetDepthFogMixer());
		mainPanel->SetEffectTexture1(fileInfo.depthKey);

		MkWindowManagerNode* mgrNode = GetWindowManagerNode();
		DepthFogEditWindowNode* editWN = dynamic_cast<DepthFogEditWindowNode*>(mgrNode->GetChildNode(L"Edit_DepthFog"));
		editWN->UpdateMultGradation();
		editWN->UpdateAddGradation();

		mainPanel->SetShaderEffectEnable(SREPO.GetDepthFogEnable());
	}

	// eDST_EdgeDetection
	{
		MkPanel* mainPanel = m_RootNode[eDST_EdgeDetection]->GetPanel(L"Main");
		mainPanel->SetShaderEffect(L"EdgeDetection");
		mainPanel->SetUserDefinedProperty(L"g_EdgeFactor", SREPO.GetEdgeDetectionAmplifier(), SREPO.GetEdgeDetectionDebugBlend());
		mainPanel->SetUserDefinedProperty(L"g_EdgeColor", SREPO.GetEdgeDetectionEdgeColor().x, SREPO.GetEdgeDetectionEdgeColor().y, SREPO.GetEdgeDetectionEdgeColor().z);
		mainPanel->SetEffectTexture1(fileInfo.depthKey);
		mainPanel->SetShaderEffectEnable(SREPO.GetEdgeDetectionEnable());
	}

	
	// eDST_HDR_Blend
	{
		MkPanel* mainPanel = m_RootNode[eDST_HDR_Blend]->GetPanel(L"Main");
		mainPanel->SetShaderEffect(L"HDR_Blend");
		mainPanel->SetUserDefinedProperty(L"g_HDRPower", SREPO.GetFakeHDRPowerHigh(), SREPO.GetFakeHDRPowerLow());
		mainPanel->SetShaderEffectEnable(SREPO.GetFakeHDREnable());
	}

	// eDST_DepthOfField
	{
		MkPanel* mainPanel = m_RootNode[eDST_DepthOfField]->GetPanel(L"Main");
		mainPanel->SetShaderEffect(L"DepthOfField");
		mainPanel->SetUserDefinedProperty(L"g_DOF_Factor", SREPO.GetDepthOfFieldPosition(), SREPO.GetDepthOfFieldRange(), SREPO.GetDepthOfFieldDebugBlend());
		mainPanel->SetEffectTexture1(fileInfo.depthKey);
		mainPanel->SetShaderEffectEnable(SREPO.GetDepthOfFieldEnable());
	}

	// eDST_RadialBlur
	{
		MkPanel* mainPanel = m_RootNode[eDST_RadialBlur]->GetPanel(L"Main");
		mainPanel->SetShaderEffect(L"RadialBlur");
		mainPanel->SetUserDefinedProperty(L"g_RadialBlurFactor", SREPO.GetRadialBlurPosition().x, SREPO.GetRadialBlurPosition().y, SREPO.GetRadialBlurStartRange(), SREPO.GetRadialBlurPower());
		mainPanel->SetShaderEffectEnable(SREPO.GetRadialBlurEnable());
	}
}

//------------------------------------------------------------------------------------------------//

void SceneSelectionWindowNode::_UpdateScenes(const MkPathName& dirPath)
{
	MkArray<MkPathName> filePathList;
	MkArray<MkPathName> extensionFilter;
	extensionFilter.PushBack(L"png");

	// 하위 디렉토리 포함 안함
	dirPath.GetWhiteFileList(filePathList, NULL, &extensionFilter, NULL, NULL, false, false);

	// 파일 이름만 추출
	MkArray<MkStr> fileNameList(filePathList.GetSize());
	MK_INDEXING_LOOP(filePathList, i)
	{
		fileNameList.PushBack(filePathList[i].GetFileName(false));
	}
	filePathList.Clear();

	// base-depth 페어를 가지고 있는 key 추출
	MkArray<bool> skipTag;
	skipTag.Fill(fileNameList.GetSize(), false);

	MkArray<MkHashStr> newKeyList(fileNameList.GetSize() / 2);

	MK_INDEXING_LOOP(fileNameList, i)
	{
		if (skipTag[i])
			continue;

		if (_CheckPairScene(fileNameList, i, skipTag, L"_b", L"_d", newKeyList))
			_CheckPairScene(fileNameList, i, skipTag, L"_d", L"_b", newKeyList);
	}
	fileNameList.Clear();
	skipTag.Clear();

	if (newKeyList.Empty())
		return;

	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");
	if (bodyFrame == NULL)
		return;

	MkListBoxControlNode* lbNode = dynamic_cast<MkListBoxControlNode*>(bodyFrame->GetChildNode(L"ListBox"));
	if (lbNode == NULL)
		return;

	MK_INDEXING_LOOP(newKeyList, i)
	{
		const MkHashStr& key = newKeyList[i];

		lbNode->AddItem(key, key.GetString());

		_ItemInfo& itemInfo = m_ItemInfo.Create(key);
		itemInfo.loaded = false;
		itemInfo.baseKey = dirPath + key.GetString() + L"_b.png";
		itemInfo.depthKey = dirPath + key.GetString() + L"_d.png";
	}
}

bool SceneSelectionWindowNode::_CheckPairScene
(const MkArray<MkStr>& fileNameList, unsigned int index, MkArray<bool>& skipTag, const MkStr& positfixA, const MkStr& positfixB, MkArray<MkHashStr>& newKeyList) const
{
	const MkStr& currFileName = fileNameList[index];
	if (!currFileName.CheckPostfix(positfixA))
		return true;

	MkStr keyBuf;
	currFileName.GetSubStr(MkArraySection(0, currFileName.GetSize() - 2), keyBuf);
	if (!keyBuf.Empty())
	{
		unsigned int pairPos = fileNameList.FindFirstInclusion(MkArraySection(index + 1), keyBuf + positfixB);
		if (pairPos != MKDEF_ARRAY_ERROR)
		{
			skipTag[pairPos] = true;

			MkHashStr currKey = keyBuf;
			if (!m_ItemInfo.Exist(currKey))
			{
				newKeyList.PushBack(currKey);
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------//
