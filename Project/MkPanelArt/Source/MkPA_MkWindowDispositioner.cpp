
#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkVisualPatternNode.h"
#include "MkPA_MkWindowDispositioner.h"


//------------------------------------------------------------------------------------------------//

unsigned int MkWindowDispositioner::AddNewLine(eLineHorizontalAlign horizontalAlign, eLineVerticalAlign verticalAlign)
{
	unsigned int currSize = m_Lines.GetSize();
	if (currSize == 0)
	{
		Clear();
	}

	_LineInfo& li = m_Lines.PushBack();
	li.rects.Reserve(64);
	li.horizontalAlign = horizontalAlign;
	li.verticalAlign = verticalAlign;

	return currSize;
}

bool MkWindowDispositioner::AddRectToCurrentLine(const MkHashStr& name, const MkFloat2& size)
{
	unsigned int lineIndex = m_Lines.GetSize();
	bool ok = ((lineIndex > 0) && (!m_RectTable.Exist(name)) && (!size.IsZero()));
	if (ok)
	{
		m_Lines[lineIndex - 1].rects.PushBack(name);
		m_RectTable.Create(name, MkFloatRect(MkFloat2::Zero, size));
	}
	return ok;
}

bool MkWindowDispositioner::AddDummyToCurrentLine(const MkFloat2& size)
{
	unsigned int lineIndex = m_Lines.GetSize();
	bool ok = ((lineIndex > 0) && (!size.IsZero()));
	if (ok)
	{
		_LineInfo& li = m_Lines[lineIndex - 1];
		
		// dummy name 생성
		MkStr buffer;
		buffer.Reserve(16);
		buffer += L"__D_";
		buffer += lineIndex;
		buffer += L"_";
		buffer += li.rects.GetSize();
		MkHashStr name(buffer);

		m_RectTable.Create(name, MkFloatRect(MkFloat2::Zero, size));
		li.rects.PushBack(name);
	}
	return ok;
}

void MkWindowDispositioner::Calculate(void)
{
	if (!m_Lines.Empty())
	{
		// line별 region 계산
		float maxLineWidth = 0.f; // line의 width 중 가장 큰 값
		float nextLineHeight = 0.f;
		
		MK_INDEXING_RLOOP(m_Lines, i) // window 좌표계는 down-top. 따라서 아래(마지막 line)에서 위(첫번째 line)로 진행
		{
			_LineInfo& li = m_Lines[i];
			if (!li.rects.Empty())
			{
				float nextRectPos = 0.f;

				MK_INDEXING_LOOP(li.rects, j) // rect의 크기와 offset이 반영된 line의 기본 영역 계산
				{
					MkFloatRect& rect = m_RectTable[li.rects[j]];
					li.region.size.x += ((j == 0) ? 0.f : m_Offset.x) + rect.size.x;
					li.region.size.y = GetMax<float>(li.region.size.y, rect.size.y); // rect의 height 중 가장 큰 값

					rect.position.x = nextRectPos; // rect의 local x position
					nextRectPos += m_Offset.x + rect.size.x;
				}

				maxLineWidth = GetMax<float>(maxLineWidth, li.region.size.x);
			}

			// line의 position
			li.region.position.y = nextLineHeight;

			nextLineHeight += li.region.size.y + m_Offset.y;
		}

		MK_INDEXING_LOOP(m_Lines, i)
		{
			_LineInfo& li = m_Lines[i];

			// line별 수평 정렬
			if ((li.region.size.x > 0.f) && (li.region.size.x < maxLineWidth))
			{
				switch (li.horizontalAlign)
				{
				case eLHA_Left: break;
				case eLHA_Center: li.region.position.x = (maxLineWidth - li.region.size.x) * 0.5f; break;
				case eLHA_Right: li.region.position.x = maxLineWidth - li.region.size.x; break;
				}
			}

			if (!li.rects.Empty())
			{
				MK_INDEXING_LOOP(li.rects, j)
				{
					MkFloatRect& rect = m_RectTable[li.rects[j]];

					// rect별 수직 정렬
					if (rect.size.y < li.region.size.y)
					{
						switch (li.verticalAlign)
						{
						case eLVA_Top: rect.position.y = li.region.size.y - rect.size.y; break;
						case eLVA_Center: rect.position.y = (li.region.size.y - rect.size.y) * 0.5f; break;
						case eLVA_Bottom: break;
						}
					}

					// margin과 line region의 position을 rect의 position에 반영
					rect.position += m_Margin;
					rect.position += li.region.position;
				}
			}
		}

		// 전체 영역 계산
		maxLineWidth;
		m_Region = m_Lines[0].region.position + MkFloat2(maxLineWidth, m_Lines[0].region.size.y);
		m_Region += m_Margin;
		m_Region += m_Margin;
	}
}

const MkFloatRect& MkWindowDispositioner::GetRect(const MkHashStr& name) const
{
	return m_RectTable.Exist(name) ? m_RectTable[name] : MkFloatRect::EMPTY;
}

bool MkWindowDispositioner::ApplyPositionToNode(MkVisualPatternNode* node) const
{
	bool ok = ((node != NULL) && m_RectTable.Exist(node->GetNodeName()));
	if (ok)
	{
		node->SetAlignmentPosition(eRAP_LeftBottom);
		node->SetAlignmentOffset(GetRect(node->GetNodeName()).position);
	}
	return ok;
}

void MkWindowDispositioner::Clear(void)
{
	m_RectTable.Clear();
	m_Lines.Clear();
	m_Region.Clear();
}

MkWindowDispositioner::MkWindowDispositioner()
{
	m_Margin = MkFloat2(MKDEF_PA_DEFAULT_WIN_FRAME_MARGIN, MKDEF_PA_DEFAULT_WIN_FRAME_MARGIN);
	m_Offset = MkFloat2(MKDEF_PA_DEFAULT_LENGTH_BETWEEN_WIN_COMPONENT_X, MKDEF_PA_DEFAULT_LENGTH_BETWEEN_WIN_COMPONENT_Y);
	
	m_Lines.Reserve(64);
}

//------------------------------------------------------------------------------------------------//