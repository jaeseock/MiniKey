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

#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"

class MkPathName;
class MkDataNode;


class MkTextNode
{
public:

	//------------------------------------------------------------------------------------------------//
	// 초기화, 할당, 해제
	// (NOTE) 노드 개별적인 attach/detach는 허용하지 않지만 공통적으로 parent 정보는 남아 있으므로
	//	SetUp()/Clear()를 통한 MkDataNode 기반의 추가/삭제는 가능
	//	이는 MkDataNode처럼 코드를 통한 자유로운 조작 대신 data를 통한 구성을 사용하도록 유도하기 위함
	//------------------------------------------------------------------------------------------------//

	// 파일로 초기화
	// (NOTE) parent 정보는 남아 있음
	bool SetUp(const MkPathName& filePath);

	// 노드로 초기화
	// (NOTE) parent 정보는 남아 있음
	void SetUp(const MkDataNode& node);

	// deep copy
	// (NOTE) source의 parent는 할당되지 않음
	MkTextNode& operator = (const MkTextNode& source);

	// 해제
	// (NOTE) parent 정보는 남아 있음
	void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// 출력
	//------------------------------------------------------------------------------------------------//

	void Export(MkDataNode& node) const;

	//------------------------------------------------------------------------------------------------//
	// 내부값 get/set
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
	// (NOTE) set은 허용하지 않음
	inline const MkArray<MkHashStr>& GetSequence(void) const { return m_Sequence; }

	// 문자열이 그려질 크기를 픽셀 단위로 반환
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

	// 직계 자식 노드 리스트를 반환
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
