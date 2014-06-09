
#include "MkCore_MkPathName.h"
#include "MkCore_MkHashStr.h"

//------------------------------------------------------------------------------------------------//

MkHashStr::MkHashStr(const MkHashStr& str) : m_Str(str.GetString()) { m_HashKey = str.GetHashKey(); }
MkHashStr::MkHashStr(const MkStr& str) : m_Str(str) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const MkPathName& str) : m_Str(str) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const wchar_t& character) : m_Str(character) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const wchar_t* wcharArray) : m_Str(wcharArray) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const std::wstring& str) : m_Str(str) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const bool& value) : m_Str(value) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const int& value) : m_Str(value) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const unsigned int& value) : m_Str(value) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const float& value) : m_Str(value) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const double& value) : m_Str(value) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const __int64& value) : m_Str(value) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const unsigned __int64& value) : m_Str(value) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const MkInt2& pt) : m_Str(pt) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const MkUInt2& pt) : m_Str(pt) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const MkVec2& value) : m_Str(value) { _GenerateHashKey(); }
MkHashStr::MkHashStr(const MkVec3& value) : m_Str(value) { _GenerateHashKey(); }

//------------------------------------------------------------------------------------------------//

void MkHashStr::_GenerateHashKey(void)
{
	unsigned int size = m_Str.GetSize();
	DWORD ch, result = 5381;
    for (unsigned int i=0; i<size; ++i)
    {
        ch = static_cast<DWORD>(m_Str[i]);
        result = ((result << 5) + result) + ch; // hash * 33 + ch
    }  
 
	m_HashKey = result;
}

//------------------------------------------------------------------------------------------------//
