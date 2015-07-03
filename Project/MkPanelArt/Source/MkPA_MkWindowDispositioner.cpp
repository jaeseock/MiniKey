
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
		
		// dummy name ����
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
		// line�� region ���
		float maxLineWidth = 0.f; // line�� width �� ���� ū ��
		float nextLineHeight = 0.f;
		
		MK_INDEXING_RLOOP(m_Lines, i) // window ��ǥ��� down-top. ���� �Ʒ�(������ line)���� ��(ù��° line)�� ����
		{
			_LineInfo& li = m_Lines[i];
			if (!li.rects.Empty())
			{
				float nextRectPos = 0.f;

				MK_INDEXING_LOOP(li.rects, j) // rect�� ũ��� offset�� �ݿ��� line�� �⺻ ���� ���
				{
					MkFloatRect& rect = m_RectTable[li.rects[j]];
					li.region.size.x += ((j == 0) ? 0.f : m_Offset.x) + rect.size.x;
					li.region.size.y = GetMax<float>(li.region.size.y, rect.size.y); // rect�� height �� ���� ū ��

					rect.position.x = nextRectPos; // rect�� local x position
					nextRectPos += m_Offset.x + rect.size.x;
				}

				maxLineWidth = GetMax<float>(maxLineWidth, li.region.size.x);
			}

			// line�� position
			li.region.position.y = nextLineHeight;

			nextLineHeight += li.region.size.y + m_Offset.y;
		}

		MK_INDEXING_LOOP(m_Lines, i)
		{
			_LineInfo& li = m_Lines[i];

			// line�� ���� ����
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

					// rect�� ���� ����
					if (rect.size.y < li.region.size.y)
					{
						switch (li.verticalAlign)
						{
						case eLVA_Top: rect.position.y = li.region.size.y - rect.size.y; break;
						case eLVA_Center: rect.position.y = (li.region.size.y - rect.size.y) * 0.5f; break;
						case eLVA_Bottom: break;
						}
					}

					// margin�� line region�� position�� rect�� position�� �ݿ�
					rect.position += m_Margin;
					rect.position += li.region.position;
				}
			}
		}

		// ��ü ���� ���
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