#ifndef __MINIKEY_CORE_MKHASHSTR_H__
#define __MINIKEY_CORE_MKHASHSTR_H__

//------------------------------------------------------------------------------------------------//
// Hash-key�� �����ϴ� MkStr�� ���̽��� �� �����ڵ� ��� ���ڿ�
// HashMap���� ���ڿ��� key�� ������ ���� �� ������ ���� �ڷ���
//
// MkMap<MkStr, ...> ������ red-black tree�� �������� ����� ������� ������(logN),
// VS���� �����ڵ��� ��(wmemcmp)�� �����Ƿ� ������ �ִٸ� �ִ��� ���̴� ���� ������
//
// MkStr�� ��ӹ����� �� �� ���� ��� ���Ἲ�� ���� ���ڿ� ���Žø��� hash-key�� ������ؾ� �ǹǷ� Ŭ�� ���� ����
// ���� ���� ������ ���� key ������ Ư���� ���ڿ��̶� �����ϰ� ������ �ڷ������� �и�
// - assign �ܿ� �ƹ��� ���ڿ� ��ȯ ����� ������� ����
// - ���ڿ� �񱳽� hash-key�� ���� �Ǵ�
// - ���� ���ڿ�(m_Str)�� ���� ������ ������ ���� ���� protected�� ����
//
// �����ӵ��� ���������� MkMap ��� MkHashMap �����̳ʸ� ���� ����� �� �ʿ伺 ����
//
// �˻���� : N(pair ����), M(DWORD(hash-key) �� ����� MkStr(std::wstring) �� ���)
// - MkMap<MkStr, ...>�� �˻���� : �� Ƚ��(logN) ��ŭ ���ڿ� ��(M)
//   -> O(logN * M)
// - MkMap<MkHashStr, ...>�� �˻���� : �� Ƚ��(logN) ��ŭ DWORD ��(1), �߰����� �ѹ��� ���ڿ� ��(M)
//   -> O(logN + M)
//
// (NOTE) hash-key�� ������ �����ϰ� ������ ������ ���ڿ� ������� �����ϴ� ���� �ƴ�
//	��, hash-key�� ��Ҵ� ���ڿ��� ��ҿ� �ƹ��� ����� ����
//	MkHashStr�� MkMap�� key�� ���� ��� ���ڿ��� �ƴ� �ؽ�Ű�� �������� ���ĵǰ� �ǹǷ�
//	MkStr�� key�� ����� �� ó�� ������ ������ �̷������ ����
//	��, MkArray<MkHashStr>::Sort...()�� �̿��ϸ� �Լ� Ư��ȭ�� ���� MkStró�� �������� ������ ������
//
// (NOTE) hash-key ���� ��뵵 ������ �����Ƿ� ��Ȱ���� ���� MkHashStr�� ��� ������ ��� �� ��
//
// http://www.gpgstudy.com/forum/viewtopic.php?t=795
//
//------------------------------------------------------------------------------------------------//

#include <atlcoll.h>
#include "MkCore_MkStr.h"


class MkHashStr
{
public:

	//------------------------------------------------------------------------------------------------//
	// ������
	//------------------------------------------------------------------------------------------------//

	MkHashStr(void);
	MkHashStr(const MkHashStr& str);
	MkHashStr(const MkStr& str);
	MkHashStr(const MkPathName& str);
	MkHashStr(const wchar_t& character);
	MkHashStr(const wchar_t* wcharArray);
	MkHashStr(const std::wstring& str);
	MkHashStr(const bool& value);
	MkHashStr(const int& value);
	MkHashStr(const unsigned int& value);
	MkHashStr(const float& value);
	MkHashStr(const double& value);
	MkHashStr(const __int64& value);
	MkHashStr(const unsigned __int64& value);
	MkHashStr(const MkInt2& pt);
	MkHashStr(const MkUInt2& pt);
	MkHashStr(const MkVec2& value);
	MkHashStr(const MkVec3& value);

	//------------------------------------------------------------------------------------------------//
	// ������
	//------------------------------------------------------------------------------------------------//

	inline bool operator == (const MkHashStr& str) const
	{
		return (m_HashKey == str.GetHashKey()) ? (m_Str == str.GetString()) : false;
	}

	inline bool operator != (const MkHashStr& str) const
	{
		return (m_HashKey == str.GetHashKey()) ? (m_Str != str.GetString()) : true;
	}

	inline bool operator < (const MkHashStr& str) const
	{
		return (m_HashKey == str.GetHashKey()) ? (m_Str < str.GetString()) : (m_HashKey < str.GetHashKey());
	}

	inline bool operator > (const MkHashStr& str) const
	{
		return (m_HashKey == str.GetHashKey()) ? (m_Str > str.GetString()) : (m_HashKey > str.GetHashKey());
	}

	inline bool operator <= (const MkHashStr& str) const
	{
		return (m_HashKey == str.GetHashKey()) ? (m_Str <= str.GetString()) : (m_HashKey <= str.GetHashKey());
	}

	inline bool operator >= (const MkHashStr& str) const
	{
		return (m_HashKey == str.GetHashKey()) ? (m_Str >= str.GetString()) : (m_HashKey >= str.GetHashKey());
	}

	inline operator const wchar_t*() const
	{
		return m_Str.GetPtr();
	}

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	inline void Clear(void) { m_Str.Clear(); _GenerateHashKey(); }

	inline bool Empty(void) const { return m_Str.Empty(); }

	inline unsigned int GetSize(void) const { return static_cast<unsigned int>(m_Str.GetSize()); }

	inline wchar_t* GetPtr(void) { return m_Str.GetPtr(); }
	inline const wchar_t* GetPtr(void) const { return m_Str.GetPtr(); }

	inline bool Equals(unsigned int position, const MkHashStr& str) const { return m_Str.Equals(position, str.GetString()); }

	inline const MkStr& GetString(void) const { return m_Str; }

	inline DWORD GetHashKey(void) const { return m_HashKey; }

	//------------------------------------------------------------------------------------------------//

protected:

	// m_Str�� �ؽ� Ű�� ������ m_HashKey�� ����
	void _GenerateHashKey(void);

protected:

	MkStr m_Str;
	DWORD m_HashKey;

public:

	// empty
	static const MkHashStr EMPTY;
	static const MkArray<MkHashStr> EMPTY_ARRAY;
};


//------------------------------------------------------------------------------------------------//
// MkHashMap�� key�� �Ǳ� ���� ����
//------------------------------------------------------------------------------------------------//

template<>
class CElementTraits<MkHashStr> :
	public CElementTraitsBase<MkHashStr>
{
public:
	static ULONG Hash(const MkHashStr& element)
	{
		return (ULONG)element.GetHashKey();
	}

	static bool CompareElements(const MkHashStr& element1, const MkHashStr& element2)
	{
		return (element1 == element2);
	}

	static int CompareElementsOrdered( const MkHashStr& element1, const MkHashStr& element2 )
	{
		if (element1 < element2)
		{
			return -1;
		}
		else if (element1 == element2)
		{
			return 0;
		}
		else
		{
			ATLASSERT( element1 > element2 );
			return 1;
		}
	}
};

//------------------------------------------------------------------------------------------------//
// MkArray<MkHashStr>::Sort...() �Լ� Ư��ȭ
//------------------------------------------------------------------------------------------------//

// �������� ����(ex> 10, 9, 8, ...)
template <> void MkArray<MkHashStr>::SortInDescendingOrder(void)
{
	unsigned int size = GetSize();
	MkArray<MkStr> strList;
	strList.Fill(size);
	for (unsigned int i=0; i<size; ++i)
	{
		strList[i] = m_Element[i];
	}
	strList.SortInDescendingOrder();
	for (unsigned int i=0; i<size; ++i)
	{
		m_Element[i] = strList[i];
	}
}

// �������� ����(ex> 1, 2, 3, ...)
template <> void MkArray<MkHashStr>::SortInAscendingOrder(void)
{
	unsigned int size = GetSize();
	MkArray<MkStr> strList;
	strList.Fill(size);
	for (unsigned int i=0; i<size; ++i)
	{
		strList[i] = m_Element[i];
	}
	strList.SortInAscendingOrder();
	for (unsigned int i=0; i<size; ++i)
	{
		m_Element[i] = strList[i];
	}
}

//------------------------------------------------------------------------------------------------//

#endif
