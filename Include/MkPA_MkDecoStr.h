#pragma once


//------------------------------------------------------------------------------------------------//
// deco string(font definition)
//
// * 공통 규칙
// - 알파벳의 경우 대소문자 구별
// - 탭(\t), 리턴(\r) 문자 무시
// - 태그 구문에서의 공문자 무시
//   ex> <%T="맑고 20"%>, <%T = "맑고 20" %>, <%  T = "맑고 20"  %> 구문 셋은 모두 동일
//
// * 선언용 헤더
// <%DecoStr%>
// - deco str 문법의 문자열로 인식되기 위해서는 해당 문자열의 첫 부분에 반드시 선언되어 있어야 함
//
// * font type 선언
// <%T=(font type)%>
// - 선언 이후의 문자열에 대한 font type 지정
// - 해당 font type은 반드시 font manager에 선언되어 있어야 함
// - 지정되어 있지 않거나 설정이 비어 있는 문자열의 경우 기본 값인 DSF로 대체 됨
// ex> <%T="맑고 20"%>, <%T="굴림 16"%>, <%T="제목용 FT"%>
//
// * font state 선언
// <%S=(font state)%>
// - 선언 이후의 문자열에 대한 font state 지정
// - 해당 font state는 반드시 font manager에 선언되어 있어야 함
// - 지정되어 있지 않거나 설정이 비어 있는 문자열의 경우 기본 값인 DSF로 대체 됨
// ex> <%S="일반항목"%>, <%S="회색 그림자"%>, <%S="제목용 FS"%>
//
// * 개행 간격 선언
// <%LF=(offset)%>
// - 개행시 라인과 라인간의 간격은 font face에 의해 결정되지만 선언 이후의 문자열에 대한 추가 간격 지정
// - 지정되어 있지 않거나 설정이 비어 있는 문자열의 경우 기본 값인 0으로 지정됨
// ex> <%LF=0%>, <%LF=20%>, <%LF=-5%>
//
// - 샘플
//	<%LFS:5%><%T:맑고20%><%S:제목%>첫번째 제목입니당.
//	<%T:굴림16%><%S:일반항목%>  - 내용 항목 첫번째
//	  - 내용 항목<%S:강조항목%> 이거이 바로 강조
//
//	<%T:맑고20%><%S:제목%>두번째 제목입니당.
//	<%T:굴림16%><%S:일반항목%>  - 내용 항목 첫번째 또나옴
//	  - 내용 항목<%S:강조항목%> 두번째 강조 ㄱㄱ
//
//	<%T:맑고20%>new type <%T:남산16%>test <%T:맑고20%>again
//
// - 사용법 1
//	MkStr bufStr = L"...";
//	MkDecoStr decoStr(bufStr);
//	MK_FONT_MGR.DrawMessage(MkInt2(300, 100), decoStr);
//
// - 사용법 2
//	MkStr bufStr;
//	MkDecoStr::Convert(L"맑고20", L"일반항목", 5, L"가나다\n 라마바\n\n  사아자", bufStr);
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

	// tag 설정이 완료된 문자열로 초기화
	// 계산 비용이 크기 때문에 신중히 사용
	void SetUp(const MkStr& source);

	// deep copy
	MkDecoStr& operator = (const MkDecoStr& decoStr);

	// 문자열이 그려질 크기를 픽셀 단위로 반환
	//inline const MkInt2& GetDrawingSize(void) const { return m_DrawingSize; }

	// SectorInfo 존재 여부 반환
	//inline bool Empty(void) const { return m_OutputList.Empty(); }

	// 해제
	void Clear(void);

	MkDecoStr() {}
	MkDecoStr(const MkStr& source) { SetUp(source); }
	MkDecoStr(const MkDecoStr& source) { *this = source; }
	~MkDecoStr() { Clear(); }

	/*
	// 일반 문자열 앞에 font type tag 적용
	// ex> L"abc" + 굴림12 -> L"<%T:굴림12%>abc"
	static bool InsertFontTypeTag(const MkHashStr& fontType, const MkStr& msg, MkStr& buffer);

	// 일반 문자열 앞에 font state tag 적용
	// ex> L"abc" + 일반문자 -> L"<%S:일반문자%>abc"
	static bool InsertFontStateTag(const MkHashStr& fontState, const MkStr& msg, MkStr& buffer);

	// 일반 문자열을 단일 font type, state가 적용된 deco string으로 변환
	// ex> L"abc" + 굴림12, 일반문자, -5 -> L"<%LFS:-5%><%T:굴림12%><%S:일반문자%>abc"
	static bool Convert(const MkHashStr& fontType, const MkHashStr& fontState, int lineFeedSize, const MkStr& msg, MkStr& buffer);

	// 일반 문자열을 단일 font type, 복수의 font state가 적용된 deco string으로 변환
	// ex> L"abcde" + 굴림12, [일반문자/타이틀/특수], [0/1/4], -5 -> L"<%LFS:-5%><%T:굴림12%><%S:일반문자%>a<%S:타이틀%>bcd<%S:특수%>e"
	static bool Convert
		(const MkHashStr& fontType, const MkArray<MkHashStr>& fontState, const MkArray<unsigned int>& statePos, int lineFeedSize, const MkStr& msg, MkStr& buffer);

	// 일반 문자열을 DSF의 deco string으로 변환
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

	// header 바로 다음 위치를 반환. header가 없으면(deco str이 아니면) 0 반환
	unsigned int _GetDecoStrBodyPosition(const MkStr& source) const;

	// 이벤트의 유효성을 체크해 갱신 혹은 등록
	void _RegisterEffectiveEvent(const _SetEvent& evt);

	//void _SetSector(MkMap<unsigned int, SectorInfo>& sectorInfos, unsigned int position, const MkHashStr& type, const MkHashStr& state);

protected:

	MkStr m_Source;

	MkArray<_SetEvent> m_EventQueue; // multi-thread도 아니고 직접접근이 많으므로 MkEventQueuePattern를 사용하지 않음

	//MkArray<SectorInfo> m_OutputList;

	//MkInt2 m_DrawingSize;

public:

	static const MkDecoStr Null;
};
