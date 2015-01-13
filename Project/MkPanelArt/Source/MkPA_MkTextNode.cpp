
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

//#include "MkCore_MkDevPanel.h"

#include "MkPA_MkFontManager.h"
#include "MkPA_MkTextNode.h"

const static MkHashStr KEY_VISIBLE(L"Visible");
const static MkHashStr KEY_TYPE(L"Type");
const static MkHashStr KEY_STYLE(L"Style");
const static MkHashStr KEY_LFOFFSET(L"LFOffset");
const static MkHashStr KEY_HOFFSET(L"HOffset");
const static MkHashStr KEY_TEXT(L"Text");
const static MkHashStr KEY_SEQUENCE(L"Seq");


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
	// �θ� ������ ��� m_LFOffset/m_HOffset���� �̹� �θ�� ������ ���� �� �ִ� ����
	node.GetData(KEY_LFOFFSET, m_LFOffset, 0);
	node.GetData(KEY_HOFFSET, m_HOffset, 0);

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
				if (!killList.Empty())
				{
					m_Sequence.Erase(killList);
				}
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
		SetFontType(srcFT);
	}

	// font style
	const MkHashStr& srcFS = source.GetFontStyle();
	if (GetFontStyle() != srcFS)
	{
		SetFontStyle(srcFS);
	}

	// linefeed/horizontal offset
	if (m_LFOffset != source.GetLineFeedOffset())
	{
		SetLineFeedOffset(source.GetLineFeedOffset());
	}
	if (m_HOffset != source.GetHorizontalOffset())
	{
		SetHorizontalOffset(source.GetHorizontalOffset());
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
	if (m_LFOffset != ((m_ParentNode == NULL) ? 0 : m_ParentNode->GetLineFeedOffset()))
	{
		node.CreateUnit(KEY_LFOFFSET, m_LFOffset);
	}
	if (m_HOffset != ((m_ParentNode == NULL) ? 0 : m_ParentNode->GetHorizontalOffset()))
	{
		node.CreateUnit(KEY_HOFFSET, m_HOffset);
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
	SetLineFeedOffset((m_ParentNode == NULL) ? 0 : m_ParentNode->GetLineFeedOffset());
	SetHorizontalOffset((m_ParentNode == NULL) ? 0 : m_ParentNode->GetHorizontalOffset());
	m_Text.Clear();
}

//------------------------------------------------------------------------------------------------//
