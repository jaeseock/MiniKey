#pragma once

//------------------------------------------------------------------------------------------------//
// cmdLine ó��
// char(std::string) ���
//
// * full str : ����
//
// �Ҵ�� ������ MKDEF_CMDLINE_SEPARATOR�� ������ �Ϲ� ����, key-value pair�� �и��� ����
// * �Ϲ� ���� : ��ȿ���ڰ� �����ϴ� ���ڿ��� ��/�� ������ ������ ����
// * key-value pair : key, value�� MKDEF_CMDLINE_ASSIGNER�� �и��Ǿ� �ִ� ��� key�� �����ϸ� pair�� �ν�
//                    �����ڴ� �������� ����
//
// ex> full str�� "aa a  ;  ; b cd = 4 5 6; == " �� ���,
//	* �Ϲ� ���� : 2��
//		- "aa a"
//		- "=="
//	* key-value pair : 1��
//		- key : "bcd", value : "456"
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"


class MkCmdLine
{
public:

	// �Ҵ�
	MkCmdLine& operator = (const char* cmdLine);
	MkCmdLine& operator = (const MkCmdLine& cmdLine);

	// �߰�
	MkCmdLine& operator += (const char* cmdLine);
	MkCmdLine& operator += (const MkCmdLine& cmdLine);

	// ���� ����
	inline const std::string& GetFullStr(void) const { return m_FullStr; }

	// �Ϲ� ���� ����
	inline unsigned int GetNormalLineCount(void) const { return m_Lines.GetSize(); }
	inline const MkArray<std::string>& GetNormalLines(void) const { return m_Lines; }
	inline const std::string& GetNormalLine(unsigned int index) const { return m_Lines[index]; }

	// �Ϲ� ���� �߰�
	inline void AddNormalLine(const std::string& line) { m_Lines.PushBack(line); }

	// �Ϲ� ���� ����
	inline void RemoveNormalLine(unsigned int index) { m_Lines.Erase(MkArraySection(index, 1)); }

	// key-value pair ����
	inline unsigned int GetPairCount(void) const { return m_Pairs.GetSize(); }
	inline const MkMap<std::string, MkArray<std::string> >& GetPairs(void) const { return m_Pairs; }
	inline bool HasPair(const std::string& key) const { return m_Pairs.Exist(key); }
	inline unsigned int GetValueCount(const std::string& key) const { return m_Pairs.Exist(key) ? m_Pairs.GetSize() : 0; }
	inline const MkArray<std::string>& GetValues(const std::string& key) const { return m_Pairs[key]; }
	inline const std::string& GetValue(const std::string& key, unsigned int index) const { return m_Pairs[key][index]; }

	// key-value pair �߰�
	void AddPair(const std::string& key, const std::string& value);

	// key-value pair ����
	bool RemovePair(const std::string& key);

	// ĳ����
	inline operator const char*() const { return m_FullStr.empty() ? NULL : m_FullStr.c_str(); }
	inline operator const std::string&() const { return m_FullStr; }

	// �Ϲ� ����, key-value pair�� ���� ���ڿ� �籸��
	void UpdateFullStr(void);

	// �ʱ�ȭ
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
