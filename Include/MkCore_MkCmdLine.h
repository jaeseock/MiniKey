#pragma once

//------------------------------------------------------------------------------------------------//
// cmdLine 처리
//
// 할당된 원본을 MKDEF_CMDLINE_SEPARATOR로 나누어 일반 라인, key-value pair로 분리해 저장
// * 일반 라인 : 유효문자가 존재하는 문자열의 앞/뒤 공백을 제거해 저장
// * key-value pair : key, value가 MKDEF_CMDLINE_ASSIGNER로 분리되어 있는 경우 key가 존재하면 pair로 인식
//                    공문자는 인정하지 않음
//
// ex> full str이 "aa a  ;  ; b cd = 4 5 6; == " 일 경우,
//	* 일반 라인 : 2개
//		- "aa a"
//		- "=="
//	* key-value pair : 1개
//		- key : "bcd", value : "456"
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkStr.h"


class MkCmdLine
{
public:

	// 어플리케이션 command line으로 초기화
	void SetUp(void);

	// 할당
	MkCmdLine& operator = (const MkStr& cmdLine);
	MkCmdLine& operator = (const MkCmdLine& cmdLine);

	// 추가
	MkCmdLine& operator += (const MkStr& cmdLine);

	// 일반 라인 참조
	inline unsigned int GetNormalLineCount(void) const { return m_Lines.GetSize(); }
	inline const MkArray<MkStr>& GetNormalLines(void) const { return m_Lines; }

	// 일반 라인 삭제
	inline void ClearNormalLines(void) { m_Lines.Clear(); }

	// key-value pair 참조
	inline unsigned int GetPairCount(void) const { return m_Pairs.GetSize(); }
	inline const MkMap<MkStr, MkArray<MkStr> >& GetPairs(void) const { return m_Pairs; }
	inline bool HasPair(const MkStr& key) const { return m_Pairs.Exist(key); }
	inline unsigned int GetValueCount(const MkStr& key) const { return m_Pairs.Exist(key) ? m_Pairs.GetSize() : 0; }
	inline const MkArray<MkStr>& GetValues(const MkStr& key) const { return m_Pairs[key]; }
	inline const MkStr& GetValue(const MkStr& key, unsigned int index) const { return m_Pairs[key][index]; }

	// key-value pair 추가
	void AddPair(const MkStr& key, const MkStr& value);

	// key-value pair 삭제
	bool RemovePair(const MkStr& key);

	// 일반 라인, key-value pair로 원본 문자열 재구성
	void GetFullStr(MkStr& buffer) const;

	// 초기화
	void Clear(void);

	MkCmdLine() {}
	MkCmdLine(const MkStr& cmdLine) { *this = cmdLine; }
	MkCmdLine(const MkCmdLine& cmdLine) { *this = cmdLine; }
	~MkCmdLine() { Clear(); }

protected:

	void _Parse(const MkStr& cmdLine);

protected:

	MkArray<MkStr> m_Lines;
	MkMap<MkStr, MkArray<MkStr> > m_Pairs;

public:

	static const MkStr Separator; // MKDEF_CMDLINE_SEPARATOR
	static const MkStr Assigner; // MKDEF_CMDLINE_ASSIGNER
};
