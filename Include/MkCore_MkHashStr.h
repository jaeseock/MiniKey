#ifndef __MINIKEY_CORE_MKHASHSTR_H__
#define __MINIKEY_CORE_MKHASHSTR_H__

//------------------------------------------------------------------------------------------------//
// Hash-key를 생성하는 MkStr을 베이스로 한 유니코드 기반 문자열
// HashMap에서 문자열을 key로 삼을때 빠른 비교 연산을 위한 자료형
//
// MkMap<MkStr, ...> 참조도 red-black tree의 위엄으로 충분히 빠르기는 하지만(logN),
// VS에서 유니코드의 비교(wmemcmp)가 느리므로 여지가 있다면 최대한 줄이는 것이 유리함
//
// MkStr을 상속받으면 좀 더 편리한 대신 무결성을 위해 문자열 갱신시마다 hash-key를 재생성해야 되므로 클럭 낭비가 심함
// 따라서 잦은 수정이 없는 key 목적의 특수한 문자열이라 가정하고 별도의 자료형으로 분리
// - assign 외에 아무런 문자열 변환 기능을 허용하지 않음
// - 문자열 비교시 hash-key로 먼저 판단
// - 내부 문자열(m_Str)에 대한 임의적 접근을 막기 위해 protected로 선언
//
// 참조속도가 절대명제라면 MkMap 대신 MkHashMap 컨테이너를 적극 고려해 볼 필요성 있음
//
// 검색비용 : N(pair 갯수), M(DWORD(hash-key) 비교 비용대비 MkStr(std::wstring) 비교 비용)
// - MkMap<MkStr, ...>의 검색비용 : 비교 횟수(logN) 만큼 문자열 비교(M)
//   -> O(logN * M)
// - MkMap<MkHashStr, ...>의 검색비용 : 비교 횟수(logN) 만큼 DWORD 비교(1), 추가적인 한번의 문자열 비교(M)
//   -> O(logN + M)
//
// (NOTE) hash-key의 목적은 균질하게 흩어놓는 것이지 문자열 순서대로 정렬하는 것이 아님
//	즉, hash-key의 대소는 문자열의 대소와 아무런 상관이 없음
//	MkHashStr가 MkMap의 key로 사용될 경우 문자열이 아닌 해쉬키를 기준으로 정렬되게 되므로
//	MkStr를 key로 사용할 때 처럼 사전식 정렬이 이루어지지 않음
//	단, MkArray<MkHashStr>::Sort...()를 이용하면 함수 특수화를 통해 MkStr처럼 사전식의 정렬이 가능함
//
// (NOTE) hash-key 생성 비용도 작지는 않으므로 재활용이 잦은 MkHashStr의 경우 보존해 사용 할 것
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
	// 생성자
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
	// 연산자
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
	// 참조
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

	// m_Str로 해쉬 키를 생성해 m_HashKey에 저장
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
// MkHashMap의 key가 되기 위한 선언
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
// MkArray<MkHashStr>::Sort...() 함수 특수화
//------------------------------------------------------------------------------------------------//

// 내림차순 정렬(ex> 10, 9, 8, ...)
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

// 오름차순 정렬(ex> 1, 2, 3, ...)
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
