#pragma once


//------------------------------------------------------------------------------------------------//
// deco string(font definition)
//
// * ���� ��Ģ
// - ���ĺ��� ��� ��ҹ��� ����
// - ��(\t), ����(\r) ���� ����
// - �±� ���������� ������ ����
//   ex> <%T="���� 20"%>, <%T = "���� 20" %>, <%  T = "���� 20"  %> ���� ���� ��� ����
//
// * ����� ���
// <%DecoStr%>
// - deco str ������ ���ڿ��� �νĵǱ� ���ؼ��� �ش� ���ڿ��� ù �κп� �ݵ�� ����Ǿ� �־�� ��
//
// * font type ����
// <%T=(font type)%>
// - ���� ������ ���ڿ��� ���� font type ����
// - �ش� font type�� �ݵ�� font manager�� ����Ǿ� �־�� ��
// - �����Ǿ� ���� �ʰų� ������ ��� �ִ� ���ڿ��� ��� �⺻ ���� DSF�� ��ü ��
// ex> <%T="���� 20"%>, <%T="���� 16"%>, <%T="����� FT"%>
//
// * font state ����
// <%S=(font state)%>
// - ���� ������ ���ڿ��� ���� font state ����
// - �ش� font state�� �ݵ�� font manager�� ����Ǿ� �־�� ��
// - �����Ǿ� ���� �ʰų� ������ ��� �ִ� ���ڿ��� ��� �⺻ ���� DSF�� ��ü ��
// ex> <%S="�Ϲ��׸�"%>, <%S="ȸ�� �׸���"%>, <%S="����� FS"%>
//
// * ���� ���� ����
// <%LF=(offset)%>
// - ����� ���ΰ� ���ΰ��� ������ font face�� ���� ���������� ���� ������ ���ڿ��� ���� �߰� ���� ����
// - �����Ǿ� ���� �ʰų� ������ ��� �ִ� ���ڿ��� ��� �⺻ ���� 0���� ������
// ex> <%LF=0%>, <%LF=20%>, <%LF=-5%>
//
// - ����
//	<%LFS:5%><%T:����20%><%S:����%>ù��° �����Դϴ�.
//	<%T:����16%><%S:�Ϲ��׸�%>  - ���� �׸� ù��°
//	  - ���� �׸�<%S:�����׸�%> �̰��� �ٷ� ����
//
//	<%T:����20%><%S:����%>�ι�° �����Դϴ�.
//	<%T:����16%><%S:�Ϲ��׸�%>  - ���� �׸� ù��° �ǳ���
//	  - ���� �׸�<%S:�����׸�%> �ι�° ���� ����
//
//	<%T:����20%>new type <%T:����16%>test <%T:����20%>again
//
// - ���� 1
//	MkStr bufStr = L"...";
//	MkDecoStr decoStr(bufStr);
//	MK_FONT_MGR.DrawMessage(MkInt2(300, 100), decoStr);
//
// - ���� 2
//	MkStr bufStr;
//	MkDecoStr::Convert(L"����20", L"�Ϲ��׸�", 5, L"������\n �󸶹�\n\n  �����", bufStr);
//	MkDecoStr decoStr(bufStr);
//	MK_FONT_MGR.DrawMessage(MkInt2(300, 100), decoStr);
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
	inline void SetLineFeedOffset(int offset) { m_LFOffset = offset; }
	inline int GetLineFeedOffset(void) const { return m_LFOffset; }

	// horizontal offset
	inline void SetHorizontalOffset(int offset) { m_HOffset = offset; }
	inline int GetHorizontalOffset(void) const { return m_HOffset; }

	// text
	inline void SetText(const MkStr& text) { m_Text = text; }
	inline const MkStr& GetText(void) const { return m_Text; }

	// sequence
	// (NOTE) set�� ������� ����
	inline const MkArray<MkHashStr>& GetSequence(void) const { return m_Sequence; }

	// ���ڿ��� �׷��� ũ�⸦ �ȼ� ������ ��ȯ
	//inline const MkInt2& GetDrawingSize(void) const { return m_DrawingSize; }

	//------------------------------------------------------------------------------------------------//
	// get parent/child node
	//------------------------------------------------------------------------------------------------//

	// parent
	inline MkTextNode* GetParentNode(void) { return m_ParentNode; }
	inline const MkTextNode* GetParentNode(void) const { return m_ParentNode; }

	// child
	inline MkTextNode* GetChildNode(const MkHashStr& name) { return m_Children.Exist(name) ? m_Children[name] : NULL; }
	inline const MkTextNode* GetChildNode(const MkHashStr& name) const { return m_Children.Exist(name) ? m_Children[name] : NULL; }

	// ���� �ڽ� ��� ����Ʈ�� ��ȯ
	inline unsigned int GetChildNodeList(MkArray<MkHashStr>& childNodeList) const { return m_Children.GetKeyList(childNodeList); }

protected:

	//------------------------------------------------------------------------------------------------//
	// build
	//------------------------------------------------------------------------------------------------//

	typedef struct __LineInfo
	{
		bool lineFeed;
		int lfOffset;
		int hOffset;
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

public:

	void Build(int widthRestriction = 0);

	void __AddTextBlock(int parentTypeID, int parentStyleID, MkArray<_TextBlock>& textBlocks) const;

	//------------------------------------------------------------------------------------------------//

	MkTextNode();
	MkTextNode(MkTextNode* parent);
	MkTextNode(const MkTextNode& source);
	~MkTextNode() { Clear(); }

protected:

	void _Init(void);

protected:

	bool m_Visible;
	MkHashStr m_Type;
	MkHashStr m_Style;
	int m_LFOffset;
	int m_HOffset;
	MkStr m_Text;
	MkArray<MkHashStr> m_Sequence;

	MkTextNode* m_ParentNode;
	MkHashMap<MkHashStr, MkTextNode*> m_Children;

	//MkInt2 m_DrawingSize;
};
