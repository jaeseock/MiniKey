#pragma once


//------------------------------------------------------------------------------------------------//
// deco string(font definition)
//
// <%LFS:(n)%>
// - 개행시 라인과 라인간의 간격은 font face에 의해 결정되지만 추가적으로 (n) pixel만큼 offset 지정
// - 문자열 글로벌 값으로 어느 위치에 선언되도 상관 없지만 마지막으로 선언 된 값이 최종적으로 사용됨
// - 만약 비어 있으면 0으로 대체 됨
// ex> <%LFS:10%>, <%LFS:-5%>
//
// <%T:(font type)%>
// - 선언 이후의 문자열에 대한 (font type) 지정
// - 해당 font type은 반드시 font manager에 선언되어 있어야 함
// - 선언 직전의 문자가 fine feed(\n)이 아니면 자동 개행(다른 font type이 한 라인에 위치 할 수 없음)
// - 만약 비어 있는 부분이 있으면 기본 값(DSF)으로 대체 됨
// ex> <%T:맑고20%>, <%T:굴림16%>
//
// <%S:(font state)%>
// - 선언 이후의 문자열에 대한 (font state) 지정
// - 해당 font state는 반드시 font manager에 선언되어 있어야 함
// - 만약 비어 있는 부분이 있으면 기본 값(DSF)으로 대체 됨
// ex> <%S:일반항목%>, <%S:제목%>
//
// (NOTE) 새 설정이나 line feed 이전 공문자같은 의미 없는 문자 무시
//   ex> "abc  <%S:강조항목%> def" == "abc<%S:강조항목%> def"
//   ex> "abd  \n  def" == "abd\n  def"
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

#include "MkCore_MkMap.h"
#include "MkCore_MkHashStr.h"


class MkDecoStr
{
public:

	// tag 설정이 완료된 문자열로 초기화
	// 계산 비용이 크기 때문에 신중히 사용
	// 하나의 type, state, line feed size로 문자열 초기화
	void SetUp(const MkStr& source);

	// deep copy
	MkDecoStr& operator = (const MkDecoStr& decoStr);

	// 문자열이 그려질 크기를 픽셀 단위로 반환
	inline const MkInt2& GetDrawingSize(void) const { return m_DrawingSize; }

	// SectorInfo 존재 여부 반환
	inline bool Empty(void) const { return m_OutputList.Empty(); }

	// 해제
	void Clear(void);

	MkDecoStr() {}
	MkDecoStr(const MkStr& source) { SetUp(source); }
	~MkDecoStr() { Clear(); }

	// 일반 문자열을 기본 설정의 deco string으로 변환
	static bool Convert(const MkHashStr& fontType, const MkHashStr& fontState, int lineFeedSize, const MkStr& msg, MkStr& buffer);

	// 일반 문자열을 DSF의 deco string으로 변환
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
