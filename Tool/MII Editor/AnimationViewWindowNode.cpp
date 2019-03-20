
#include "MkPA_MkBitmapPool.h"

#include "AppWindowCreationHelper.h"
#include "AnimationViewWindowNode.h"


//------------------------------------------------------------------------------------------------//

#define APPDEF_BORDER_SIZE MkFloat2(16.f, 16.f)

const static MkHashStr DEMO_SEQUENCE_KEY = L"__#DEMO";

void AnimationViewWindowNode::SetImagePath(const MkPathName& imagePath)
{
	m_Texture = MK_BITMAP_POOL.GetBitmapTexture(imagePath);
}

void AnimationViewWindowNode::UpdateSequence(const MkHashStr& key, const MkImageInfo::Sequence& sequence, MkFloat2& clientSize)
{
	if (m_Texture == NULL)
		return;

	if (key.Empty())
	{
		m_Texture->GetImageInfo().UpdateSequence(DEMO_SEQUENCE_KEY, sequence);
	}

	// 링크된 서브셋의 최대 크기 탐색
	MkFloat2 maxSubsetSize;
	MkConstMapLooper<double, MkHashStr> trackLooper(sequence.tracks);
	MK_STL_LOOP(trackLooper)
	{
		const MkImageInfo::Subset* subset = m_Texture->GetImageInfo().GetCurrentSubsetPtr(trackLooper.GetCurrentField());
		if (subset != NULL)
		{
			maxSubsetSize.CompareAndKeepMax(subset->rectSize);
		}
	}

	if (!maxSubsetSize.IsPositive())
		return;

	m_MaxSubsetSize = maxSubsetSize;
	MkFloat2 LB = APPDEF_BORDER_SIZE + MkFloat2(0.f, 20.f);
	MkFloat2 RT = LB + m_MaxSubsetSize;
	MkFloat2 panelPos = LB;

	// axis
	{
		MkLineShape& srLineY = CreateLine(L"AxisY");
		srLineY.SetColor(MkColor::Red);
		srLineY.SetLocalDepth(-1.f);

		switch (GetHorizontalRectAlignmentType(sequence.pivot))
		{
		case eRAT_Left:
			srLineY.AddLine(MkFloat2(LB.x, LB.y - 10.f), MkFloat2(LB.x, RT.y + 10.f));
			break;
		case eRAT_Middle:
			srLineY.AddLine(MkFloat2((LB.x + RT.x) / 2.f, LB.y - 10.f), MkFloat2((LB.x + RT.x) / 2.f, RT.y + 10.f));
			panelPos.x += m_MaxSubsetSize.x / 2.f;
			break;
		case eRAT_Right:
			srLineY.AddLine(MkFloat2(RT.x, LB.y - 10.f), MkFloat2(RT.x, RT.y + 10.f));
			panelPos.x += m_MaxSubsetSize.x;
			break;
		}

		MkLineShape& srLineX = CreateLine(L"AxisX");
		srLineX.SetColor(MkColor::Red);
		srLineX.SetLocalDepth(-1.f);

		switch (GetVerticalRectAlignmentType(sequence.pivot))
		{
		case eRAT_Top:
			srLineX.AddLine(MkFloat2(LB.x - 10.f, RT.y), MkFloat2(RT.x + 10.f, RT.y));
			panelPos.y += m_MaxSubsetSize.y;
			break;
		case eRAT_Center:
			srLineX.AddLine(MkFloat2(LB.x - 10.f, (LB.y + RT.y) / 2.f), MkFloat2(RT.x + 10.f, (LB.y + RT.y) / 2.f));
			panelPos.y += m_MaxSubsetSize.y / 2.f;
			break;
		case eRAT_Bottom:
			srLineX.AddLine(MkFloat2(LB.x - 10.f, LB.y), MkFloat2(RT.x + 10.f, LB.y));
			break;
		}
	}

	// region
	{
		MkLineShape& srLine = CreateLine(L"Guide");
		srLine.SetColor(MkColor::LightGray);
		srLine.SetLocalDepth(-3.f);

		MkArray<MkFloat2> lineVertices(5);
		lineVertices.PushBack(LB);
		lineVertices.PushBack(MkFloat2(LB.x, RT.y));
		lineVertices.PushBack(RT);
		lineVertices.PushBack(MkFloat2(RT.x, LB.y));
		lineVertices.PushBack(LB);
		srLine.AddLine(lineVertices);
	}

	// sequence
	{
		MkPanel& panel = PanelExist(L"SeqView") ? *GetPanel(L"SeqView") : CreatePanel(L"SeqView");
		panel.SetTexture(m_Texture->GetPoolKey(), key.Empty() ? DEMO_SEQUENCE_KEY : key);
		panel.SetLocalPosition(panelPos);
		panel.SetLocalDepth(-2.f);
	}

	// play time
	{
		MkLineShape& srLine = CreateLine(L"PlayRange");
		srLine.SetColor(MkColor::Yellow);
		srLine.SetLocalDepth(-1.f);
		srLine.AddLine(APPDEF_BORDER_SIZE, APPDEF_BORDER_SIZE + MkFloat2(m_MaxSubsetSize.x, 0.f));
	}

	clientSize = APPDEF_BORDER_SIZE * 2.f + m_MaxSubsetSize + MkFloat2(0.f, 20.f);
}

void AnimationViewWindowNode::SetSequenceData(const MkHashStr& key, const MkImageInfo::Sequence& sequence)
{
	if (m_Texture != NULL)
	{
		m_Texture->GetImageInfo().UpdateSequence(key, sequence);
		m_Texture->GetImageInfo().RemoveSequence(DEMO_SEQUENCE_KEY);
	}
}

void AnimationViewWindowNode::Update(double currTime)
{
	// play guide
	MkPanel* panel = GetPanel(L"SeqView");
	if (panel != NULL)
	{
		float progress = panel->GetSequenceProgress(currTime, true);
		MkFloat2 offset(progress * m_MaxSubsetSize.x, 0.f);
		offset += APPDEF_BORDER_SIZE;

		MkLineShape& srLine = CreateLine(L"PlayGuide");
		srLine.SetColor(MkColor::Yellow);
		srLine.SetLocalDepth(-1.f);
		srLine.AddLine(offset + MkFloat2(0.f, 5.f), offset + MkFloat2(0.f, -5.f));
	}

	MkWindowBaseNode::Update(currTime);
}

//------------------------------------------------------------------------------------------------//
