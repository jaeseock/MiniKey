#pragma once


//------------------------------------------------------------------------------------------------//
// MkTextNode
// - font type, style, 개행시 위치 설정을 가진 문자열 집합
// - MkDataNode 기반의 data를 통한 구성
// - 노드 구조를 가지며 key로 탐색하여 내부 값 변경 가능(반영을 위해서는 Build() 필요)
//
// * Build()
// - 노드 상태(수정 용이)를 그대로 그릴려고 하면 느리기 때문에 build를 통해 그리기 편한 형태로 변경
// - build의 결과물도 수정은 가능하지만 문제가 없도록 엄격히 해야 함(가급적 setter를 통한 수정 후 build를 권장)
// - Build시 가로 폭 제한 가능
//   (NOTE) 만약 주어진 제한이 단 한 글자도 들어가지 못할 정도로 작으면 빌드 실패함
//
// * MkDataNode 구성 문법
// - 설정 종류(keyword)
//		- font type(Type)
//		- font style(Style)
//		- 개행시 높이 위치 pixel offset(LFV)
//		- 개행시 수평 위치 pixel offset(LFH)
//		- 문자열(Text) : 배열일 경우 자동 개행으로 인식
//		(NOTE) 로딩되는 font type/style은 font manager에 등록된 상태이어야 함
// - 노드 내부의 설정은 해당 노드와 자식들에게 영향을 미침
// - 자식 노드에 해당 종류의 자체적인 설정이 없으면 부모 노드의 설정을 이어받음
// - 출력 순서는 해당 노드의 Text, Seq에 선언된 순서대로의 자식 노드 순(Seq가 없으면 사전 순서)
// - 노드 구조이므로 기본적으로 stack 형태이지만 필요하다면 나열 형식으로 사용 할 수는 있음
// ex>
//
//	str Type = "BigBold";
//	str Style = "Desc:Title";
//	int LFV = 10;
//	str Seq = "1st" / "2st";
//
//	Node "1st"
//	{
//		str Text = "[ 첫번째 제목입니당 ]";
//	
//		Node "Sub list"
//		{
//			str Type = "Medium";
//			str Style = "Desc:Normal";
//			int LFV = 3;
//			int LFH = 8;
//			str Seq = "일번이고" / "이번이구나";
//		
//			Node "일번이고"
//			{
//				str Text = "\n- 평범 항목 첫번째이고\n";
//			}
//		
//			Node "이번이구나"
//			{
//				str Text =
//					"- 평범 항목 두번째인데" /
//					"- 자동 개행의 유일한 방법";
//			}
//		}
//	}
//
//	Node "2st"
//	{
//		str Text = "\n[ 이거이 두번째 제목 ]";
//	
//		Node "0"
//		{
//			str Type = "Medium";
//			str Style = "Desc:Normal";
//			int LFV = 3;
//
//			Node "0"
//			{
//				str Text = "\n  - 내용 항목 첫번째";
//			}
//
//			Node "1"
//			{
//				Node "100"
//				{
//					str Text = "\n  - 내용 항목 두번째인데 ";
//				}
//
//				Node "200"
//				{
//					str Type = "Special";
//					str Style = "Desc:Highlight";
//					str Text = "<% 바꿔주시용 %>";
//				}
//			
//				Node "300"
//				{
//					str Text = " <- 바꾸고 강조   " / "  - 막줄\n";
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
	// (NOTE) source의 build 정보도 할당되지 않음. 따라서 복사된 객체를 사용하기 위해서는 별도로 Build()를 호출해 주어야 함
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
	// (NOTE) 변경이 발생했을 경우 Build()를 호출해 주어야 반영 됨
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
	// (NOTE) set은 허용하지 않음
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

	// 직계 자식 노드 리스트를 반환
	inline unsigned int GetChildNodeList(MkArray<MkHashStr>& childNodeList) const { return m_Children.GetKeyList(childNodeList); }

	//------------------------------------------------------------------------------------------------//
	// build
	//------------------------------------------------------------------------------------------------//

	// 가로 폭 제한 설정/반환
	// pixelWidth가 0보다 크면 해당 크기로 제한함(폭을 넘는 문자열은 개행됨)
	// (NOTE) Build() 호출 후 반영됨
	inline void SetWidthRestriction(int pixelWidth = 0) { m_WidthRestriction = pixelWidth; }
	inline int GetWidthRestriction(void) const { return m_WidthRestriction; }

	// 노드 정보를 토대로 렌더링에 최적화된 data를 생성
	// (NOTE) 비용이 대단히 큼
	void Build(void);

	// 텍스트가 차지하는 픽셀 영역 반환(build시 생성됨)
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

	// MkDataNode로부터 구성
	bool m_Visible;
	MkHashStr m_Type;
	MkHashStr m_Style;
	int m_LFV;
	int m_LFH;
	MkStr m_Text;
	MkArray<MkHashStr> m_Sequence;

	MkTextNode* m_ParentNode;
	MkHashMap<MkHashStr, MkTextNode*> m_Children;

	// build용
	int m_WidthRestriction;

	// 출력 data
	MkArray<_OutputData> m_OutputDataList;
	MkInt2 m_WholePixelSize;
};
