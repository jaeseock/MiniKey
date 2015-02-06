#pragma once


//------------------------------------------------------------------------------------------------//
// MkTextNode
// - font type, style, ����� ��ġ ������ ���� ���ڿ� ����
// - MkDataNode ����� data�� ���� ����
// - ��� ������ ������ key�� Ž���Ͽ� ���� �� ���� ����(�ݿ��� ���ؼ��� Build() �ʿ�)
//
// * Build()
// - ��� ����(���� ����)�� �״�� �׸����� �ϸ� ������ ������ build�� ���� �׸��� ���� ���·� ����
// - build�� ������� ������ ���������� ������ ������ ������ �ؾ� ��(������ setter�� ���� ���� �� build�� ����)
// - Build�� ���� �� ���� ����
//   (NOTE) ���� �־��� ������ �� �� ���ڵ� ���� ���� ������ ������ ���� ������
//
// * MkDataNode ���� ����
// - ���� ����(keyword)
//		- font type(Type)
//		- font style(Style)
//		- ����� ���� ��ġ pixel offset(LFV)
//		- ����� ���� ��ġ pixel offset(LFH)
//		- ���ڿ�(Text) : �迭�� ��� �ڵ� �������� �ν�
//		(NOTE) �ε��Ǵ� font type/style�� font manager�� ��ϵ� �����̾�� ��
// - ��� ������ ������ �ش� ���� �ڽĵ鿡�� ������ ��ħ
// - �ڽ� ��忡 �ش� ������ ��ü���� ������ ������ �θ� ����� ������ �̾����
// - ��� ������ �ش� ����� Text, Seq�� ����� ��������� �ڽ� ��� ��(Seq�� ������ ���� ����)
// - ��� �����̹Ƿ� �⺻������ stack ���������� �ʿ��ϴٸ� ���� �������� ��� �� ���� ����
// ex>
//
//	str Type = "BigBold";
//	str Style = "Desc:Title";
//	int LFV = 10;
//	str Seq = "1st" / "2st";
//
//	Node "1st"
//	{
//		str Text = "[ ù��° �����Դϴ� ]";
//	
//		Node "Sub list"
//		{
//			str Type = "Medium";
//			str Style = "Desc:Normal";
//			int LFV = 3;
//			int LFH = 8;
//			str Seq = "�Ϲ��̰�" / "�̹��̱���";
//		
//			Node "�Ϲ��̰�"
//			{
//				str Text = "\n- ��� �׸� ù��°�̰�\n";
//			}
//		
//			Node "�̹��̱���"
//			{
//				str Text =
//					"- ��� �׸� �ι�°�ε�" /
//					"- �ڵ� ������ ������ ���";
//			}
//		}
//	}
//
//	Node "2st"
//	{
//		str Text = "\n[ �̰��� �ι�° ���� ]";
//	
//		Node "0"
//		{
//			str Type = "Medium";
//			str Style = "Desc:Normal";
//			int LFV = 3;
//
//			Node "0"
//			{
//				str Text = "\n  - ���� �׸� ù��°";
//			}
//
//			Node "1"
//			{
//				Node "100"
//				{
//					str Text = "\n  - ���� �׸� �ι�°�ε� ";
//				}
//
//				Node "200"
//				{
//					str Type = "Special";
//					str Style = "Desc:Highlight";
//					str Text = "<% �ٲ��ֽÿ� %>";
//				}
//			
//				Node "300"
//				{
//					str Text = " <- �ٲٰ� ����   " / "  - ����\n";
//				}
//			}
//		}
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"


class MkPathName;
class MkDataNode;

class MkTextNode
{
public:

	//------------------------------------------------------------------------------------------------//
	// �ʱ�ȭ, �Ҵ�, ����
	// (NOTE) ��� �������� attach/detach�� ������� ������ ���������� parent ������ ���� �����Ƿ�
	//	SetUp()/Clear()�� ���� MkDataNode ����� �߰�/������ ����
	//	�̴� MkDataNodeó�� �ڵ带 ���� �����ο� ���� ��� data�� ���� ������ ����ϵ��� �����ϱ� ����
	//------------------------------------------------------------------------------------------------//

	// ���Ϸ� �ʱ�ȭ
	// (NOTE) parent ������ ���� ����
	bool SetUp(const MkPathName& filePath);

	// ���� �ʱ�ȭ
	// (NOTE) parent ������ ���� ����
	void SetUp(const MkDataNode& node);

	// deep copy
	// (NOTE) source�� parent�� �Ҵ���� ����
	// (NOTE) source�� build ������ �Ҵ���� ����. ���� ����� ��ü�� ����ϱ� ���ؼ��� ������ Build()�� ȣ���� �־�� ��
	MkTextNode& operator = (const MkTextNode& source);

	// ����
	// (NOTE) parent ������ ���� ����
	void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// ���
	//------------------------------------------------------------------------------------------------//

	void Export(MkDataNode& node) const;

	//------------------------------------------------------------------------------------------------//
	// ���ΰ� get/set
	// (NOTE) ������ �߻����� ��� Build()�� ȣ���� �־�� �ݿ� ��
	//------------------------------------------------------------------------------------------------//

	// visible
	inline void SetVisible(bool visible) { m_Visible = visible; }
	inline bool GetVisible(void) const { return m_Visible; }

	// type
	void SetFontType(const MkHashStr& fontType);
	inline const MkHashStr& GetFontType(void) const { return (m_Type.Empty() && (m_ParentNode != NULL)) ? m_ParentNode->GetFontType() : m_Type; }

	// style
	void SetFontStyle(const MkHashStr& fontStyle);
	inline const MkHashStr& GetFontStyle(void) const { return (m_Style.Empty() && (m_ParentNode != NULL)) ? m_ParentNode->GetFontStyle() : m_Style; }

	// line feed offset
	inline void SetLineFeedVerticalOffset(int offset) { m_LFV = offset; }
	inline int GetLineFeedVerticalOffset(void) const { return m_LFV; }

	// horizontal offset
	inline void SetLineFeedHorizontalOffset(int offset) { m_LFH = offset; }
	inline int GetLineFeedHorizontalOffset(void) const { return m_LFH; }

	// text
	inline void SetText(const MkStr& text) { m_Text = text; }
	inline const MkStr& GetText(void) const { return m_Text; }

	// sequence
	// (NOTE) set�� ������� ����
	inline const MkArray<MkHashStr>& GetSequence(void) const { return m_Sequence; }

	//------------------------------------------------------------------------------------------------//
	// get parent/child node
	//------------------------------------------------------------------------------------------------//

	// parent
	inline MkTextNode* GetParentNode(void) { return m_ParentNode; }
	inline const MkTextNode* GetParentNode(void) const { return m_ParentNode; }

	// child
	inline bool ChildExist(const MkHashStr& name) const { return m_Children.Exist(name); }
	inline MkTextNode* GetChildNode(const MkHashStr& name) { return ChildExist(name) ? m_Children[name] : NULL; }
	inline const MkTextNode* GetChildNode(const MkHashStr& name) const { return ChildExist(name) ? m_Children[name] : NULL; }

	// ���� �ڽ� ��� ����Ʈ�� ��ȯ
	inline unsigned int GetChildNodeList(MkArray<MkHashStr>& childNodeList) const { return m_Children.GetKeyList(childNodeList); }

	//------------------------------------------------------------------------------------------------//
	// build
	//------------------------------------------------------------------------------------------------//

	// ���� �� ���� ����/��ȯ
	// pixelWidth�� 0���� ũ�� �ش� ũ��� ������(���� �Ѵ� ���ڿ��� �����)
	// (NOTE) Build() ȣ�� �� �ݿ���
	inline void SetWidthRestriction(int pixelWidth = 0) { m_WidthRestriction = pixelWidth; }
	inline int GetWidthRestriction(void) const { return m_WidthRestriction; }

	// ��� ������ ���� �������� ����ȭ�� data�� ����
	// (NOTE) ����� ����� ŭ
	void Build(void);

	// �ؽ�Ʈ�� �����ϴ� �ȼ� ���� ��ȯ(build�� ������)
	inline const MkInt2& GetWholePixelSize(void) const { return m_WholePixelSize; }	

protected:

	typedef struct __LineInfo
	{
		bool lineFeed;
		int lfv;
		int lfh;
		MkStr text;
	}
	_LineInfo;

	typedef struct __TextBlock
	{
		int typeID;
		int styleID;
		MkArray<_LineInfo> lines;
	}
	_TextBlock;

	typedef struct __OutputData
	{
		int fontHeight;
		MkInt2 position;
		int typeID;
		int styleID;
		MkStr text;
	}
	_OutputData;

public:

	void __AddTextBlock(int parentTypeID, int parentStyleID, MkArray<_TextBlock>& textBlocks) const;
	void __Draw(const MkInt2& position = MkInt2(0, 0)) const;

	//------------------------------------------------------------------------------------------------//

	MkTextNode();
	MkTextNode(MkTextNode* parent);
	MkTextNode(const MkTextNode& source);
	~MkTextNode() { Clear(); }

protected:

	void _Init(void);
	void _ApplySingleLineOutputData(MkArray<_OutputData>& lineData, int maxFontHeight);

protected:

	// MkDataNode�κ��� ����
	bool m_Visible;
	MkHashStr m_Type;
	MkHashStr m_Style;
	int m_LFV;
	int m_LFH;
	MkStr m_Text;
	MkArray<MkHashStr> m_Sequence;

	MkTextNode* m_ParentNode;
	MkHashMap<MkHashStr, MkTextNode*> m_Children;

	// build��
	int m_WidthRestriction;

	// ��� data
	MkArray<_OutputData> m_OutputDataList;
	MkInt2 m_WholePixelSize;
};
