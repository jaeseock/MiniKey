
#include "MkCore_MkDataNode.h"

#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkListBoxControlNode.h"
#include "MkPA_MkDropDownListControlNode.h"
#include "MkPA_MkSliderControlNode.h"
#include "MkPA_MkWindowFactory.h"

#include "SettingRepository.h"
#include "DepthFogEditWindowNode.h"


//------------------------------------------------------------------------------------------------//

void DepthFogEditWindowNode::SetUp(MkSceneNode* rootNode)
{
	m_RootNode = rootNode;

	// title bar
	SetTitleBar(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 300.f, true);
	SetIcon(MkWindowThemeData::eIT_Default);
	SetCaption(L"편집 : 깊이 안개", eRAP_LeftCenter);
	SetAlignmentPosition(eRAP_RightTop);
	SetAlignmentOffset(MkFloat2(-10.f, -10.f));
	
	// body frame
	const float HEIGHT = 330.f;

	MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(this, L"BodyFrame");
	bodyFrame->SetBodyFrame
		(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eCT_DefaultBox, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, MkFloat2(224.f, HEIGHT));

	MkWindowFactory wndFactory;
	wndFactory.SetMinClientSizeForButton(MkFloat2(205.f, 12.f));

	// refresh btn
	{
		MkWindowBaseNode* btnInst = wndFactory.CreateButtonTypeNode(L"RefreshDir", MkWindowThemeData::eCT_OKBtn, L"맵 리스트 갱신");
		btnInst->SetLocalPosition(MkFloat2(10.f, HEIGHT - 50.f));
		bodyFrame->AttachChildNode(btnInst);
	}

	// mult map
	MkDropDownListControlNode* multDDList;
	{
		multDDList = MkDropDownListControlNode::CreateChildNode(bodyFrame, L"DD_MultMap");
		multDDList->SetDropDownList(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 205.f, 12);
		multDDList->SetLocalDepth(-2.f);
		multDDList->SetLocalPosition(MkFloat2(10.f, HEIGHT - 90.f));

		MkWindowBaseNode* btnInst = wndFactory.CreateButtonTypeNode(L"Reload_MultMap", MkWindowThemeData::eCT_NormalBtn, L"Multiply 파일 다시 열기");
		btnInst->SetLocalPosition(MkFloat2(10.f, HEIGHT - 120.f));
		bodyFrame->AttachChildNode(btnInst);
	}

	// add map
	MkDropDownListControlNode* addDDList;
	{
		addDDList = MkDropDownListControlNode::CreateChildNode(bodyFrame, L"DD_AddMap");
		addDDList->SetDropDownList(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 205.f, 12);
		addDDList->SetLocalDepth(-2.f);
		addDDList->SetLocalPosition(MkFloat2(10.f, HEIGHT - 160.f));

		MkWindowBaseNode* btnInst = wndFactory.CreateButtonTypeNode(L"Reload_AddMap", MkWindowThemeData::eCT_NormalBtn, L"Add 파일 다시 열기");
		btnInst->SetLocalPosition(MkFloat2(10.f, HEIGHT - 190.f));
		bodyFrame->AttachChildNode(btnInst);
	}

	// mixer
	{
		MkPanel& textPanel = bodyFrame->CreatePanel(L"ST_Mixer");
		textPanel.SetLocalPosition(MkFloat2(10.f, HEIGHT - 240.f));
		textPanel.SetLocalDepth(-1.f);
		textPanel.SetTextMsg(L"[Mult color] ~ [Add color]");

		MkSliderControlNode* sliderNode = MkSliderControlNode::CreateChildNode(bodyFrame, L"SL_Mixer");
		sliderNode->SetHorizontalSlider(MkWindowThemeData::DefaultThemeName, MkWindowThemeData::eFT_Small, 204.f, 0, 100, static_cast<int>(SREPO.GetDepthFogMixer() * 100.f));
		sliderNode->SetLocalPosition(MkFloat2(10.f, HEIGHT - 270.f));
		sliderNode->SetLocalDepth(-1.f);
	}

	{
		MkWindowBaseNode* btnInst = wndFactory.CreateButtonTypeNode(L"Btn_Reset", MkWindowThemeData::eCT_CancelBtn, L"설정 초기화");
		btnInst->SetLocalPosition(MkFloat2(10.f, 10.f));
		bodyFrame->AttachChildNode(btnInst);
	}

	m_MultMapPath.ConvertToRootBasisAbsolutePath(L"Gradation\\Mult");
	_UpdateMapDir(m_MultMapPath, L"DD_MultMap");
	UpdateMultGradation();

	m_AddMapPath.ConvertToRootBasisAbsolutePath(L"Gradation\\Add");
	_UpdateMapDir(m_AddMapPath, L"DD_AddMap");
	UpdateAddGradation();
}

void DepthFogEditWindowNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (eventType == ePA_SNE_SliderMoved)
	{
		if (path.GetSize() == 2)
		{
			int factor;
			argument->GetData(MkSliderControlNode::ArgKey_SliderPos, factor, 0);

			SREPO.SetDepthFogMixer(static_cast<float>(factor) / 100.f);

			if (m_RootNode != NULL)
			{
				MkPanel* mainPanel = m_RootNode->GetPanel(L"Main");
				if (mainPanel != NULL)
				{
					mainPanel->SetUserDefinedProperty(L"g_FogMixer", SREPO.GetDepthFogMixer());
				}
			}
			return;
		}
	}
	else if (eventType == ePA_SNE_DropDownItemSet)
	{
		if (path.GetSize() == 2)
		{
			if (path[1].Equals(0, L"DD_MultMap") && (!m_Reloading))
			{
				MkHashStr buffer;
				if (argument->GetDataEx(MkListBoxControlNode::ArgKey_Item, buffer, 0) && (!buffer.Empty()))
				{
					SREPO.SetDepthFogMultMap(buffer);
					UpdateMultGradation();
				}
			}
			else if (path[1].Equals(0, L"DD_AddMap") && (!m_Reloading))
			{
				MkHashStr buffer;
				if (argument->GetDataEx(MkListBoxControlNode::ArgKey_Item, buffer, 0) && (!buffer.Empty()))
				{
					SREPO.SetDepthFogAddMap(buffer);
					UpdateAddGradation();
				}
			}
		}
	}
	else if (eventType == ePA_SNE_CursorLBtnReleased)
	{
		if (path.GetSize() == 2)
		{
			if (path[1].Equals(0, L"RefreshDir"))
			{
				_UpdateMapDir(m_MultMapPath, L"DD_MultMap");
				UpdateMultGradation();

				_UpdateMapDir(m_AddMapPath, L"DD_AddMap");
				UpdateAddGradation();
			}
			else if (path[1].Equals(0, L"Reload_MultMap"))
			{
				UpdateMultGradation();
				return;
			}
			else if (path[1].Equals(0, L"Reload_AddMap"))
			{
				UpdateAddGradation();
				return;
			}
			else if (path[1].Equals(0, L"Btn_Reset"))
			{
				SREPO.ResetDepthFogValue();
				_ResetEffectValue();
			}
		}
	}

	MkTitleBarControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void DepthFogEditWindowNode::Activate(void)
{
	_ResetEffectValue();

	MkTitleBarControlNode::Activate();
}

bool DepthFogEditWindowNode::UpdateMultGradation(void)
{
	MkHashStr targetKey;

	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");
	MkDropDownListControlNode* ddList = dynamic_cast<MkDropDownListControlNode*>(bodyFrame->GetChildNode(L"DD_MultMap"));
	if (ddList->ItemExist(SREPO.GetDepthFogMultMap()))
	{
		targetKey = SREPO.GetDepthFogMultMap();
	}
	else
	{
		MkArray<MkHashStr> keyList;
		ddList->GetItemKeyList(keyList);
		if (!keyList.Empty())
		{
			targetKey = keyList[0];
			SREPO.SetDepthFogMultMap(targetKey);
		}
	}

	if (targetKey.Empty())
		return false;

	ddList->SetTargetItemKey(targetKey);

	MkPathName newPath = m_MultMapPath + targetKey.GetString() + L".png";

	bool ok = false;
	MkPanel* mainPanel = m_RootNode->GetPanel(L"Main");
	if (mainPanel != NULL)
	{
		const MkBaseTexture* lastTex = mainPanel->GetEffectTexturePtr2();
		if (lastTex != NULL)
		{
			mainPanel->SetEffectTexture2(NULL);
			MK_BITMAP_POOL.UnloadBitmapTexture(lastTex->GetPoolKey());
		}
		return mainPanel->SetEffectTexture2(newPath);
	}
	return false;
}

bool DepthFogEditWindowNode::UpdateAddGradation(void)
{
	MkHashStr targetKey;

	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");
	MkDropDownListControlNode* ddList = dynamic_cast<MkDropDownListControlNode*>(bodyFrame->GetChildNode(L"DD_AddMap"));
	if (ddList->ItemExist(SREPO.GetDepthFogAddMap()))
	{
		targetKey = SREPO.GetDepthFogAddMap();
	}
	else
	{
		MkArray<MkHashStr> keyList;
		ddList->GetItemKeyList(keyList);
		if (!keyList.Empty())
		{
			targetKey = keyList[0];
			SREPO.SetDepthFogAddMap(targetKey);
		}
	}

	if (targetKey.Empty())
		return false;

	ddList->SetTargetItemKey(targetKey);

	MkPathName newPath = m_AddMapPath + targetKey.GetString() + L".png";

	bool ok = false;
	MkPanel* mainPanel = m_RootNode->GetPanel(L"Main");
	if (mainPanel != NULL)
	{
		const MkBaseTexture* lastTex = mainPanel->GetEffectTexturePtr3();
		if (lastTex != NULL)
		{
			mainPanel->SetEffectTexture3(NULL);
			MK_BITMAP_POOL.UnloadBitmapTexture(lastTex->GetPoolKey());
		}
		return mainPanel->SetEffectTexture3(newPath);
	}
	return false;
}

//------------------------------------------------------------------------------------------------//

void DepthFogEditWindowNode::_UpdateMapDir(const MkPathName& dirPath, const MkHashStr& ddListName)
{
	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");
	MkDropDownListControlNode* ddList = dynamic_cast<MkDropDownListControlNode*>(bodyFrame->GetChildNode(ddListName));

	ddList->ClearAllItems();
	
	MkArray<MkPathName> filePathList;
	MkArray<MkPathName> extensionFilter;
	extensionFilter.PushBack(L"png");

	dirPath.GetWhiteFileList(filePathList, NULL, &extensionFilter, NULL, NULL, false, false);

	m_Reloading = true;
	MK_INDEXING_LOOP(filePathList, i)
	{
		MkPathName fileName = filePathList[i].GetFileName(false);
		ddList->AddItem(fileName, fileName);
	}
	m_Reloading = false;
}

void DepthFogEditWindowNode::_ResetEffectValue(void)
{
	UpdateMultGradation();
	UpdateAddGradation();

	MkSceneNode* bodyFrame = GetChildNode(L"BodyFrame");
	MkSliderControlNode* sliderNode = dynamic_cast<MkSliderControlNode*>(bodyFrame->GetChildNode(L"SL_Mixer"));
	sliderNode->SetSliderValue(static_cast<int>(SREPO.GetDepthFogMixer() * 100.f));
}

//------------------------------------------------------------------------------------------------//