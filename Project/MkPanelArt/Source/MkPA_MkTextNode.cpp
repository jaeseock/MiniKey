
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


// Build()용 임시 버퍼 예약크기. 데이터가 이 양을 넘어가더라도 기능상 이상은 없지만 구성 시간이 오래 걸리게 됨
// 빌드 끝나면 날아가는 임시 데이터이니 메모리에 무리가 가지 않는 선에서 넉넉히 잡아도 됨 
#define MKDEF_TEXTNODE_DEF_BLOCK_SIZE 512 // 블록 예약크기. 블록은 type, style 설정이 변하는 구간
#define MKDEF_TEXTNODE_DEF_LINE_SIZE 512 // 블록당 라인 예약크기
#define MKDEF_TEXTNODE_DEF_INC_SIZE_BY_RESTRICTION 64 // 폭 제한으로 인해 증가되리라 예상되는 라인 배수
#define MKDEF_TEXTNODE_DEF_DATA_COUNT_IN_A_LINE 256 // 한 라인에 존재할 output data 예약크기

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
	// 기본값은 true
	node.GetData(KEY_VISIBLE, m_Visible, 0);
	
	// font type
	if (node.GetDataEx(KEY_TYPE, m_Type, 0) && (!m_Type.Empty()))
	{
		MK_CHECK(MK_FONT_MGR.CheckAvailableFontType(m_Type), L"미등록된 " + m_Type.GetString() + L" font type으로 MkTextNode 초기화 시도")
		{
			m_Type.Clear();
		}

		if ((m_ParentNode != NULL) && (!m_Type.Empty()) && (m_ParentNode->GetFontType() == m_Type)) // 부모의 font type과 동일하면 무시
		{
			m_Type.Clear();
		}
	}

	// font style
	if (node.GetDataEx(KEY_STYLE, m_Style, 0) && (!m_Style.Empty()))
	{
		MK_CHECK(MK_FONT_MGR.CheckAvailableFontStyle(m_Style), L"미등록된 " + m_Style.GetString() + L" font style로 MkTextNode 초기화 시도")
		{
			m_Style.Clear();
		}

		if ((m_ParentNode != NULL) && (!m_Style.Empty()) && (m_ParentNode->GetFontStyle() == m_Style)) // 부모의 font style과 동일하면 무시
		{
			m_Style.Clear();
		}
	}

	// linefeed/horizontal offset
	// 부모가 존재할 경우 m_LFV/m_LFH에는 이미 부모와 동일한 값이 들어가 있는 상태
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
					m_Text += MkStr::LF; // 자동 개행
				}
				m_Text += textBuf[i];
			}
		}

		m_Text.RemoveKeyword(MkStr::TAB); // tab 제거
		m_Text.RemoveKeyword(MkStr::CR); // carriage return 제거
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
			// sequence 읽기 시도
			if (node.GetDataEx(KEY_SEQUENCE, m_Sequence))
			{
				MK_CHECK(!m_Sequence.Empty(), node.GetNodeName().GetString() + L" text node의 sequence가 비었음") {} // key는 있는데 값이 없음
			}

			// 사전식 정렬
			if (m_Sequence.Empty())
			{
				MkArray<MkHashStr> nodeNameList;
				node.GetChildNodeList(nodeNameList);
				nodeNameList.SortInAscendingOrder();
				m_Sequence = nodeNameList;
			}
			// 유효성 점검
			else
			{
				// sequence에 속한 이름이 node의 자식인지 확인하고 없으면 sequence에서 삭제
				MkArray<unsigned int> killList(m_Sequence.GetSize());
				MK_INDEXING_LOOP(m_Sequence, i)
				{
					MK_CHECK(node.ChildExist(m_Sequence[i]), node.GetNodeName().GetString() + L" text node의 자식이 아닌 " + m_Sequence[i].GetString() + L" 노드가 sequence에 존재")
					{
						killList.PushBack(i);
					}
				}
				m_Sequence.Erase(killList);
			}
		}

		// children. m_Sequence에 들어가 있는 key들은 모두 유효함이 보장 됨(중복 가능성은 존재)
		MK_INDEXING_LOOP(m_Sequence, i)
		{
			const MkHashStr& currName = m_Sequence[i];
			if (!m_Children.Exist(currName))
			{
				MkTextNode* childNode = new MkTextNode(this);
				MK_CHECK(childNode != NULL, L"MkTextNode 할당 실패")
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
		MK_CHECK(childNode != NULL, L"MkTextNode 할당 실패")
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
			MK_CHECK(childNode != NULL, L"MkDataNode 할당 실패")
				continue;

			looper.GetCurrentField()->Export(*childNode);
		}
	}
}

bool MkTextNode::SetFontType(const MkHashStr& fontType)
{
	MK_CHECK(MK_FONT_MGR.CheckAvailableFontType(fontType), L"미등록된 " + fontType.GetString() + L" font type 지정 시도")
		return false;

	m_Type = fontType;
	return true;
}

bool MkTextNode::SetFontStyle(const MkHashStr& fontStyle)
{
	MK_CHECK(MK_FONT_MGR.CheckAvailableFontStyle(fontStyle), L"미등록된 " + fontStyle.GetString() + L" font style 지정 시도")
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
	// 모든 노드를 순회하며 text block 구성
	MkArray<_TextBlock> textBlocks(MKDEF_TEXTNODE_DEF_BLOCK_SIZE);
	__AddTextBlock(-1, -1, textBlocks);

	// line이 존재하지 않는 무의미한 block 제거
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

	// output data 비우고 크기 예약
	if (m_WidthRestriction > 0) // 폭 제한 여부
	{
		// 정밀하게 면적비율로 계산 할 수도 있지만 오버헤드를 줄이기 위해 메모리를 희생
		// 어짜피 임시 할당이고 최적화를 할 것이므로 과해도 상관 없음
		expectedLineCount *= MKDEF_TEXTNODE_DEF_INC_SIZE_BY_RESTRICTION;
	}

	m_OutputDataList.Clear();
	m_OutputDataList.Reserve(expectedLineCount);
	m_WholePixelSize.Clear();

	// 만들어진 block들을 바탕으로 그려질 위치를 계산
	MkInt2 position;
	int maxFontHeight = 0; // 현재 행의 최대 폰트 크기
	MkArray<_OutputData> lineData(MKDEF_TEXTNODE_DEF_DATA_COUNT_IN_A_LINE); // 한 행의 정보가 담길 임시 버퍼

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
			MkDeque<_LineInfo> lineQueue; // 직접 tb.lines[j]를 쓰지 않는 이유는 폭 제한시 추가 _LineInfo가 생성 될 가능성이 있기 때문
			lineQueue.PushBack(tb.lines[j]);

			bool allTextPassed = false;
			while (!lineQueue.Empty())
			{
				const _LineInfo& li = lineQueue[0];

				if (li.lineFeed) // 개행 발생
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
					int lineWidth = position.x + MK_FONT_MGR.GetTextSize(typeName, li.text, false).x; // 현 행의 가로 폭

					// 폭 제한이 있을 경우 체크
					if ((m_WidthRestriction > 0) && (lineWidth > m_WidthRestriction))
					{
						// 개행용 _LineInfo 생성
						_LineInfo& lfInfo = lineQueue.PushBack();
						lfInfo.lineFeed = true;
						lfInfo.lfv = li.lfv;
						lfInfo.lfh = li.lfh;
						lfInfo.shift = 0;

						// 텍스트를 분할해 현 라인에 출력 가능한 부분은 _OutputData에 남기고 나머지는 _LineInfo를 생성해 할당
						_LineInfo& nextInfo = lineQueue.PushBack();
						nextInfo.lineFeed = false;
						nextInfo.lfv = 0;
						nextInfo.lfh = 0;
						nextInfo.shift = 0;

						// 논리적으로 모든 텍스트가 포함되는(cutPos == li.text.GetSize()) 경우는 없음(lineWidth > m_WidthRestriction)
						unsigned int cutPos = MK_FONT_MGR.__FindSplitPosition(typeName, li.text, m_WidthRestriction - position.x);

						if (cutPos == 0) // 모든 텍스트가 다음 행으로 넘어 감
						{
							// 연속적으로 아무런 텍스트도 넣지 못하면 m_WidthRestriction 지나치게 작게 잡힌거라 판단
							MK_CHECK(!allTextPassed, L"폭 제한이 너무 작음")
							{
								m_OutputDataList.Clear();
								m_WholePixelSize.Clear();
								return;
							}
							allTextPassed = true;

							nextInfo.text = li.text;
						}
						else // cutPos < li.text.GetSize(). 텍스트 분할
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
						if (position.x > m_WholePixelSize.x) // 최대 가로 폭 갱신
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

	MK_CHECK(typeID >= 0, L"미등록 font type인 " + m_Type.GetString() + L"을 사용해 text node build 시도")
		return;

	int styleID = m_Style.Empty() ?
		((parentStyleID < 0) ? MK_FONT_MGR.GetFontStyleID(MkFontManager::DefaultS) : parentStyleID) :
		MK_FONT_MGR.GetFontStyleID(m_Style);

	MK_CHECK(styleID >= 0, L"미등록 font style인 " + m_Style.GetString() + L"을 사용해 text node build 시도")
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
			li.lineFeed = (i != 0); // 첫번째 라인은 개행의 대상이 아님
			li.lfv = GetLineFeedVerticalOffset();
			li.lfh = GetLineFeedHorizontalOffset();
			li.shift = 0;

			MkStr& currText = tokens[i];
			currText.RemoveKeyword(MkStr::TAB); // tab 제거
			currText.RemoveKeyword(MkStr::CR); // carriage return 제거

			if (!currText.Empty())
			{
				unsigned int validPos = currText.GetFirstValidPosition();
				if (validPos != MKDEF_ARRAY_ERROR) // 출력 될 유효문자가 존재해야 함
				{
					currText.GetSubStr(MkArraySection(validPos), li.text);
					li.shift = validPos * spaceSize;
				}
			}

			if (li.lineFeed || (!li.text.Empty())) // 개행도 없는데 문자열이 비었으면 무시
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
