
#include "MkCore_MkCheck.h"
#include "MkCore_MkDeque.h"
#include "MkCore_MkDataNode.h"

//#include "MkCore_MkDevPanel.h"

#include "MkPA_MkFontManager.h"
#include "MkPA_MkTextNode.h"

const static MkHashStr KEY_VISIBLE(L"Visible");
const static MkHashStr KEY_TYPE(L"Type");
const static MkHashStr KEY_STYLE(L"Style");
const static MkHashStr KEY_LFV(L"LFV");
const static MkHashStr KEY_LFH(L"LFH");
const static MkHashStr KEY_TEXT(L"Text");
const static MkHashStr KEY_SEQUENCE(L"Seq");


// Build()�� �ӽ� ���� ����ũ��. �����Ͱ� �� ���� �Ѿ���� ��ɻ� �̻��� ������ ���� �ð��� ���� �ɸ��� ��
// ���� ������ ���ư��� �ӽ� �������̴� �޸𸮿� ������ ���� �ʴ� ������ �˳��� ��Ƶ� �� 
#define MKDEF_TEXTNODE_DEF_BLOCK_SIZE 512 // ��� ����ũ��. ����� type, style ������ ���ϴ� ����
#define MKDEF_TEXTNODE_DEF_LINE_SIZE 512 // ��ϴ� ���� ����ũ��
#define MKDEF_TEXTNODE_DEF_INC_SIZE_BY_RESTRICTION 64 // �� �������� ���� �����Ǹ��� ����Ǵ� ���� ���
#define MKDEF_TEXTNODE_DEF_DATA_COUNT_IN_A_LINE 256 // �� ���ο� ������ output data ����ũ��

//------------------------------------------------------------------------------------------------//

bool MkTextNode::SetUp(const MkPathName& filePath)
{
	MkDataNode node;
	bool ok = node.Load(filePath);
	if (ok)
	{
		SetUp(node);
		m_Children.Rehash();
	}
	return ok;
}

void MkTextNode::SetUp(const MkDataNode& node)
{
	Clear();

	// visible
	// �⺻���� true
	node.GetData(KEY_VISIBLE, m_Visible, 0);
	
	// font type
	if (node.GetDataEx(KEY_TYPE, m_Type, 0) && (!m_Type.Empty()))
	{
		MK_CHECK(MK_FONT_MGR.CheckAvailableFontType(m_Type), L"�̵�ϵ� " + m_Type.GetString() + L" font type���� MkTextNode �ʱ�ȭ �õ�")
		{
			m_Type.Clear();
		}

		if ((m_ParentNode != NULL) && (!m_Type.Empty()) && (m_ParentNode->GetFontType() == m_Type)) // �θ��� font type�� �����ϸ� ����
		{
			m_Type.Clear();
		}
	}

	// font style
	if (node.GetDataEx(KEY_STYLE, m_Style, 0) && (!m_Style.Empty()))
	{
		MK_CHECK(MK_FONT_MGR.CheckAvailableFontStyle(m_Style), L"�̵�ϵ� " + m_Style.GetString() + L" font style�� MkTextNode �ʱ�ȭ �õ�")
		{
			m_Style.Clear();
		}

		if ((m_ParentNode != NULL) && (!m_Style.Empty()) && (m_ParentNode->GetFontStyle() == m_Style)) // �θ��� font style�� �����ϸ� ����
		{
			m_Style.Clear();
		}
	}

	// linefeed/horizontal offset
	// �θ� ������ ��� m_LFV/m_LFH���� �̹� �θ�� ������ ���� �� �ִ� ����
	node.GetData(KEY_LFV, m_LFV, 0);
	node.GetData(KEY_LFH, m_LFH, 0);

	// text
	MkArray<MkStr> textBuf;
	if (node.GetData(KEY_TEXT, textBuf))
	{
		if (textBuf.GetSize() == 1)
		{
			m_Text = textBuf[0];
		}
		else // if (textBuf.GetSize() > 1)
		{
			unsigned int totalCnt = 0;
			MK_INDEXING_LOOP(textBuf, i)
			{
				totalCnt += textBuf[i].GetSize();
			}
			totalCnt += (textBuf.GetSize() - 1) * MkStr::LF.GetSize();
			m_Text.Reserve(totalCnt);

			MK_INDEXING_LOOP(textBuf, i)
			{
				if (i > 0)
				{
					m_Text += MkStr::LF; // �ڵ� ����
				}
				m_Text += textBuf[i];
			}
		}

		m_Text.RemoveKeyword(MkStr::TAB); // tab ����
		m_Text.RemoveKeyword(MkStr::CR); // carriage return ����
	}

	// sequence
	unsigned int childNodeCount = node.GetChildNodeCount();
	if (childNodeCount > 0)
	{
		if (childNodeCount == 1)
		{
			MkArray<MkHashStr> nodeNameList;
			node.GetChildNodeList(nodeNameList);
			m_Sequence.PushBack(nodeNameList[0]);
		}
		else if (childNodeCount > 1)
		{
			// sequence �б� �õ�
			if (node.GetDataEx(KEY_SEQUENCE, m_Sequence))
			{
				MK_CHECK(!m_Sequence.Empty(), node.GetNodeName().GetString() + L" text node�� sequence�� �����") {} // key�� �ִµ� ���� ����
			}

			// ������ ����
			if (m_Sequence.Empty())
			{
				MkArray<MkHashStr> nodeNameList;
				node.GetChildNodeList(nodeNameList);
				nodeNameList.SortInAscendingOrder();
				m_Sequence = nodeNameList;
			}
			// ��ȿ�� ����
			else
			{
				// sequence�� ���� �̸��� node�� �ڽ����� Ȯ���ϰ� ������ sequence���� ����
				MkArray<unsigned int> killList(m_Sequence.GetSize());
				MK_INDEXING_LOOP(m_Sequence, i)
				{
					MK_CHECK(node.ChildExist(m_Sequence[i]), node.GetNodeName().GetString() + L" text node�� �ڽ��� �ƴ� " + m_Sequence[i].GetString() + L" ��尡 sequence�� ����")
					{
						killList.PushBack(i);
					}
				}
				m_Sequence.Erase(killList);
			}
		}

		// children. m_Sequence�� �� �ִ� key���� ��� ��ȿ���� ���� ��(�ߺ� ���ɼ��� ����)
		MK_INDEXING_LOOP(m_Sequence, i)
		{
			const MkHashStr& currName = m_Sequence[i];
			if (!m_Children.Exist(currName))
			{
				MkTextNode* childNode = new MkTextNode(this);
				MK_CHECK(childNode != NULL, L"MkTextNode �Ҵ� ����")
					continue;

				m_Children.Create(currName, childNode);
				childNode->SetUp(*node.GetChildNode(currName));
			}
		}
	}
}

MkTextNode& MkTextNode::operator = (const MkTextNode& source)
{
	Clear();

	// visible
	SetVisible(source.GetVisible());

	// font type
	const MkHashStr& srcFT = source.GetFontType();
	if (GetFontType() != srcFT)
	{
		m_Type = srcFT;
	}

	// font style
	const MkHashStr& srcFS = source.GetFontStyle();
	if (GetFontStyle() != srcFS)
	{
		m_Style = srcFS;
	}

	// linefeed/horizontal offset
	if (m_LFV != source.GetLineFeedVerticalOffset())
	{
		SetLineFeedVerticalOffset(source.GetLineFeedVerticalOffset());
	}
	if (m_LFH != source.GetLineFeedHorizontalOffset())
	{
		SetLineFeedHorizontalOffset(source.GetLineFeedHorizontalOffset());
	}

	// text
	SetText(source.GetText());

	// sequence
	m_Sequence = source.GetSequence();
	
	// children
	MkArray<MkHashStr> nodeNameList;
	source.GetChildNodeList(nodeNameList);
	MK_INDEXING_LOOP(nodeNameList, i)
	{
		MkTextNode* childNode = new MkTextNode(this);
		MK_CHECK(childNode != NULL, L"MkTextNode �Ҵ� ����")
			continue;

		const MkHashStr& currName = nodeNameList[i];
		m_Children.Create(currName, childNode);
		*childNode = *source.GetChildNode(currName);
	}

	return *this;
}

void MkTextNode::Clear(void)
{
	_Init();

	MkHashMapLooper<MkHashStr, MkTextNode*> looper(m_Children);
	MK_STL_LOOP(looper)
	{
		delete looper.GetCurrentField();
	}
	m_Children.Clear();
	m_Sequence.Clear();

	m_WidthRestriction = 0;

	m_OutputDataList.Clear();
	m_WholePixelSize.Clear();
}

void MkTextNode::Export(MkDataNode& node) const
{
	node.Clear();

	// visible
	if (!m_Visible)
	{
		node.CreateUnit(KEY_VISIBLE, m_Visible);
	}

	// font type
	if (!m_Type.Empty())
	{
		node.CreateUnitEx(KEY_TYPE, m_Type);
	}

	// font style
	if (!m_Style.Empty())
	{
		node.CreateUnitEx(KEY_STYLE, m_Style);
	}

	// linefeed/horizontal offset
	if (m_LFV != ((m_ParentNode == NULL) ? 0 : m_ParentNode->GetLineFeedVerticalOffset()))
	{
		node.CreateUnit(KEY_LFV, m_LFV);
	}
	if (m_LFH != ((m_ParentNode == NULL) ? 0 : m_ParentNode->GetLineFeedHorizontalOffset()))
	{
		node.CreateUnit(KEY_LFH, m_LFH);
	}

	// text
	if (!m_Text.Empty())
	{
		node.CreateUnit(KEY_TEXT, m_Text);
	}

	// sequence
	if (m_Sequence.GetSize() > 1)
	{
		node.CreateUnitEx(KEY_SEQUENCE, m_Sequence);
	}

	// children
	if (!m_Children.Empty())
	{
		MkConstHashMapLooper<MkHashStr, MkTextNode*> looper(m_Children);
		MK_STL_LOOP(looper)
		{
			MkDataNode* childNode = node.CreateChildNode(looper.GetCurrentKey());
			MK_CHECK(childNode != NULL, L"MkDataNode �Ҵ� ����")
				continue;

			looper.GetCurrentField()->Export(*childNode);
		}
	}
}

bool MkTextNode::SetFontType(const MkHashStr& fontType)
{
	MK_CHECK(MK_FONT_MGR.CheckAvailableFontType(fontType), L"�̵�ϵ� " + fontType.GetString() + L" font type ���� �õ�")
		return false;

	m_Type = fontType;
	return true;
}

bool MkTextNode::SetFontStyle(const MkHashStr& fontStyle)
{
	MK_CHECK(MK_FONT_MGR.CheckAvailableFontStyle(fontStyle), L"�̵�ϵ� " + fontStyle.GetString() + L" font style ���� �õ�")
		return false;

	m_Style = fontStyle;
	return true;
}

bool MkTextNode::ChildExist(const MkArray<MkHashStr>& path) const
{
	const MkTextNode* targetNode = this;
	MK_INDEXING_LOOP(path, i)
	{
		targetNode = targetNode->GetChildNode(path[i]);
		if (targetNode == NULL)
			return false;
	}
	return true;
}

MkTextNode* MkTextNode::GetChildNode(const MkArray<MkHashStr>& path)
{
	MkTextNode* targetNode = this;
	MK_INDEXING_LOOP(path, i)
	{
		targetNode = targetNode->GetChildNode(path[i]);
		if (targetNode == NULL)
			return NULL;
	}
	return targetNode;
}

const MkTextNode* MkTextNode::GetChildNode(const MkArray<MkHashStr>& path) const
{
	const MkTextNode* targetNode = this;
	MK_INDEXING_LOOP(path, i)
	{
		targetNode = targetNode->GetChildNode(path[i]);
		if (targetNode == NULL)
			return NULL;
	}
	return targetNode;
}

void MkTextNode::Build(void)
{
	// ��� ��带 ��ȸ�ϸ� text block ����
	MkArray<_TextBlock> textBlocks(MKDEF_TEXTNODE_DEF_BLOCK_SIZE);
	__AddTextBlock(-1, -1, textBlocks);

	// line�� �������� �ʴ� ���ǹ��� block ����
	unsigned int expectedLineCount = 0;
	MkArray<unsigned int> killList(textBlocks.GetSize());
	MK_INDEXING_LOOP(textBlocks, i)
	{
		if (textBlocks[i].lines.Empty())
		{
			killList.PushBack(i);
		}
		else
		{
			expectedLineCount = textBlocks[i].lines.GetSize();
		}
	}
	textBlocks.Erase(killList);

	// output data ���� ũ�� ����
	if (m_WidthRestriction > 0) // �� ���� ����
	{
		// �����ϰ� ���������� ��� �� ���� ������ ������带 ���̱� ���� �޸𸮸� ���
		// ��¥�� �ӽ� �Ҵ��̰� ����ȭ�� �� ���̹Ƿ� ���ص� ��� ����
		expectedLineCount *= MKDEF_TEXTNODE_DEF_INC_SIZE_BY_RESTRICTION;
	}

	m_OutputDataList.Clear();
	m_OutputDataList.Reserve(expectedLineCount);
	m_WholePixelSize.Clear();

	// ������� block���� �������� �׷��� ��ġ�� ���
	MkInt2 position;
	int maxFontHeight = 0; // ���� ���� �ִ� ��Ʈ ũ��
	MkArray<_OutputData> lineData(MKDEF_TEXTNODE_DEF_DATA_COUNT_IN_A_LINE); // �� ���� ������ ��� �ӽ� ����

	MK_INDEXING_LOOP(textBlocks, i)
	{
		const _TextBlock& tb = textBlocks[i];

		const MkHashStr& typeName = MK_FONT_MGR.GetFontTypeName(tb.typeID);
		int fontHeight = MK_FONT_MGR.GetFontHeight(typeName);
		if (fontHeight > maxFontHeight)
		{
			maxFontHeight = fontHeight;
		}

		MK_INDEXING_LOOP(tb.lines, j)
		{
			MkDeque<_LineInfo> lineQueue; // ���� tb.lines[j]�� ���� �ʴ� ������ �� ���ѽ� �߰� _LineInfo�� ���� �� ���ɼ��� �ֱ� ����
			lineQueue.PushBack(tb.lines[j]);

			bool allTextPassed = false;
			while (!lineQueue.Empty())
			{
				const _LineInfo& li = lineQueue[0];

				if (li.lineFeed) // ���� �߻�
				{
					_ApplySingleLineOutputData(lineData, maxFontHeight);
					
					position.x = li.lfh;
					position.y += maxFontHeight + li.lfv;
					m_WholePixelSize.y = position.y;

					maxFontHeight = fontHeight;
				}

				if (!li.text.Empty())
				{
					MkStr outputText;
					position.x += li.shift;
					int lineWidth = position.x + MK_FONT_MGR.GetTextSize(typeName, li.text, false).x; // �� ���� ���� ��

					// �� ������ ���� ��� üũ
					if ((m_WidthRestriction > 0) && (lineWidth > m_WidthRestriction))
					{
						// ����� _LineInfo ����
						_LineInfo& lfInfo = lineQueue.PushBack();
						lfInfo.lineFeed = true;
						lfInfo.lfv = li.lfv;
						lfInfo.lfh = li.lfh;
						lfInfo.shift = 0;

						// �ؽ�Ʈ�� ������ �� ���ο� ��� ������ �κ��� _OutputData�� ����� �������� _LineInfo�� ������ �Ҵ�
						_LineInfo& nextInfo = lineQueue.PushBack();
						nextInfo.lineFeed = false;
						nextInfo.lfv = 0;
						nextInfo.lfh = 0;
						nextInfo.shift = 0;

						// �������� ��� �ؽ�Ʈ�� ���ԵǴ�(cutPos == li.text.GetSize()) ���� ����(lineWidth > m_WidthRestriction)
						unsigned int cutPos = MK_FONT_MGR.__FindSplitPosition(typeName, li.text, m_WidthRestriction - position.x);

						if (cutPos == 0) // ��� �ؽ�Ʈ�� ���� ������ �Ѿ� ��
						{
							// ���������� �ƹ��� �ؽ�Ʈ�� ���� ���ϸ� m_WidthRestriction ����ġ�� �۰� �����Ŷ� �Ǵ�
							MK_CHECK(!allTextPassed, L"�� ������ �ʹ� ����")
							{
								m_OutputDataList.Clear();
								m_WholePixelSize.Clear();
								return;
							}
							allTextPassed = true;

							nextInfo.text = li.text;
						}
						else // cutPos < li.text.GetSize(). �ؽ�Ʈ ����
						{
							li.text.GetSubStr(MkArraySection(0, cutPos), outputText);
							li.text.GetSubStr(MkArraySection(cutPos), nextInfo.text);
						}

						lineWidth = m_WidthRestriction;
					}
					else
					{
						outputText = li.text;
					}

					if (!outputText.Empty())
					{
						_OutputData& od = lineData.PushBack();
						od.fontHeight = fontHeight;
						od.position = position;
						od.typeID = tb.typeID;
						od.styleID = tb.styleID;
						od.text = outputText;

						position.x = lineWidth;
						if (position.x > m_WholePixelSize.x) // �ִ� ���� �� ����
						{
							m_WholePixelSize.x = position.x;
						}

						allTextPassed = false;
					}
				}

				lineQueue.PopFront();
			}
		}
	}

	_ApplySingleLineOutputData(lineData, maxFontHeight);

	m_WholePixelSize.y += maxFontHeight;
	m_OutputDataList.OptimizeMemory();
}

void MkTextNode::__AddTextBlock(int parentTypeID, int parentStyleID, MkArray<_TextBlock>& textBlocks) const
{
	if (!m_Visible)
		return;

	int typeID = m_Type.Empty() ?
		((parentTypeID < 0) ? MK_FONT_MGR.GetFontTypeID(MkFontManager::DefaultT) : parentTypeID) :
		MK_FONT_MGR.GetFontTypeID(m_Type);

	MK_CHECK(typeID >= 0, L"�̵�� font type�� " + m_Type.GetString() + L"�� ����� text node build �õ�")
		return;

	int styleID = m_Style.Empty() ?
		((parentStyleID < 0) ? MK_FONT_MGR.GetFontStyleID(MkFontManager::DefaultS) : parentStyleID) :
		MK_FONT_MGR.GetFontStyleID(m_Style);

	MK_CHECK(styleID >= 0, L"�̵�� font style�� " + m_Style.GetString() + L"�� ����� text node build �õ�")
		return;

	bool createNewBlock = true;
	if (!textBlocks.Empty())
	{
		const _TextBlock& tb = textBlocks[textBlocks.GetSize() - 1];
		createNewBlock = ((typeID != tb.typeID) || (styleID != tb.styleID));
	}

	if (createNewBlock)
	{
		_TextBlock& tb = textBlocks.PushBack();
		tb.typeID = typeID;
		tb.styleID = styleID;
		tb.lines.Reserve(MKDEF_TEXTNODE_DEF_LINE_SIZE);
	}

	if (!m_Text.Empty())
	{
		int spaceSize = MK_FONT_MGR.GetFontSpaceSize(MK_FONT_MGR.GetFontTypeName(typeID));
		MkArray<_LineInfo>& lines = textBlocks[textBlocks.GetSize() - 1].lines;

		MkArray<MkStr> tokens;
		m_Text.Tokenize(tokens, MkStr::LF, false);

		MK_INDEXING_LOOP(tokens, i)
		{
			_LineInfo li;
			li.lineFeed = (i != 0); // ù��° ������ ������ ����� �ƴ�
			li.lfv = GetLineFeedVerticalOffset();
			li.lfh = GetLineFeedHorizontalOffset();
			li.shift = 0;

			MkStr& currText = tokens[i];
			currText.RemoveKeyword(MkStr::TAB); // tab ����
			currText.RemoveKeyword(MkStr::CR); // carriage return ����

			if (!currText.Empty())
			{
				unsigned int validPos = currText.GetFirstValidPosition();
				if (validPos != MKDEF_ARRAY_ERROR) // ��� �� ��ȿ���ڰ� �����ؾ� ��
				{
					currText.GetSubStr(MkArraySection(validPos), li.text);
					li.shift = validPos * spaceSize;
				}
			}

			if (li.lineFeed || (!li.text.Empty())) // ���൵ ���µ� ���ڿ��� ������� ����
			{
				lines.PushBack(li);
			}
		}
	}

	if (!m_Sequence.Empty())
	{
		MK_INDEXING_LOOP(m_Sequence, i)
		{
			const MkHashStr& currChildName = m_Sequence[i];
			if (m_Children.Exist(currChildName))
			{
				m_Children[currChildName]->__AddTextBlock(typeID, styleID, textBlocks);
			}
		}
	}
}

void MkTextNode::__Draw(const MkInt2& position) const
{
	MK_INDEXING_LOOP(m_OutputDataList, i)
	{
		const MkTextNode::_OutputData& od = m_OutputDataList[i];
		const MkHashStr& fontType = MK_FONT_MGR.GetFontTypeName(od.typeID);
		const MkHashStr& fontStyle = MK_FONT_MGR.GetFontStyleName(od.styleID);

		MK_FONT_MGR.DrawMessage(position + od.position, fontType, fontStyle, od.text);
	}
}

MkTextNode::MkTextNode()
{
	m_ParentNode = NULL;
	_Init();
}

MkTextNode::MkTextNode(MkTextNode* parent)
{
	m_ParentNode = parent;
	_Init();
}

MkTextNode::MkTextNode(const MkTextNode& source)
{
	m_ParentNode = NULL;
	*this = source;
}

//------------------------------------------------------------------------------------------------//

void MkTextNode::_Init(void)
{
	SetVisible(true);
	m_Type.Clear();
	m_Style.Clear();
	SetLineFeedVerticalOffset((m_ParentNode == NULL) ? 0 : m_ParentNode->GetLineFeedVerticalOffset());
	SetLineFeedHorizontalOffset((m_ParentNode == NULL) ? 0 : m_ParentNode->GetLineFeedHorizontalOffset());
	m_Text.Clear();
}

void MkTextNode::_ApplySingleLineOutputData(MkArray<_OutputData>& lineData, int maxFontHeight)
{
	if (!lineData.Empty())
	{
		MK_INDEXING_LOOP(lineData, i)
		{
			_OutputData& od = lineData[i];
			od.position.y += (maxFontHeight - od.fontHeight) / 2;
		}

		m_OutputDataList += lineData;
		lineData.Flush();
	}
}

//------------------------------------------------------------------------------------------------//
