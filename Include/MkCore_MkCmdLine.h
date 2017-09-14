#pragma once

//------------------------------------------------------------------------------------------------//
// cmdLine ó��
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
#include "MkCore_MkStr.h"


class MkCmdLine
{
public:

	// ���ø����̼� command line���� �ʱ�ȭ
	void SetUp(void);

	// �Ҵ�
	MkCmdLine& operator = (const MkStr& cmdLine);
	MkCmdLine& operator = (const MkCmdLine& cmdLine);

	// �߰�
	MkCmdLine& operator += (const MkStr& cmdLine);

	// �Ϲ� ���� ����
	inline unsigned int GetNormalLineCount(void) const { return m_Lines.GetSize(); }
	inline const MkArray<MkStr>& GetNormalLines(void) const { return m_Lines; }

	// �Ϲ� ���� ����
	inline void ClearNormalLines(void) { m_Lines.Clear(); }

	// key-value pair ����
	inline unsigned int GetPairCount(void) const { return m_Pairs.GetSize(); }
	inline const MkMap<MkStr, MkArray<MkStr> >& GetPairs(void) const { return m_Pairs; }
	inline bool HasPair(const MkStr& key) const { return m_Pairs.Exist(key); }
	inline unsigned int GetValueCount(const MkStr& key) const { return m_Pairs.Exist(key) ? m_Pairs.GetSize() : 0; }
	inline const MkArray<MkStr>& GetValues(const MkStr& key) const { return m_Pairs[key]; }
	inline const MkStr& GetValue(const MkStr& key, unsigned int index) const { return m_Pairs[key][index]; }

	// key-value pair �߰�
	void AddPair(const MkStr& key, const MkStr& value);

	// key-value pair ����
	bool RemovePair(const MkStr& key);

	// �Ϲ� ����, key-value pair�� ���� ���ڿ� �籸��
	void GetFullStr(MkStr& buffer) const;

	// �ʱ�ȭ
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
