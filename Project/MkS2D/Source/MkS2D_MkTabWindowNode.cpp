
#include <math.h>

#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkTabWindowNode.h"
#include "MkS2D_MkSceneNodeFamilyDefinition.h"


const static MkHashStr FRONT_BTN_NAME = L"__#Front";
const static MkHashStr REAR_BTN_NAME = L"__#Rear";
const static MkHashStr BODY_WIN_NAME = L"__#Body";

//------------------------------------------------------------------------------------------------//
bool MkTabWindowNode::CreateTabRoot(const MkHashStr& themeName, eTabAlignment tabAlighment, const MkFloat2& tabButtonSize, const MkFloat2& tabBodySize)
{
	m_TabAlighment = tabAlighment;
	m_TabButtonSize = tabButtonSize;
	m_TabBodySize = tabBodySize;

	const float MARGIN = MK_WR_PRESET.GetMargin();
	const float DMARGIN = MARGIN * 2.f;

	m_TabButtonSize.x = GetMax<float>(m_TabButtonSize.x, DMARGIN);
	m_TabButtonSize.y = GetMax<float>(m_TabButtonSize.y, DMARGIN);
	m_TabBodySize.x = GetMax<float>(m_TabBodySize.x, DMARGIN);
	m_TabBodySize.y = GetMax<float>(m_TabBodySize.y, DMARGIN);

	m_TabCapacity = static_cast<unsigned int>(floor(m_TabBodySize.x / (m_TabButtonSize.x + MARGIN)));
	MK_CHECK(m_TabCapacity > 0, L"MkTabWindowNode에서 잘못된 크기 설정으로 생성 시도")
		return false;

	return CreateWindowPreset(themeName, eS2D_WPC_BackgroundWindow, m_TabBodySize);
}

bool MkTabWindowNode::SetTabButtonSize(const MkFloat2& tabButtonSize)
{
	MK_CHECK(m_TabCapacity > 0, L"초기화되지 않은 " + GetNodeName().GetString() + L" MkTabWindowNode에 SetTabButtonSize() 시도")
		return false;

	const float MARGIN = MK_WR_PRESET.GetMargin();
	const float DMARGIN = MARGIN * 2.f;

	MkFloat2 tmpSize;
	tmpSize.x = GetMax<float>(tabButtonSize.x, DMARGIN);
	tmpSize.y = GetMax<float>(tabButtonSize.y, DMARGIN);

	unsigned int tabCapacity = static_cast<unsigned int>(floor(m_TabBodySize.x / (tmpSize.x + MARGIN)));
	bool ok = ((tabCapacity > 0) && (tabCapacity >= m_TabList.GetSize()));
	if (ok)
	{
		m_TabButtonSize = tmpSize;
		m_TabCapacity = tabCapacity;

		MK_INDEXING_LOOP(m_TabList, i)
		{
			MkSceneNode* tabNode = GetChildNode(m_TabList[i]);
			if (tabNode != NULL)
			{
				MkBaseWindowNode* frontBtn = dynamic_cast<MkBaseWindowNode*>(tabNode->GetChildNode(FRONT_BTN_NAME));
				MkBaseWindowNode* rearBtn = dynamic_cast<MkBaseWindowNode*>(tabNode->GetChildNode(REAR_BTN_NAME));
				if ((frontBtn != NULL) && (rearBtn != NULL))
				{
					frontBtn->SetPresetComponentSize(m_TabButtonSize);
					rearBtn->SetPresetComponentSize(m_TabButtonSize);
				}
			}
		}

		_RepositionTabs(0);
	}
	return ok;
}

bool MkTabWindowNode::SetTabBodySize(const MkFloat2& tabBodySize)
{
	MK_CHECK(m_TabCapacity > 0, L"초기화되지 않은 " + GetNodeName().GetString() + L" MkTabWindowNode에 SetTabBodySize() 시도")
		return false;

	const float MARGIN = MK_WR_PRESET.GetMargin();
	const float DMARGIN = MARGIN * 2.f;

	MkFloat2 tmpSize;
	tmpSize.x = GetMax<float>(tabBodySize.x, DMARGIN);
	tmpSize.y = GetMax<float>(tabBodySize.y, DMARGIN);

	unsigned int tabCapacity = static_cast<unsigned int>(floor(tmpSize.x / (m_TabButtonSize.x + MARGIN)));
	bool ok = ((tabCapacity > 0) && (tabCapacity >= m_TabList.GetSize()));
	if (ok)
	{
		m_TabBodySize = tmpSize;
		m_TabCapacity = tabCapacity;

		SetPresetComponentSize(m_TabBodySize);

		_RepositionTabs(0);
	}
	return ok;
}

MkBaseWindowNode* MkTabWindowNode::AddTab(const MkHashStr& tabName, const ItemTagInfo& tagInfo, MkBaseWindowNode* customWindow)
{
	MK_CHECK(m_TabCapacity > 0, L"초기화되지 않은 " + GetNodeName().GetString() + L" MkTabWindowNode에 tab 생성 시도 : " + tabName.GetString())
		return NULL;

	MK_CHECK(!tabName.Empty(), GetNodeName().GetString() + L" MkTabWindowNode에 이름 없는 tab 생성 시도")
		return NULL;

	MK_CHECK(!m_Tabs.Exist(tabName), GetNodeName().GetString() + L" MkTabWindowNode에 이미 존재하는 tab 생성 시도 : " + tabName.GetString())
		return NULL;

	unsigned int tabIndex = m_TabList.GetSize();
	if (tabIndex >= m_TabCapacity)
		return NULL;

	MkBaseWindowNode* targetNode = new MkBaseWindowNode(tabName);
	MK_CHECK(targetNode != NULL, GetNodeName().GetString() + L" MkTabWindowNode에서 tab alloc 실패 : " + tabName.GetString())
		return NULL;

	MkBaseWindowNode* frontBtn = __CreateWindowPreset(targetNode, FRONT_BTN_NAME, m_PresetThemeName, eS2D_WPC_TabFront, m_TabButtonSize);
	if (frontBtn != NULL)
	{
		frontBtn->SetPresetComponentItemTag(tagInfo);
		frontBtn->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
		frontBtn->SetAttribute(eIgnoreMovement, true);
		frontBtn->SetVisible(m_CurrentFrontTab.Empty());
	}

	MkBaseWindowNode* rearBtn = __CreateWindowPreset(targetNode, REAR_BTN_NAME, m_PresetThemeName, eS2D_WPC_TabRearButton, m_TabButtonSize);
	if (rearBtn != NULL)
	{
		rearBtn->SetPresetComponentItemTag(tagInfo);
		rearBtn->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
		rearBtn->SetAttribute(eIgnoreMovement, true);
		rearBtn->SetVisible(!m_CurrentFrontTab.Empty());
	}

	MkBaseWindowNode* bodyWin = NULL;
	if (customWindow == NULL)
	{
		bodyWin = new MkBaseWindowNode(BODY_WIN_NAME);
	}
	else
	{
		customWindow->ChangeNodeName(BODY_WIN_NAME);
		bodyWin = customWindow;
	}
	if (bodyWin != NULL)
	{
		bodyWin->SetAttribute(eIgnoreMovement, true);
		bodyWin->SetVisible(m_CurrentFrontTab.Empty());

		targetNode->AttachChildNode(bodyWin);
	}

	MK_CHECK((frontBtn != NULL) && (rearBtn != NULL) && (bodyWin != NULL), GetNodeName().GetString() + L" MkTabWindowNode에서 tab 생성 실패 : " + tabName.GetString())
	{
		delete targetNode;
		return NULL;
	}

	targetNode->SetAttribute(eIgnoreMovement, true);

	m_TabList.PushBack(tabName);
	
	AttachChildNode(targetNode);
	m_Tabs.Create(tabName, targetNode);

	switch (m_TabAlighment)
	{
	case eLeftside:
		{
			MkFloat2 btnPosition(static_cast<float>(tabIndex) * (m_TabButtonSize.x + MK_WR_PRESET.GetMargin()), m_TabBodySize.y);
			frontBtn->SetLocalPosition(btnPosition);
			rearBtn->SetLocalPosition(btnPosition);
		}
		break;

	case eRightside:
		_RepositionTabs(0);
		break;
	}

	if (m_CurrentFrontTab.Empty())
	{
		m_CurrentFrontTab = tabName;

		_PushWindowEvent(MkSceneNodeFamilyDefinition::eTabSelection);
	}

	return bodyWin;
}

bool MkTabWindowNode::RemoveTab(const MkHashStr& tabName)
{
	if (m_Tabs.Exist(tabName))
	{
		MkSceneNode* targetNode = GetChildNode(tabName);
		if (targetNode != NULL)
		{
			unsigned int pos = m_TabList.FindFirstInclusion(MkArraySection(0), tabName);
			m_TabList.Erase(MkArraySection(pos, 1));
			m_Tabs.Erase(tabName);

			DetachChildNode(tabName);
			delete targetNode;

			switch (m_TabAlighment)
			{
			case eLeftside: _RepositionTabs(pos); break;
			case eRightside: _RepositionTabs(0); break;
			}

			if (tabName == m_CurrentFrontTab)
			{
				_TurnOffCurrentFrontTab();
			}
			return true;
		}
	}
	return false;
}

MkBaseWindowNode* MkTabWindowNode::GetWindowNodeOfTab(const MkHashStr& tabName)
{
	if (m_Tabs.Exist(tabName))
	{
		MkSceneNode* targetTab = GetChildNode(tabName);
		if (targetTab != NULL)
		{
			return dynamic_cast<MkBaseWindowNode*>(targetTab->GetChildNode(BODY_WIN_NAME));
		}
	}
	return NULL;
}

bool MkTabWindowNode::SelectTab(const MkHashStr& tabName)
{
	bool ok = (m_Tabs.Exist(tabName) && (tabName != m_CurrentFrontTab));
	if (ok)
	{
		_MoveTargetTabToFront(tabName);
	}
	return ok;
}

bool MkTabWindowNode::SetTabEnable(const MkHashStr& tabName, bool enable)
{
	if (m_Tabs.Exist(tabName))
	{
		MkSceneNode* targetTab = GetChildNode(tabName);
		if (targetTab != NULL)
		{
			MkBaseWindowNode* targetRearBtn = dynamic_cast<MkBaseWindowNode*>(targetTab->GetChildNode(REAR_BTN_NAME));
			if (targetRearBtn != NULL)
			{
				if (enable == targetRearBtn->GetEnable())
					return true;

				// disable -> enable은 조건상 front tab이 아님을 보장. 다만 기존 front tab이 없을 경우(disable tab만 존재하는 경우) 대신 front tab이 됨
				if (enable)
				{
					targetRearBtn->SetEnable(true);

					if (m_CurrentFrontTab.Empty())
					{
						m_CurrentFrontTab = tabName;
						_SetTabState(m_CurrentFrontTab, true);

						_PushWindowEvent(MkSceneNodeFamilyDefinition::eTabSelection);
					}
				}
				// enable -> disable은 front tab일 가능성 고려
				else
				{
					_SetTabState(tabName, false);
					targetRearBtn->SetEnable(false);

					// disable 대상이 front tab이면 가장 앞에 있는 활성화된 tab을 front로 지정
					if (tabName == m_CurrentFrontTab)
					{
						_TurnOffCurrentFrontTab();
					}
				}
				return true;
			}
		}
	}
	return false;
}

bool MkTabWindowNode::GetTabEnable(const MkHashStr& tabName) const
{
	if (m_Tabs.Exist(tabName))
	{
		const MkSceneNode* targetTab = GetChildNode(tabName);
		if (targetTab != NULL)
		{
			const MkBaseWindowNode* targetRearBtn = dynamic_cast<const MkBaseWindowNode*>(targetTab->GetChildNode(REAR_BTN_NAME));
			if (targetRearBtn != NULL)
			{
				return targetRearBtn->GetEnable();
			}
		}
	}
	return false;
}

unsigned int MkTabWindowNode::GetTabSequence(const MkHashStr& tabName) const
{
	return m_TabList.FindFirstInclusion(MkArraySection(0), tabName);
}

bool MkTabWindowNode::SetTabSequence(const MkHashStr& tabName, unsigned int position)
{
	if (position < m_TabList.GetSize())
	{
		unsigned int lastPos = m_TabList.FindFirstInclusion(MkArraySection(0), tabName);
		if (lastPos != MKDEF_ARRAY_ERROR)
		{
			if (position != lastPos)
			{
				m_TabList.Erase(MkArraySection(lastPos, 1));
				m_TabList.Insert(position, tabName);

				_RepositionTabs(GetMin<unsigned int>(lastPos, position));
			}
			return true;
		}
	}
	return false;
}

bool MkTabWindowNode::MoveTabToOneStepForward(const MkHashStr& tabName)
{
	unsigned int pos = m_TabList.FindFirstInclusion(MkArraySection(0), tabName);
	if (pos != MKDEF_ARRAY_ERROR)
	{
		return (pos == 0) ? true : SetTabSequence(tabName, pos - 1);
	}
	return false;
}

bool MkTabWindowNode::MoveTabToOneStepBackword(const MkHashStr& tabName)
{
	unsigned int pos = m_TabList.FindFirstInclusion(MkArraySection(0), tabName);
	if (pos != MKDEF_ARRAY_ERROR)
	{
		++pos;
		return (pos == m_TabList.GetSize()) ? true : SetTabSequence(tabName, pos);
	}
	return false;
}

void MkTabWindowNode::Load(const MkDataNode& node)
{
	int tabAlighment = static_cast<int>(eLeftside);
	node.GetData(MkSceneNodeFamilyDefinition::TabWindow::TabAlighmentKey, tabAlighment, 0);
	m_TabAlighment = static_cast<eTabAlignment>(tabAlighment);

	MkVec2 tabButtonSize;
	node.GetData(MkSceneNodeFamilyDefinition::TabWindow::TabButtonSizeKey, tabButtonSize, 0);
	m_TabButtonSize = MkFloat2(tabButtonSize.x, tabButtonSize.y);

	MkVec2 tabBodySize;
	node.GetData(MkSceneNodeFamilyDefinition::TabWindow::TabBodySizeKey, tabBodySize, 0);
	m_TabBodySize = MkFloat2(tabBodySize.x, tabBodySize.y);

	MkArray<MkStr> tabList;
	node.GetData(MkSceneNodeFamilyDefinition::TabWindow::TabListKey, tabList);

	m_TabCapacity = static_cast<unsigned int>(floor(m_TabBodySize.x / (m_TabButtonSize.x + MK_WR_PRESET.GetMargin())));

	// MkBaseWindowNode
	MkBaseWindowNode::Load(node);

	// 하위 노드 구성이 완료된 후 진행
	if (!tabList.Empty())
	{
		m_TabList.Reserve(tabList.GetSize());

		MK_INDEXING_LOOP(tabList, i)
		{
			const MkHashStr& tabName = tabList[i];
			MkBaseWindowNode* targetTab = dynamic_cast<MkBaseWindowNode*>(GetChildNode(tabName));
			if (targetTab != NULL)
			{
				m_TabList.PushBack(tabName);
				m_Tabs.Create(tabName, targetTab);

				MkSceneNode* frontBtn = targetTab->GetChildNode(FRONT_BTN_NAME);
				if ((frontBtn != NULL) && frontBtn->GetVisible())
				{
					m_CurrentFrontTab = tabName;
				}
			}
		}
	}
}

void MkTabWindowNode::Save(MkDataNode& node)
{
	// 기존 템플릿이 없으면 템플릿 적용
	_ApplyBuildingTemplateToSave(node, MkSceneNodeFamilyDefinition::TabWindow::TemplateName);

	node.SetData(MkSceneNodeFamilyDefinition::TabWindow::TabAlighmentKey, static_cast<int>(m_TabAlighment), 0);
	node.SetData(MkSceneNodeFamilyDefinition::TabWindow::TabButtonSizeKey, MkVec2(m_TabButtonSize.x, m_TabButtonSize.y), 0);
	node.SetData(MkSceneNodeFamilyDefinition::TabWindow::TabBodySizeKey, MkVec2(m_TabBodySize.x, m_TabBodySize.y), 0);

	if (!m_TabList.Empty())
	{
		MkArray<MkStr> tabList(m_TabList.GetSize());
		MK_INDEXING_LOOP(m_TabList, i)
		{
			tabList.PushBack(m_TabList[i].GetString());
		}
		node.SetData(MkSceneNodeFamilyDefinition::TabWindow::TabListKey, tabList);
	}

	// MkBaseWindowNode
	MkBaseWindowNode::Save(node);
}

void MkTabWindowNode::UseWindowEvent(WindowEvent& evt)
{
	switch (evt.type)
	{
	case MkSceneNodeFamilyDefinition::eCursorLeftRelease:
		{
			if (evt.node->GetNodeName() == REAR_BTN_NAME)
			{
				MkSceneNode* parentNode = evt.node->GetParentNode();
				if ((parentNode != NULL) && m_Tabs.Exist(parentNode->GetNodeName()))
				{
					_MoveTargetTabToFront(parentNode->GetNodeName());
				}
			}
		}
		break;
	}
}

void MkTabWindowNode::Clear(void)
{
	m_TabList.Clear();
	m_Tabs.Clear();
	m_TabCapacity = 0;

	MkBaseWindowNode::Clear();
}

MkTabWindowNode::MkTabWindowNode(const MkHashStr& name) : MkBaseWindowNode(name)
{
	m_TabAlighment = eLeftside;
	m_TabCapacity = 0;
}

//------------------------------------------------------------------------------------------------//

void MkTabWindowNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(MkSceneNodeFamilyDefinition::TabWindow::TemplateName);
	MK_CHECK(tNode != NULL, MkSceneNodeFamilyDefinition::TabWindow::TemplateName.GetString() + L" template node alloc 실패")
		return;

	tNode->ApplyTemplate(MkSceneNodeFamilyDefinition::BaseWindow::TemplateName); // MkBaseWindowNode의 template 적용

	tNode->CreateUnit(MkSceneNodeFamilyDefinition::TabWindow::TabAlighmentKey, static_cast<int>(eLeftside));
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::TabWindow::TabButtonSizeKey, MkVec2::Zero);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::TabWindow::TabBodySizeKey, MkVec2::Zero);
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::TabWindow::TabListKey, MkStr::Null);

	tNode->DeclareToTemplate(true);
}

//------------------------------------------------------------------------------------------------//

void MkTabWindowNode::_RepositionTabs(unsigned int startPos)
{
	if (startPos < m_TabList.GetSize())
	{
		MkFloat2 btnPosition;
		btnPosition.y = m_TabBodySize.y;
		const float MARGIN = MK_WR_PRESET.GetMargin();

		float offset = 0.f;
		if (m_TabAlighment == eRightside)
		{
			float totalBtnListSize = static_cast<float>(m_TabList.GetSize()) * m_TabButtonSize.x + static_cast<float>(m_TabList.GetSize() - 1) * MARGIN;
			offset = m_TabBodySize.x - totalBtnListSize;
		}

		for (unsigned int cnt = m_TabList.GetSize(), i = startPos; i < cnt; ++i)
		{
			MkSceneNode* tabNode = GetChildNode(m_TabList[i]);
			if (tabNode != NULL)
			{
				MkSceneNode* frontBtn = tabNode->GetChildNode(FRONT_BTN_NAME);
				MkSceneNode* rearBtn = tabNode->GetChildNode(REAR_BTN_NAME);
				if ((frontBtn != NULL) && (rearBtn != NULL))
				{
					btnPosition.x = offset + static_cast<float>(i) * (m_TabButtonSize.x + MARGIN);
					frontBtn->SetLocalPosition(btnPosition);
					rearBtn->SetLocalPosition(btnPosition);
				}
			}
		}
	}
}

void MkTabWindowNode::_SetTabState(const MkHashStr& tabName, bool front)
{
	MkSceneNode* targetTab = GetChildNode(tabName);
	if (targetTab != NULL)
	{
		MkSceneNode* frontBtn = targetTab->GetChildNode(FRONT_BTN_NAME);
		if (frontBtn != NULL)
		{
			frontBtn->SetVisible(front);
		}
		MkSceneNode* rearBtn = targetTab->GetChildNode(REAR_BTN_NAME);
		if (rearBtn != NULL)
		{
			rearBtn->SetVisible(!front);

			if (rearBtn->GetVisible())
			{
				MkBaseWindowNode* rb = dynamic_cast<MkBaseWindowNode*>(rearBtn);
				if (rb != NULL)
				{
					rb->SetPresetComponentWindowStateToDefault();
				}
			}
		}
		MkSceneNode* bodyWin = targetTab->GetChildNode(BODY_WIN_NAME);
		if (bodyWin != NULL)
		{
			bodyWin->SetVisible(front);
		}
	}
}

void MkTabWindowNode::_MoveTargetTabToFront(const MkHashStr& tabName)
{
	if (!m_CurrentFrontTab.Empty())
	{
		_SetTabState(m_CurrentFrontTab, false);
	}

	m_CurrentFrontTab = tabName;
	_SetTabState(m_CurrentFrontTab, true);

	_PushWindowEvent(MkSceneNodeFamilyDefinition::eTabSelection);
}

void MkTabWindowNode::_TurnOffCurrentFrontTab(void)
{
	MkHashStr tabName = m_CurrentFrontTab;

	MK_INDEXING_LOOP(m_TabList, i)
	{
		const MkHashStr& currTabName = m_TabList[i];
		if (tabName != currTabName)
		{
			MkSceneNode* currTab = GetChildNode(currTabName);
			if (currTab != NULL)
			{
				MkBaseWindowNode* currRearBtn = dynamic_cast<MkBaseWindowNode*>(currTab->GetChildNode(REAR_BTN_NAME));
				if ((currRearBtn != NULL) && currRearBtn->GetEnable())
				{
					m_CurrentFrontTab = currTabName;
					break;
				}
			}
		}
	}

	// 모든 tab이 비활성화 상태. front tab 없음
	if (tabName == m_CurrentFrontTab)
	{
		m_CurrentFrontTab.Clear();
	}
	// 해당 tab을 front로 지정
	else
	{
		_SetTabState(m_CurrentFrontTab, true);

		_PushWindowEvent(MkSceneNodeFamilyDefinition::eTabSelection);
	}
}

//------------------------------------------------------------------------------------------------//