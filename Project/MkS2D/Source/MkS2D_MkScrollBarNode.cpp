
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
	MK_CHECK((direction == eVertical) || (direction == eHorizontal), GetNodeName().GetString() + L" MkScrollBarNode에 잘못된 eBarDirection로 초기화 시도")
		return false;

	bool isVertical = (m_BarDirection == eVertical);

	MkBaseWindowNode* prevNode = NULL;
	MkBaseWindowNode* nextNode = NULL;
	if (hasDirBtn)
	{
		prevNode = __CreateWindowPreset(this, PREV_BTN_NODE_NAME, themeName, (isVertical) ? eS2D_WPC_DirUpButton : eS2D_WPC_DirLeftButton, MkFloat2(0.f, 0.f));
		nextNode = __CreateWindowPreset(this, NEXT_BTN_NODE_NAME, themeName, (isVertical) ? eS2D_WPC_DirDownButton : eS2D_WPC_DirRightButton, MkFloat2(0.f, 0.f));

		MK_CHECK((prevNode != NULL) && (nextNode != NULL), GetNodeName().GetString() + L" MkScrollBarNode에서 prev/next 버튼 생성 실패")
			return false;
	}

	float prevBtnLength = (prevNode == NULL) ? 0.f : ((isVertical) ? prevNode->GetPresetComponentSize().y : prevNode->GetPresetComponentSize().x);
	float nextBtnLength = (nextNode == NULL) ? 0.f : ((isVertical) ? nextNode->GetPresetComponentSize().y : nextNode->GetPresetComponentSize().x);

	const float MARGIN = MK_WR_PRESET.GetMargin();
	float minLength = prevBtnLength + nextBtnLength + MARGIN * 2.f;
	float totalLength = GetMax<float>(minLength, length); // 길이 결정
	float bodyLength = totalLength - prevBtnLength - nextBtnLength;

	m_SlideNode = NULL;

	MkFloat2 barSize = MkFloat2(bodyLength, bodyLength);
	MkBaseWindowNode* barNode = __CreateWindowPreset(this, BAR_NODE_NAME, themeName, (isVertical) ? eS2D_WPC_VSlideBar : eS2D_WPC_HSlideBar, barSize);
	MK_CHECK(barNode != NULL, GetNodeName().GetString() + L" MkScrollBarNode에서 bar 생성 실패")
		return false;

	m_SlideNode = __CreateWindowPreset(barNode, SLIDE_NODE_NAME, themeName, (isVertical) ? eS2D_WPC_VSlideButton : eS2D_WPC_HSlideButton, barSize);
	MK_CHECK(m_SlideNode != NULL, GetNodeName().GetString() + L" MkScrollBarNode에서 slide 생성 실패")
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

void MkScrollBarNode::SetPageInfo(unsigned int totalPageSize, unsigned int onePageSize, unsigned int sizePerGrid)
{
	MK_CHECK(m_SlideNode != NULL, GetNodeName().GetString() + L" MkScrollBarNode가 생성되지 않았는데 SetPageInfo() 시도")
		return;

	// 최소 1
	m_OnePageSize = GetMax<unsigned int>(1, onePageSize);

	// [1 ~ m_OnePageSize]
	m_SizePerGrid = Clamp<unsigned int>(sizePerGrid, 1, m_OnePageSize);

	// 위치 초기화
	m_CurrentGridPosition = 0;

	SetPageInfo(totalPageSize);
}

void MkScrollBarNode::SetPageInfo(unsigned int totalPageSize)
{
	MK_CHECK(m_SlideNode != NULL, GetNodeName().GetString() + L" MkScrollBarNode가 생성되지 않았는데 SetPageInfo() 시도")
		return;
	
	m_TotalPageSize = totalPageSize;

	m_GridCount = 0;

	// [0 < m_SizePerGrid <= m_OnePageSize < m_TotalPageSize] 관계가 성립하면
	if ((0 < m_SizePerGrid) && (m_SizePerGrid <= m_OnePageSize) && (m_OnePageSize < m_TotalPageSize))
	{
		unsigned int maxGridSize = m_TotalPageSize - m_OnePageSize;
		m_GridCount = maxGridSize / m_SizePerGrid;
		if ((maxGridSize % m_SizePerGrid) > 0)
		{
			++m_GridCount;
		}
	}

	_UpdateSlideTransform();
}

void MkScrollBarNode::SetWheelMovementCatchingArea(const MkFloat2& areaSize)
{
}

void MkScrollBarNode::Load(const MkDataNode& node)
{
	int barDir = 0;
	node.GetData(MkSceneNodeFamilyDefinition::ScrollBar::BarDirectionKey, barDir, 0);
	m_BarDirection = static_cast<eBarDirection>(barDir);

	// MkBaseWindowNode
	MkBaseWindowNode::Load(node);
}

void MkScrollBarNode::Save(MkDataNode& node)
{
	// 기존 템플릿이 없으면 템플릿 적용
	_ApplyBuildingTemplateToSave(node, MkSceneNodeFamilyDefinition::ScrollBar::TemplateName);

	node.SetData(MkSceneNodeFamilyDefinition::ScrollBar::BarDirectionKey, static_cast<int>(m_BarDirection), 0);
	
	// MkBaseWindowNode
	MkBaseWindowNode::Save(node);
}

MkScrollBarNode::MkScrollBarNode(const MkHashStr& name) : MkBaseWindowNode(name)
{
	m_BarDirection = eVertical;
	m_TotalPageSize = 0;
	m_OnePageSize = 0;
	m_SizePerGrid = 0;
	
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
	MK_CHECK(tNode != NULL, MkSceneNodeFamilyDefinition::ScrollBar::TemplateName.GetString() + L" template node alloc 실패")
		return;

	tNode->ApplyTemplate(MkSceneNodeFamilyDefinition::BaseWindow::TemplateName); // MkBaseWindowNode의 template 적용

	tNode->CreateUnit(MkSceneNodeFamilyDefinition::ScrollBar::BarDirectionKey, static_cast<int>(0));

	tNode->DeclareToTemplate(true);
}

void MkScrollBarNode::_UpdateSlideTransform(void)
{
	//m_BarDirection = eVertical;
	//m_TotalPageSize = 0;
	//m_OnePageSize = 0;
	//m_SizePerGrid = 0;
	
	//m_SlideNode = NULL;
	//m_MaxSlideSize = 0.f;
	//m_GridCount = 0;
	//m_CurrentGridPosition = 0;

	if (m_SlideNode != NULL)
	{
		// 크기
		float slideSize = m_MaxSlideSize;
		if (m_GridCount > 0) // m_GridCount가 1 이상이라는 말은 m_TotalPageSize 또한 1 이상임을 의미
		{
			float pageRate = GetMin<float>(static_cast<float>(m_OnePageSize) / static_cast<float>(m_TotalPageSize), 1.f); // maximum 1.f 제한
			slideSize *= pageRate;
		}
		m_SlideNode->SetPresetComponentSize(MkFloat2(slideSize, slideSize));

		// 위치
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

//------------------------------------------------------------------------------------------------//