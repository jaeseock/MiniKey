#pragma once

//------------------------------------------------------------------------------------------------//
// cmdLine 처리
// char(std::string) 기반
//
// * full str : 원본
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


class MkCmdLine
{
public:

	// 할당
	MkCmdLine& operator = (const char* cmdLine);
	MkCmdLine& operator = (const MkCmdLine& cmdLine);

	// 추가
	MkCmdLine& operator += (const char* cmdLine);
	MkCmdLine& operator += (const MkCmdLine& cmdLine);

	// 원본 참조
	inline const std::string& GetFullStr(void) const { return m_FullStr; }

	// 일반 라인 참조
	inline unsigned int GetNormalLineCount(void) const { return m_Lines.GetSize(); }
	inline const MkArray<std::string>& GetNormalLines(void) const { return m_Lines; }
	inline const std::string& GetNormalLine(unsigned int index) const { return m_Lines[index]; }

	// 일반 라인 추가
	inline void AddNormalLine(const std::string& line) { m_Lines.PushBack(line); }

	// 일반 라인 삭제
	inline void RemoveNormalLine(unsigned int index) { m_Lines.Erase(MkArraySection(index, 1)); }

	// key-value pair 참조
	inline unsigned int GetPairCount(void) const { return m_Pairs.GetSize(); }
	inline const MkMap<std::string, MkArray<std::string> >& GetPairs(void) const { return m_Pairs; }
	inline bool HasPair(const std::string& key) const { return m_Pairs.Exist(key); }
	inline unsigned int GetValueCount(const std::string& key) const { return m_Pairs.Exist(key) ? m_Pairs.GetSize() : 0; }
	inline const MkArray<std::string>& GetValues(const std::string& key) const { return m_Pairs[key]; }
	inline const std::string& GetValue(const std::string& key, unsigned int index) const { return m_Pairs[key][index]; }

	// key-value pair 추가
	void AddPair(const std::string& key, const std::string& value);

	// key-value pair 삭제
	bool RemovePair(const std::string& key);

	// 캐스팅
	inline operator const char*() const { return m_FullStr.empty() ? NULL : m_FullStr.c_str(); }
	inline operator const std::string&() const { return m_FullStr; }

	// 일반 라인, key-value pair로 원본 문자열 재구성
	void UpdateFullStr(void);

	// 초기화
	void Clear(void);

	MkCmdLine() {}
	MkCmdLine(const MkCmdLine& cmdLine) { *this = cmdLine; }
	~MkCmdLine() { Clear(); }

protected:

	void _Parse(const std::string& cmdStr);

protected:

	std::string m_FullStr;

	MkArray<std::string> m_Lines;
	MkMap<std::string, MkArray<std::string> > m_Pairs;

public:

	static const std::string Separator; // MKDEF_CMDLINE_SEPARATOR
	static const std::string Assigner; // MKDEF_CMDLINE_ASSIGNER
};
