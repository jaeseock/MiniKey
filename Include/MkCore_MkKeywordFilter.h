#pragma once

//------------------------------------------------------------------------------------------------//
// global instance - keyword filter
//
// - slang은 해당 keyword를 대체어(MKDEF_SLANG_REPLACEMENT_TAG)로 치환해 반환
//   keyword는 영문자의 경우 대소문자 구분하지 않음
//   ex>
//		// 비속어 등록
//		MK_KEYWORD_FILTER.RegisterSlang(L"FUCK");
//		MK_KEYWORD_FILTER.RegisterSlang(L"Fucking");
//		...
//
//		// 비속어 검사
//		MkStr inputMsg = L"너 fuck 나는 fucking 아님";
//		MkStr dest;
//		if (MK_KEYWORD_FILTER.CheckSlang(inputMsg, dest)) // dest == L"너 **** 나는 ******* 아님"
//		{ ... }
//
// - tag는 해당 keyword를 설정된 tag로 치환해 반환
//   keyword는 영문자의 경우 대소문자 구분함
//   ex>
//		// tag 등록
//		MK_KEYWORD_FILTER.SetTag(L"_%MyName%_", L"미모와 기품");
//		...
//
//		// tag 적용
//		MkStr inputMsg = L"오레노 나마에와 _%MyName%_ 이도다";
//		MkStr dest;
//		MK_KEYWORD_FILTER.CheckTag(inputMsg, dest); // dest == L"오레노 나마에와 미모와 기품 이도다"
//
// 표준 단어를 대상으로 한 심플한 알고리즘. 보다 정확한 필터링이 필요하면 
// http://galab-work.cs.pusan.ac.kr/TRBoard/datafile/REP0903VULGAR.pdf 참조해 추가 할 것
//
// (NOTE) thread-safe하지 않음
//

//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkHashMap.h"

#define MK_KEYWORD_FILTER MkKeywordFilter::Instance()


class MkKeywordFilter : public MkSingletonPattern<MkKeywordFilter>
{
public:

	//------------------------------------------------------------------------------------------------//
	// 비속어
	//------------------------------------------------------------------------------------------------//

	// 키워드 등록
	void RegisterSlang(const MkStr& keyword);

	// src의 비속어 포함여부 반환
	bool CheckSlang(const MkStr& src) const;

	// src의 비속어 포함여부를 반환하고 MKDEF_SLANG_REPLACEMENT_TAG로 치환된 문자열을 dest에 할당
	bool CheckSlang(const MkStr& src, MkStr& dest) const;

	//------------------------------------------------------------------------------------------------//
	// tag
	//------------------------------------------------------------------------------------------------//

	// keyword에 해당하는 tag 설정
	void SetTag(const MkHashStr& keyword, const MkStr& tag);

	// src를 검사해 keyword에 해당하는 tag로 치환된 문자열을 dest에 할당
	void CheckTag(const MkStr& src, MkStr& dest) const;

	//------------------------------------------------------------------------------------------------//

	MkKeywordFilter() {}
	virtual ~MkKeywordFilter();

protected:

	bool _CheckString(const MkStr& src, MkStr& dest, bool slang) const;

protected:

	//------------------------------------------------------------------------------------------------//

	class CNode
	{
	public:

		void Clear(void);

		CNode() { m_EOK = false; }
		~CNode() { Clear(); }

		//----------------------------------------------------------------------------------//
		// 등록용
		//----------------------------------------------------------------------------------//

		// keyword문자열의 index위치에 있는 문자를 end of keyword 플래그와 함께 등록
		CNode* RegisterCharacter(const MkStr& keyword, unsigned int index, bool eok);

		// keyword문자열을 트리로 구축
		void RegisterKeyword(const MkStr& keyword);

		//----------------------------------------------------------------------------------//
		// 검색용
		//----------------------------------------------------------------------------------//

		// src의 index위치에 있는 문자를 검사해 자식으로 존재하지 않으면 NULL을, 존재하면 해당 노드를 반환 
		const CNode* CheckCharacter(const MkStr& src, unsigned int index) const;

		// src의 처음부터 keyword 포함 여부를 검사해 포함되었을 경우 크기를 반환
		// keyword가 없으면 0 반환(최초 하나만 검사)
		unsigned int FindKeyword(const MkStr& src, unsigned int beginPos) const;

	public:

		bool m_EOK; // end of keyword

		typedef MkHashMap<wchar_t, CNode*> ChildTableType;
		ChildTableType m_Children; // 자식 노드 테이블
	};

	//------------------------------------------------------------------------------------------------//

protected:

	// slang
	CNode m_SlangRootNode;

	// tag
	CNode m_TagRootNode;
	MkHashMap<MkHashStr, MkStr> m_TagMap;
};
