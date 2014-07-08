
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkS2D_MkProjectDefinition.h"
#include "MkS2D_MkWindowResourceManager.h"
#include "MkS2D_MkScrollBarNode.h"
#include "MkS2D_MkSceneNodeFamilyDefinition.h"


const static MkHashStr PREV_BTN_NODE_NAME = L"Prev";
const static MkHashStr NEXT_BTN_NODE_NAME = L"Next";
const static MkHashStr BAR_NODE_NAME = L"Bar";
const static MkHashStr SLIDE_NODE_NAME = L"Slide";

//------------------------------------------------------------------------------------------------//

bool MkScrollBarNode::CreateScrollBar(const MkHashStr& themeName, eBarDirection direction, bool hasDirBtn, float length)
{
	m_BarDirection = direction;
	MK_CHECK((direction == eVertical) || (direction == eHorizontal), GetNodeName().GetString() + L" MkScrollBarNode�� �߸��� eBarDirection�� �ʱ�ȭ �õ�")
		return false;

	bool isVertical = (m_BarDirection == eVertical);

	MkBaseWindowNode* prevNode = NULL;
	MkBaseWindowNode* nextNode = NULL;
	if (hasDirBtn)
	{
		prevNode = __CreateWindowPreset(this, PREV_BTN_NODE_NAME, themeName, (isVertical) ? eS2D_WPC_DirUpButton : eS2D_WPC_DirLeftButton, MkFloat2(0.f, 0.f));
		nextNode = __CreateWindowPreset(this, NEXT_BTN_NODE_NAME, themeName, (isVertical) ? eS2D_WPC_DirDownButton : eS2D_WPC_DirRightButton, MkFloat2(0.f, 0.f));

		MK_CHECK((prevNode != NULL) && (nextNode != NULL), GetNodeName().GetString() + L" MkScrollBarNode���� prev/next ��ư ���� ����")
			return false;
	}

	float prevBtnLength = (prevNode == NULL) ? 0.f : ((isVertical) ? prevNode->GetPresetComponentSize().y : prevNode->GetPresetComponentSize().x);
	float nextBtnLength = (nextNode == NULL) ? 0.f : ((isVertical) ? nextNode->GetPresetComponentSize().y : nextNode->GetPresetComponentSize().x);

	const float MARGIN = MK_WR_PRESET.GetMargin();
	float minLength = prevBtnLength + nextBtnLength + MARGIN * 2.f;
	float totalLength = GetMax<float>(minLength, length); // ���� ����
	float bodyLength = totalLength - prevBtnLength - nextBtnLength;

	m_SlideNode = NULL;

	MkFloat2 barSize = MkFloat2(bodyLength, bodyLength);
	MkBaseWindowNode* barNode = __CreateWindowPreset(this, BAR_NODE_NAME, themeName, (isVertical) ? eS2D_WPC_VSlideBar : eS2D_WPC_HSlideBar, barSize);
	MK_CHECK(barNode != NULL, GetNodeName().GetString() + L" MkScrollBarNode���� bar ���� ����")
		return false;

	m_SlideNode = __CreateWindowPreset(barNode, SLIDE_NODE_NAME, themeName, (isVertical) ? eS2D_WPC_VSlideButton : eS2D_WPC_HSlideButton, barSize);
	MK_CHECK(m_SlideNode != NULL, GetNodeName().GetString() + L" MkScrollBarNode���� slide ���� ����")
		return false;

	m_MaxSlideSize = (isVertical) ? barNode->GetPresetComponentSize().y : barNode->GetPresetComponentSize().x;

	if (hasDirBtn)
	{
		if (isVertical)
		{
			barNode->SetLocalPosition(MkFloat2(0.f, nextNode->GetPresetComponentSize().y));
			prevNode->SetLocalPosition(MkFloat2(0.f, nextNode->GetPresetComponentSize().y + m_MaxSlideSize));
		}
		else
		{
			barNode->SetLocalPosition(MkFloat2(prevNode->GetPresetComponentSize().x, 0.f));
			nextNode->SetLocalPosition(MkFloat2(prevNode->GetPresetComponentSize().x + m_MaxSlideSize, 0.f));
		}

		prevNode->SetAttribute(eIgnoreMovement, true);
		nextNode->SetAttribute(eIgnoreMovement, true);
	}

	barNode->SetAttribute(eIgnoreMovement, true);

	m_SlideNode->SetLocalDepth(-MKDEF_BASE_WINDOW_DEPTH_GRID);
	m_SlideNode->SetAttribute(eDragMovement, true);
	m_SlideNode->SetAttribute(eConfinedToParent, true);

	return true;
}

void MkScrollBarNode::SetPageInfo(unsigned int totalPageSize, unsigned int onePageSize, unsigned int sizePerGrid, unsigned int gridsPerAction)
{
	MK_CHECK(m_SlideNode != NULL, GetNodeName().GetString() + L" MkScrollBarNode�� �������� �ʾҴµ� SetPageInfo() �õ�")
		return;

	// �ּ� 1
	m_OnePageSize = GetMax<unsigned int>(1, onePageSize);

	// [1 ~ m_OnePageSize]
	m_SizePerGrid = Clamp<unsigned int>(sizePerGrid, 1, m_OnePageSize);

	// ��ġ �ʱ�ȭ
	m_CurrentGridPosition = 0;

	SetPageInfo(totalPageSize);

	m_GridsPerAction = (m_GridCount > 1) ? static_cast<int>(Clamp<unsigned int>(gridsPerAction, 1, m_GridCount)) : 1;
}

void MkScrollBarNode::SetPageInfo(unsigned int totalPageSize)
{
	MK_CHECK(m_SlideNode != NULL, GetNodeName().GetString() + L" MkScrollBarNode�� �������� �ʾҴµ� SetPageInfo() �õ�")
		return;
	
	m_TotalPageSize = totalPageSize;

	m_GridCount = 0;

	// [0 < m_SizePerGrid <= m_OnePageSize < m_TotalPageSize] ���谡 �����ϸ�
	if ((0 < m_SizePerGrid) && (m_SizePerGrid <= m_OnePageSize) && (m_OnePageSize < m_TotalPageSize))
	{
		unsigned int maxGridSize = m_TotalPageSize - m_OnePageSize;
		m_GridCount = maxGridSize / m_SizePerGrid + 1;
		if ((maxGridSize % m_SizePerGrid) > 0)
		{
			++m_GridCount;
		}
	}

	_UpdateSlideTransform();
}

void MkScrollBarNode::SetGridPosition(unsigned int gridPosition)
{
	if ((gridPosition < m_GridCount) && (gridPosition != m_CurrentGridPosition))
	{
		m_CurrentGridPosition = gridPosition;

		_UpdateSlideTransform();
		_PushWindowEvent(MkSceneNodeFamilyDefinition::eScrollPositionChanged);
	}
}

void MkScrollBarNode::Load(const MkDataNode& node)
{
	int barDir = 0;
	node.GetData(MkSceneNodeFamilyDefinition::ScrollBar::BarDirectionKey, barDir, 0);
	m_BarDirection = static_cast<eBarDirection>(barDir);

	unsigned int totalPageSize = 0;
	node.GetData(MkSceneNodeFamilyDefinition::ScrollBar::TotalPageSizeKey, totalPageSize, 0);

	unsigned int onePageSize = 1;
	node.GetData(MkSceneNodeFamilyDefinition::ScrollBar::OnePageSizeKey, onePageSize, 0);

	unsigned int sizePerGrid = 1;
	node.GetData(MkSceneNodeFamilyDefinition::ScrollBar::SizePerGridKey, sizePerGrid, 0);

	int gridsPerAction = 1;
	node.GetData(MkSceneNodeFamilyDefinition::ScrollBar::GridsPerActionKey, gridsPerAction, 0);

	// MkBaseWindowNode
	MkBaseWindowNode::Load(node);

	// ���� ���� �� ������ �ʱ�ȭ
	MkBaseWindowNode* barNode = dynamic_cast<MkBaseWindowNode*>(GetChildNode(BAR_NODE_NAME));
	if (barNode != NULL)
	{
		m_SlideNode = dynamic_cast<MkBaseWindowNode*>(barNode->GetChildNode(SLIDE_NODE_NAME));
		if (m_SlideNode != NULL)
		{
			m_MaxSlideSize = (m_BarDirection == eVertical) ? barNode->GetPresetComponentSize().y : barNode->GetPresetComponentSize().x;

			SetPageInfo(totalPageSize, onePageSize, sizePerGrid, gridsPerAction);
		}
	}
}

void MkScrollBarNode::Save(MkDataNode& node)
{
	// ���� ���ø��� ������ ���ø� ����
	_ApplyBuildingTemplateToSave(node, MkSceneNodeFamilyDefinition::ScrollBar::TemplateName);

	node.SetData(MkSceneNodeFamilyDefinition::ScrollBar::BarDirectionKey, static_cast<int>(m_BarDirection), 0);
	node.SetData(MkSceneNodeFamilyDefinition::ScrollBar::TotalPageSizeKey, m_TotalPageSize, 0);
	node.SetData(MkSceneNodeFamilyDefinition::ScrollBar::OnePageSizeKey, m_OnePageSize, 0);
	node.SetData(MkSceneNodeFamilyDefinition::ScrollBar::SizePerGridKey, m_SizePerGrid, 0);
	node.SetData(MkSceneNodeFamilyDefinition::ScrollBar::GridsPerActionKey, m_GridsPerAction, 0);

	// MkBaseWindowNode
	MkBaseWindowNode::Save(node);
}

bool MkScrollBarNode::HitEventMouseWheelMove(int delta, const MkFloat2& position)
{
	switch (delta) // 3��ӱ����� �ν�
	{
	case 120:
		SetGridPosition(_GetNewGridPosition(-m_GridsPerAction));
		return true;
	case 240:
		SetGridPosition(_GetNewGridPosition(-m_GridsPerAction * 2));
		return true;
	case 360:
		SetGridPosition(_GetNewGridPosition(-m_GridsPerAction * 3));
		return true;
	case -120:
		SetGridPosition(_GetNewGridPosition(m_GridsPerAction));
		return true;
	case -240:
		SetGridPosition(_GetNewGridPosition(m_GridsPerAction * 2));
		return true;
	case -360:
		SetGridPosition(_GetNewGridPosition(m_GridsPerAction * 3));
		return true;
	}
	return false;
}

void MkScrollBarNode::UseWindowEvent(WindowEvent& evt)
{
	if (evt.node->GetNodeName() == PREV_BTN_NODE_NAME)
	{
		switch (evt.type)
		{
		case MkSceneNodeFamilyDefinition::eCursorLeftRelease:
			SetGridPosition(_GetNewGridPosition(-m_GridsPerAction));
			break;
		}
	}
	else if (evt.node->GetNodeName() == NEXT_BTN_NODE_NAME)
	{
		switch (evt.type)
		{
		case MkSceneNodeFamilyDefinition::eCursorLeftRelease:
			SetGridPosition(_GetNewGridPosition(m_GridsPerAction));
			break;
		}
	}
}

MkScrollBarNode::MkScrollBarNode(const MkHashStr& name) : MkBaseWindowNode(name)
{
	m_BarDirection = eVertical;
	m_TotalPageSize = 0;
	m_OnePageSize = 1;
	m_SizePerGrid = 1;
	m_GridsPerAction = 1;
	
	m_SlideNode = NULL;
	m_MaxSlideSize = 0.f;
	m_GridCount = 0;
	m_CurrentGridPosition = 0;
	m_CurrentPagePosition = 0;
}

//------------------------------------------------------------------------------------------------//

void MkScrollBarNode::__GenerateBuildingTemplate(void)
{
	MkDataNode node;
	MkDataNode* tNode = node.CreateChildNode(MkSceneNodeFamilyDefinition::ScrollBar::TemplateName);
	MK_CHECK(tNode != NULL, MkSceneNodeFamilyDefinition::ScrollBar::TemplateName.GetString() + L" template node alloc ����")
		return;

	tNode->ApplyTemplate(MkSceneNodeFamilyDefinition::BaseWindow::TemplateName); // MkBaseWindowNode�� template ����

	tNode->CreateUnit(MkSceneNodeFamilyDefinition::ScrollBar::BarDirectionKey, static_cast<int>(0));
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::ScrollBar::TotalPageSizeKey, static_cast<unsigned int>(0));
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::ScrollBar::OnePageSizeKey, static_cast<unsigned int>(1));
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::ScrollBar::SizePerGridKey, static_cast<unsigned int>(1));
	tNode->CreateUnit(MkSceneNodeFamilyDefinition::ScrollBar::GridsPerActionKey, static_cast<int>(1));

	tNode->DeclareToTemplate(true);
}

void MkScrollBarNode::_UpdateSlideTransform(void)
{
	if (m_SlideNode != NULL)
	{
		// ũ��
		float slideSize = m_MaxSlideSize;
		if (m_GridCount > 0) // m_GridCount�� 1 �̻��̶�� ���� m_TotalPageSize ���� 1 �̻����� �ǹ�
		{
			float pageRate = GetMin<float>(static_cast<float>(m_OnePageSize) / static_cast<float>(m_TotalPageSize), 1.f); // maximum 1.f ����
			slideSize *= pageRate;
		}
		m_SlideNode->SetPresetComponentSize(MkFloat2(slideSize, slideSize));

		// ��ġ
		MkFloat2 slidePos;
		if (slideSize < m_MaxSlideSize)
		{
			unsigned int beginPos = m_SizePerGrid * m_CurrentGridPosition;
			unsigned int endPos = GetMin<unsigned int>(beginPos + m_OnePageSize, m_TotalPageSize);
			m_CurrentPagePosition = endPos - m_OnePageSize;

			float fPos = static_cast<float>(m_CurrentPagePosition) * m_MaxSlideSize / static_cast<float>(m_TotalPageSize);
			if (m_BarDirection == eVertical)
			{
				slidePos.y = m_MaxSlideSize - fPos - slideSize;
			}
			else
			{
				slidePos.x = fPos;
			}
		}
		m_SlideNode->SetLocalPosition(slidePos);
	}
}

unsigned int MkScrollBarNode::_GetNewGridPosition(int offset) const
{
	if (m_GridCount > 0)
	{
		int pos = Clamp<int>(static_cast<int>(m_CurrentGridPosition) + offset, 0, static_cast<int>(m_GridCount) - 1);
		return static_cast<unsigned int>(pos);
	}
	return 0;
}

//------------------------------------------------------------------------------------------------//