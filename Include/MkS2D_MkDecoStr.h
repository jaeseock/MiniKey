#pragma once


//------------------------------------------------------------------------------------------------//
// deco string(font definition)
//
// <%LFS:(n)%>
// - ����� ���ΰ� ���ΰ��� ������ font face�� ���� ���������� �߰������� (n) pixel��ŭ offset ����
// - ���ڿ� �۷ι� ������ ��� ��ġ�� ����ǵ� ��� ������ ���������� ���� �� ���� ���������� ����
// - ���� ��� ������ 0���� ��ü ��
// ex> <%LFS:10%>, <%LFS:-5%>
//
// <%T:(font type)%>
// - ���� ������ ���ڿ��� ���� (font type) ����
// - �ش� font type�� �ݵ�� font manager�� ����Ǿ� �־�� ��
// - ���� ������ ���ڰ� fine feed(\n)�� �ƴϸ� �ڵ� ����(�ٸ� font type�� �� ���ο� ��ġ �� �� ����)
// - ���� ��� �ִ� �κ��� ������ �⺻ ��(DSF)���� ��ü ��
// ex> <%T:����20%>, <%T:����16%>
//
// <%S:(font state)%>
// - ���� ������ ���ڿ��� ���� (font state) ����
// - �ش� font state�� �ݵ�� font manager�� ����Ǿ� �־�� ��
// - ���� ��� �ִ� �κ��� ������ �⺻ ��(DSF)���� ��ü ��
// ex> <%S:�Ϲ��׸�%>, <%S:����%>
//
// (NOTE) �� �����̳� line feed ���� �����ڰ��� �ǹ� ���� ���� ����
//   ex> "abc  <%S:�����׸�%> def" == "abc<%S:�����׸�%> def"
//   ex> "abd  \n  def" == "abd\n  def"
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

#include "MkCore_MkMap.h"
#include "MkCore_MkHashStr.h"


class MkDecoStr
{
public:

	// tag ������ �Ϸ�� ���ڿ��� �ʱ�ȭ
	// ��� ����� ũ�� ������ ������ ���
	// �ϳ��� type, state, line feed size�� ���ڿ� �ʱ�ȭ
	void SetUp(const MkStr& source);

	// deep copy
	MkDecoStr& operator = (const MkDecoStr& decoStr);

	// ���ڿ��� �׷��� ũ�⸦ �ȼ� ������ ��ȯ
	inline const MkInt2& GetDrawingSize(void) const { return m_DrawingSize; }

	// SectorInfo ���� ���� ��ȯ
	inline bool Empty(void) const { return m_OutputList.Empty(); }

	// ����
	void Clear(void);

	MkDecoStr() {}
	MkDecoStr(const MkStr& source) { SetUp(source); }
	~MkDecoStr() { Clear(); }

	// �Ϲ� ���ڿ��� �⺻ ������ deco string���� ��ȯ
	static bool Convert(const MkHashStr& fontType, const MkHashStr& fontState, int lineFeedSize, const MkStr& msg, MkStr& buffer);

	// �Ϲ� ���ڿ��� DSF�� deco string���� ��ȯ
	static bool Convert(const MkStr& msg, MkStr& buffer);

public:

	typedef struct _SectorInfo
	{
		MkHashStr type;
		MkHashStr state;
		MkStr text;
		MkInt2 position;
	}
	SectorInfo;

	inline const MkArray<SectorInfo>& __GetOutputList(void) const { return m_OutputList; }

protected:

	void _SetSector(MkMap<unsigned int, SectorInfo>& sectorInfos, unsigned int position, const MkHashStr& type, const MkHashStr& state);

protected:

	MkArray<SectorInfo> m_OutputList;

	MkInt2 m_DrawingSize;

public:

	static const MkDecoStr Null;
};
