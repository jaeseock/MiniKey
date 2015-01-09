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

//#include "MkCore_MkMap.h"
#include "MkCore_MkArray.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkEventQueuePattern.h"


class MkDecoStr
{
public:

	// tag ������ �Ϸ�� ���ڿ��� �ʱ�ȭ
	// ��� ����� ũ�� ������ ������ ���
	void SetUp(const MkStr& source);

	// deep copy
	MkDecoStr& operator = (const MkDecoStr& decoStr);

	// ���ڿ��� �׷��� ũ�⸦ �ȼ� ������ ��ȯ
	//inline const MkInt2& GetDrawingSize(void) const { return m_DrawingSize; }

	// SectorInfo ���� ���� ��ȯ
	//inline bool Empty(void) const { return m_OutputList.Empty(); }

	// ����
	void Clear(void);

	MkDecoStr() {}
	MkDecoStr(const MkStr& source) { SetUp(source); }
	MkDecoStr(const MkDecoStr& source) { *this = source; }
	~MkDecoStr() { Clear(); }

	/*
	// �Ϲ� ���ڿ� �տ� font type tag ����
	// ex> L"abc" + ����12 -> L"<%T:����12%>abc"
	static bool InsertFontTypeTag(const MkHashStr& fontType, const MkStr& msg, MkStr& buffer);

	// �Ϲ� ���ڿ� �տ� font state tag ����
	// ex> L"abc" + �Ϲݹ��� -> L"<%S:�Ϲݹ���%>abc"
	static bool InsertFontStateTag(const MkHashStr& fontState, const MkStr& msg, MkStr& buffer);

	// �Ϲ� ���ڿ��� ���� font type, state�� ����� deco string���� ��ȯ
	// ex> L"abc" + ����12, �Ϲݹ���, -5 -> L"<%LFS:-5%><%T:����12%><%S:�Ϲݹ���%>abc"
	static bool Convert(const MkHashStr& fontType, const MkHashStr& fontState, int lineFeedSize, const MkStr& msg, MkStr& buffer);

	// �Ϲ� ���ڿ��� ���� font type, ������ font state�� ����� deco string���� ��ȯ
	// ex> L"abcde" + ����12, [�Ϲݹ���/Ÿ��Ʋ/Ư��], [0/1/4], -5 -> L"<%LFS:-5%><%T:����12%><%S:�Ϲݹ���%>a<%S:Ÿ��Ʋ%>bcd<%S:Ư��%>e"
	static bool Convert
		(const MkHashStr& fontType, const MkArray<MkHashStr>& fontState, const MkArray<unsigned int>& statePos, int lineFeedSize, const MkStr& msg, MkStr& buffer);

	// �Ϲ� ���ڿ��� DSF�� deco string���� ��ȯ
	static bool Convert(const MkStr& msg, MkStr& buffer);
	*/

public:

	/*
	typedef struct _SectorInfo
	{
		MkHashStr type;
		MkHashStr state;
		MkStr text;
		MkInt2 position;
	}
	SectorInfo;
	*/

	//inline const MkArray<SectorInfo>& __GetOutputList(void) const { return m_OutputList; }

protected:

	enum eEventType
	{
		eSetFontType = 0, // arg0 : name
		eSetFontState, // arg0 : name
		eSetLineFeedOffset // arg1 : offset
	};

	typedef MkEventUnitPack3<eEventType, MkHashStr, int, unsigned int> _SetEvent; // arg2 : position on source

	// header �ٷ� ���� ��ġ�� ��ȯ. header�� ������(deco str�� �ƴϸ�) 0 ��ȯ
	unsigned int _GetDecoStrBodyPosition(const MkStr& source) const;

	// �̺�Ʈ�� ��ȿ���� üũ�� ���� Ȥ�� ���
	void _RegisterEffectiveEvent(const _SetEvent& evt);

	//void _SetSector(MkMap<unsigned int, SectorInfo>& sectorInfos, unsigned int position, const MkHashStr& type, const MkHashStr& state);

protected:

	MkStr m_Source;

	MkArray<_SetEvent> m_EventQueue; // multi-thread�� �ƴϰ� ���������� �����Ƿ� MkEventQueuePattern�� ������� ����

	//MkArray<SectorInfo> m_OutputList;

	//MkInt2 m_DrawingSize;

public:

	static const MkDecoStr Null;
};
