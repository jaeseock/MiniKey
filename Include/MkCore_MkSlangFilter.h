#ifndef __MINIKEY_CORE_MKSLANGFILTER_H__
#define __MINIKEY_CORE_MKSLANGFILTER_H__

//------------------------------------------------------------------------------------------------//
// global instance - slang filter
//
// 비속어 필터
//
// thread-safe하지는 않지만 초기화시 일괄 등록(produce), 런타임중에는 사용(consume)만 하는
// 형태이기 때문에 문제는 없음
//
// 영문자의 경우 소문자로 자동 변환되어 검색(대문자 등록시에 소문자로 변환되어 저장)
//
// 표준 단어를 대상으로 한 심플한 알고리즘. 보다 정확한 비속어 필터링이 필요하면 
// http://galab-work.cs.pusan.ac.kr/TRBoard/datafile/REP0903VULGAR.pdf 참조해 추가 할 것
//
// ex>
//	// 키워드 등록
//	MK_SLANG_FILTER.RegisterKeyword(L"fuck");
//	MK_SLANG_FILTER.RegisterKeyword(L"fucking");
//	...
//
//	// 문자열 검사
//	MkStr inputMsg = L"너 fuck 나는 fucking 아님";
//	MkStr dest;
//	if (MK_SLANG_FILTER.CheckString(inputMsg, dest)) { ... }
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkHashMap.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkStr.h"

#define MK_SLANG_FILTER MkSlangFilter::Instance()


class MkSlangFilter : public MkSingletonPattern<MkSlangFilter>
{
public:

	// 비속어 키워드 등록
	void RegisterKeyword(const MkStr& keyword);

	// src의 비속어 포함여부 반환
	bool CheckString(const MkStr& src) const;

	// src의 비속어 포함여부를 반환하고 MKDEF_SLANG_REPLACEMENT_TAG로 치환된 문자열을 dest에 할당
	bool CheckString(const MkStr& src, MkStr& dest) const;

protected:

	//------------------------------------------------------------------------------------------------//

	class CNode
	{
	public:

		void Clear(void);

		CNode() { m_EOK = false; }
		~CNode() { Clear(); }

		//----------------------------------------------------------------------------------//
		// 비속어 등록용 메소드
		//----------------------------------------------------------------------------------//

		// keyword문자열의 index위치에 있는 문자를 end of keyword 플래그와 함께 등록
		CNode* RegisterCharacter(const MkStr& keyword, unsigned int index, bool eok);

		// keyword문자열을 트리로 구축
		void RegisterKeyword(const MkStr& keyword);

		//----------------------------------------------------------------------------------//
		// 비속어 검색용 메소드
		//----------------------------------------------------------------------------------//

		// keyword문자열의 index위치에 있는 문자를 검사해 자식으로 존재하지 않으면 NULL을,
		// 존재하면 해당 노드를 반환 
		const CNode* CheckCharacter(const MkStr& keyword, unsigned int index) const;

		// keyword문자열의 처음부터 비속어 포함 여부를 검사해 포함되었을 경우 크기를 반환
		// 비속어가 없으면 0 반환(최초 하나만 검사)
		unsigned int FindSlang(const MkStr& keyword, unsigned int beginPos) const;

		// src 문자열을 검사해 비속어가 검출될 경우 '*'로 치환해 dest로 변환
		// 반환값은 비속어 존재 여부
		bool CheckString(const MkStr& src, MkStr& dest) const;

	public:

		bool m_EOK; // end of keyword

		typedef MkHashMap<wchar_t, CNode*> ChildTableType;
		ChildTableType m_Children; // 자식 노드 테이블
	};

	//------------------------------------------------------------------------------------------------//

	void _ConvertTextToLower(MkStr& msg, MkArray<bool>& converting) const;
	void _ConvertTextToOriginal(MkStr& msg, const MkArray<bool>& converting) const;

public:

	MkSlangFilter() {}
	virtual ~MkSlangFilter() { m_RootNode.Clear(); }

protected:

	CNode m_RootNode;
};

//------------------------------------------------------------------------------------------------//

#endif
