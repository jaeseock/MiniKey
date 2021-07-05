
#include <assert.h>
#include <locale>
#include <fstream>
#include <time.h>
#include "MkCore_MkVec2.h"
#include "MkCore_MkVec3.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkStringOp.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkStr.h"


#define MKDEF_WCHAR_TAP L'\t'
#define MKDEF_WCHAR_LINEFEED L'\n'
#define MKDEF_WCHAR_RETURN L'\r'
#define MKDEF_WCHAR_SPACE L' '

// static value
static unsigned int gCurrentCodePage = CP_ACP;
static MkArray<wchar_t> gBlankTag;
static MkMap<unsigned int, unsigned int> gUnicodeBandwidth;

//------------------------------------------------------------------------------------------------//

void MkStr::SetUp(unsigned int codePage)
{
	// code page가 별도 설정되어 있지 않으면 OS 설정을 따름
	if (codePage == 0)
	{
		std::locale newLocale("", std::locale::ctype);
		std::locale::global(newLocale);

		MkStr localCodePage, localName;
		localCodePage.ImportMultiByteString(newLocale.name());
		unsigned int cpPos = localCodePage.GetFirstBlock(0, L".", localName);
		if (cpPos != MKDEF_ARRAY_ERROR)
		{
			MkStr cpBuf;
			localCodePage.GetSubStr(MkArraySection(cpPos), cpBuf);
			codePage = cpBuf.ToUnsignedInteger();
		}
		else
		{
			codePage = CP_ACP;
		}
	}

	SetGlobalCodePage(codePage);

	if (gBlankTag.Empty())
	{
		gBlankTag.Reserve(4);
		gBlankTag.PushBack(MKDEF_WCHAR_TAP);
		gBlankTag.PushBack(MKDEF_WCHAR_LINEFEED);
		gBlankTag.PushBack(MKDEF_WCHAR_RETURN);
		gBlankTag.PushBack(MKDEF_WCHAR_SPACE);
	}

	// https://namu.wiki/w/%EC%9C%A0%EB%8B%88%EC%BD%94%EB%93%9C#s-4.1
	// BMP만 대상으로 함
	if (gUnicodeBandwidth.Empty())
	{
		gUnicodeBandwidth.Create(0, 0); // ansi. 무시
		gUnicodeBandwidth.Create(0x80, 1252); // latin. 유럽권은 어지간하면 이거 하나로 커버 됨
		gUnicodeBandwidth.Create(0x250, 0); // 무시
		gUnicodeBandwidth.Create(0x370, 737); // Greek
		gUnicodeBandwidth.Create(0x400, 1251); // Cyrillic
		gUnicodeBandwidth.Create(0x530, 0); // 무시
		gUnicodeBandwidth.Create(0xE00, 874); // Thai
		gUnicodeBandwidth.Create(0xE80, 0); // 무시
		gUnicodeBandwidth.Create(0x3040, 932); // Japanese
		gUnicodeBandwidth.Create(0x3100, 0); // 무시
		gUnicodeBandwidth.Create(0x31F0, 932); // Japanese
		gUnicodeBandwidth.Create(0x3200, 0); // 무시
		gUnicodeBandwidth.Create(0x3400, 932); // Chinese. 한중일 통합 한자 확장 A
		gUnicodeBandwidth.Create(0x4DC0, 0); // 무시
		gUnicodeBandwidth.Create(0x4E00, 932); // Chinese. 한중일 통합 한자
		gUnicodeBandwidth.Create(0xA000, 0); // 무시
		gUnicodeBandwidth.Create(0xAC00, 949); // 완성형 한글(조합형은 무시)
		gUnicodeBandwidth.Create(0xD7B0, 0); // 무시
		gUnicodeBandwidth.Create(0xF900, 932); // Chinese. 한중일 통합 한자
		gUnicodeBandwidth.Create(0xFB00, 0); // 무시
		
	}
}

void MkStr::SetGlobalCodePage(unsigned int codePage) { gCurrentCodePage = codePage; }

unsigned int MkStr::GetGlobalCodePage(void) { return gCurrentCodePage; }

unsigned short GetPrimaryLanguageID(void) { return PRIMARYLANGID(::GetUserDefaultUILanguage()); }
unsigned short GetSubLanguageID(void) { return SUBLANGID(::GetUserDefaultUILanguage()); }

//------------------------------------------------------------------------------------------------//

MkStr::MkStr(void) { Flush(); }
MkStr::MkStr(const MkStr& str) { *this = str; }
MkStr::MkStr(const MkPathName& str) { m_Str = str.m_Str; }
MkStr::MkStr(const MkHashStr& str) { *this = str.GetString(); }
MkStr::MkStr(const wchar_t& character) { *this = character; }
MkStr::MkStr(const wchar_t* wcharArray) { *this = wcharArray; }
MkStr::MkStr(const std::wstring& str) { *this = str; }
MkStr::MkStr(const MkByteArray& str) { *this = str; }
MkStr::MkStr(const bool& value) { *this = value; }
MkStr::MkStr(const int& value) { *this = value; }
MkStr::MkStr(const unsigned int& value) { *this = value; }
MkStr::MkStr(const float& value) { *this = value; }
MkStr::MkStr(const double& value) { *this = value; }
MkStr::MkStr(const __int64& value) { *this = value; }
MkStr::MkStr(const unsigned __int64& value) { *this = value; }
MkStr::MkStr(const MkInt2& pt) { *this = pt; }
MkStr::MkStr(const MkUInt2& pt) { *this = pt; }
MkStr::MkStr(const MkVec2& value) { *this = value; }
MkStr::MkStr(const MkVec3& value) { *this = value; }

//------------------------------------------------------------------------------------------------//

MkStr& MkStr::operator = (const MkStr& str)
{
	m_Str.Flush();
	m_Str.PushBack(str.GetWholeBlockDescriptor());
	return *this;
}

MkStr& MkStr::operator = (const MkPathName& str)
{
	m_Str = str.m_Str;
	return *this;
}

MkStr& MkStr::operator = (const MkHashStr& str)
{
	*this = str.GetString();
	return *this;
}

MkStr& MkStr::operator = (const wchar_t& character)
{
	m_Str.Flush();
	m_Str.Reserve(m_Str.__GetBlockExpansionSizeInArray()); // 2글자 입력되므로 최소한 한 블록 확보
	m_Str.PushBack(character);
	m_Str.PushBack(NULL);
	return *this;
}

MkStr& MkStr::operator = (const wchar_t* wcharArray)
{
	if (wcharArray == NULL)
	{
		Flush();
	}
	else
	{
		unsigned int size = static_cast<unsigned int>(lstrlen(wcharArray));
		m_Str.Flush();
		m_Str.PushBack(MkMemoryBlockDescriptor<wchar_t>(wcharArray, size + 1)); // NULL 포함 복사
	}
	return *this;
}

MkStr& MkStr::operator = (const std::wstring& str)
{
	*this = str.c_str();
	return *this;
}

MkStr& MkStr::operator = (const MkByteArray& str)
{
	ReadTextStream(str, false);
	return *this;
}

MkStr& MkStr::operator = (const bool& value)
{
	*this = (value) ? L"true" : L"false";
	return *this;
}

MkStr& MkStr::operator = (const int& value)
{
	wchar_t tmp[20];
	_itow_s(value, tmp, 20, 10);
	*this = tmp;
	return *this;
}

MkStr& MkStr::operator = (const unsigned int& value)
{
	wchar_t tmp[20];
	unsigned long tv = static_cast<unsigned long>(value);
	_ultow_s(tv, tmp, 20, 10); // _uitow_s 가 없으므로 _ultow_s로 대치
	*this = tmp;
	return *this;
}

MkStr& MkStr::operator = (const float& value)
{
	wchar_t tmp[50];
	swprintf_s(tmp, 50, L"%.5f", value);

	int nullPos = 0;
	int dotPos = -1;
	for (int i=0; i<50; ++i)
	{
		const wchar_t& c = tmp[i];
		if (c == NULL)
		{
			nullPos = i;
			break;
		}
		else if (c == L'.')
		{
			dotPos = i;
		}
	}

	if ((nullPos > 0) && (dotPos > -1))
	{
		for (int i=nullPos-1; i>=dotPos; --i)
		{
			wchar_t& c = tmp[i];
			if ((c == L'0') || (c == L'.'))
			{
				c = NULL;
			}
			else
				break;
		}
	}

	*this = tmp;
	return *this;
}

MkStr& MkStr::operator = (const double& value)
{
	wchar_t tmp[320];
	swprintf_s(tmp, 320, L"%.5f", value);

	int nullPos = 0;
	int dotPos = -1;
	for (int i=0; i<320; ++i)
	{
		const wchar_t& c = tmp[i];
		if (c == NULL)
		{
			nullPos = i;
			break;
		}
		else if (c == L'.')
		{
			dotPos = i;
		}
	}

	if ((nullPos > 0) && (dotPos > -1))
	{
		for (int i=nullPos-1; i>=dotPos; --i)
		{
			wchar_t& c = tmp[i];
			if ((c == L'0') || (c == L'.'))
			{
				c = NULL;
			}
			else
				break;
		}
	}

	*this = tmp;
	return *this;
}

MkStr& MkStr::operator = (const __int64& value)
{
	wchar_t tmp[40];
	_i64tow_s(value, tmp, 40, 10);
	*this = tmp;
	return *this;
}

MkStr& MkStr::operator = (const unsigned __int64& value)
{
	wchar_t tmp[40];
	_ui64tow_s(value, tmp, 40, 10);
	*this = tmp;
	return *this;
}

MkStr& MkStr::operator = (const MkInt2& pt)
{
	wchar_t tmp[50];
	swprintf_s(tmp, 50, L"(%d, %d)", pt.x, pt.y);
	*this = tmp;
	return *this;
}

MkStr& MkStr::operator = (const MkUInt2& pt)
{
	wchar_t tmp[50];
	swprintf_s(tmp, 50, L"(%d, %d)", pt.x, pt.y);
	*this = tmp;
	return *this;
}

MkStr& MkStr::operator = (const MkVec2& value)
{
	MkStr buffer;
	buffer.Reserve(100);
	buffer += L"(";
	buffer += value.x;
	buffer += L", ";
	buffer += value.y;
	buffer += L")";
	*this = buffer;
	return *this;
}

MkStr& MkStr::operator = (const MkVec3& value)
{
	MkStr buffer;
	buffer.Reserve(150);
	buffer += L"(";
	buffer += value.x;
	buffer += L", ";
	buffer += value.y;
	buffer += L", ";
	buffer += value.z;
	buffer += L")";
	*this = buffer;
	return *this;
}

MkStr& MkStr::operator = (const MkByteArrayDescriptor& byteBlock)
{
	Flush();
	if (byteBlock.IsValid())
	{
		unsigned int charSize = byteBlock.GetSize() / sizeof(wchar_t);
		m_Str.Fill(charSize + 1);
		memcpy_s(reinterpret_cast<unsigned char*>(m_Str.GetPtr()), byteBlock.GetSize(), byteBlock, byteBlock.GetSize());
		m_Str[charSize] = NULL;
	}
	return *this;
}

bool MkStr::operator == (const MkStr& str) const { return (m_Str == str.m_Str); }
bool MkStr::operator == (const wchar_t* str) const { return (m_Str == MkStr(str).m_Str); }
bool MkStr::operator != (const MkStr& str) const { return (m_Str != str.m_Str); }
bool MkStr::operator != (const wchar_t* str) const { return (m_Str != MkStr(str).m_Str); }

bool MkStr::operator < (const MkStr& str) const
{
	unsigned int sourceSize = GetSize();
	unsigned int targetSize = str.GetSize();
	int comparison = wmemcmp(GetPtr(), str.GetPtr(), min(sourceSize, targetSize));
	if (comparison < 0) return true;
	else if (comparison > 0) return false;
	else return (sourceSize < targetSize);
}

bool MkStr::operator > (const MkStr& str) const
{
	unsigned int sourceSize = GetSize();
	unsigned int targetSize = str.GetSize();
	int comparison = wmemcmp(GetPtr(), str.GetPtr(), min(sourceSize, targetSize));
	if (comparison < 0) return false;
	else if (comparison > 0) return true;
	else return (sourceSize > targetSize);
}

bool MkStr::operator <= (const MkStr& str) const { return !(*this > str); }
bool MkStr::operator >= (const MkStr& str) const { return !(*this < str); }

MkStr MkStr::operator + (const MkStr& str) const
{
	MkStr tmp(*this);
	tmp += str;
	return tmp;
}

MkStr& MkStr::operator += (const MkStr& str)
{
	m_Str.PopBack(1);
	m_Str.PushBack(str.m_Str);
	return *this;
}

wchar_t& MkStr::operator [] (unsigned int offset)
{
	assert(offset < GetSize());
	return m_Str[offset];
}

const wchar_t& MkStr::operator [] (unsigned int offset) const
{
	assert(offset < GetSize());
	return m_Str[offset];
}

MkStr MkStr::operator ^ (const MkStr& keyStr) const
{
	MkStr tmp;
	unsigned int srcSize = GetSize();
	unsigned int keySize = keyStr.GetSize();
	if ((srcSize > 0) && (keySize > 0))
	{
		tmp.m_Str.Fill(srcSize + 1);
		unsigned int keyIndex = 0;
		for (unsigned int i=0; i<srcSize; ++i)
		{
			tmp.m_Str[i] = m_Str[i] ^ keyStr[keyIndex];

			++keyIndex;
			if (keyIndex >= keySize)
			{
				keyIndex = 0;
			}
		}
		tmp.m_Str[srcSize] = NULL;
	}
	return tmp;
}

MkStr& MkStr::operator ^= (const MkStr& keyStr)
{
	unsigned int srcSize = GetSize();
	unsigned int keySize = keyStr.GetSize();
	if ((srcSize > 0) && (keySize > 0))
	{
		unsigned int keyIndex = 0;
		for (unsigned int i=0; i<srcSize; ++i)
		{
			 m_Str[i] ^= keyStr[keyIndex];

			++keyIndex;
			if (keyIndex >= keySize)
			{
				keyIndex = 0;
			}
		}
	}
	return *this;
}

//------------------------------------------------------------------------------------------------//

void MkStr::Reserve(unsigned int size)
{
	m_Str.Reserve(size + 1);
}

void MkStr::OptimizeMemory(void)
{
	m_Str.OptimizeMemory();
}

void MkStr::Flush(void)
{
	m_Str.Flush();
	m_Str.PushBack(NULL);
}

void MkStr::Clear(void)
{
	m_Str.Clear();
	m_Str.PushBack(NULL);
}

//------------------------------------------------------------------------------------------------//

bool MkStr::Empty(void) const { return (m_Str.GetSize() == 1); }

unsigned int MkStr::GetSize(void) const { return (m_Str.GetSize() - 1); }

bool MkStr::IsDigit(void) const
{
	if (Empty())
		return false;

	unsigned int beginPosition = ((m_Str[0] == L'+') || (m_Str[0] == L'-')) ? 1 : 0;
	unsigned int size = GetSize();
	if ((beginPosition == 1) && (size == 1))
		return false;

	for (unsigned int i=beginPosition; i<size; ++i)
	{
		if ((m_Str[i] < L'0') || (m_Str[i] > L'9'))
			return false;
	}
	return true;
}

unsigned int MkStr::GetRepresentativeCodePage(void) const
{
	unsigned int codePage = 0;
	MK_INDEXING_LOOP(m_Str, i)
	{
		//m_Str[i];
	}
	return codePage;
}

const wchar_t& MkStr::GetAt(unsigned int offset) const
{
	assert(offset < GetSize());
	return m_Str[offset];
}

MkMemoryBlockDescriptor<wchar_t> MkStr::GetBodyBlockDescriptor(void) const
{
	return m_Str.GetMemoryBlockDescriptor(MkArraySection(0, m_Str.GetSize() - 1));
}

MkMemoryBlockDescriptor<wchar_t> MkStr::GetWholeBlockDescriptor(void) const
{
	return m_Str.GetMemoryBlockDescriptor();
}

MkMemoryBlockDescriptor<wchar_t> MkStr::GetMemoryBlockDescriptor(const MkArraySection& section) const
{
	return m_Str.GetMemoryBlockDescriptor(section);
}

//------------------------------------------------------------------------------------------------//

void MkStr::ImportMultiByteString(const std::string& str)
{
	std::wstring buffer;
	MkStringOp::ConvertString(str, buffer, gCurrentCodePage);
	*this = buffer.c_str();
}

void MkStr::ExportMultiByteString(std::string& str) const
{
	MkStringOp::ConvertString(std::wstring(m_Str.GetPtr()), str, gCurrentCodePage);
}

//------------------------------------------------------------------------------------------------//

void MkStr::ImportByteArray(const MkByteArray& buffer)
{
	if (buffer.Empty())
	{
		*this = L"NA";
	}
	else
	{
		m_Str.Clear();

		unsigned int fullSize = buffer.GetSize() * 2;
		m_Str.Fill(fullSize + 1);

		MK_INDEXING_LOOP(buffer, i)
		{
			const unsigned char& currByte = buffer[i];
			unsigned char upperPart = currByte >> 4;
			unsigned char lowerPart = currByte & 0xf;

			unsigned int target = i * 2;
			m_Str[target] = static_cast<wchar_t>((upperPart < 10) ? (upperPart + L'0') : (upperPart + L'7'));
			m_Str[target + 1] = static_cast<wchar_t>((lowerPart < 10) ? (lowerPart + L'0') : (lowerPart + L'7'));
		}

		m_Str[fullSize] = NULL;
	}
}

bool MkStr::ExportByteArray(MkByteArray& buffer) const
{
	if ((GetSize() % 2) != 0)
		return false;

	buffer.Clear();
	if ((GetSize() == 2) && (GetAt(0) == L'N') && (GetAt(0) == L'A'))
		return true;

	buffer.Fill(GetSize() / 2);

	MK_INDEXING_LOOP(buffer, i)
	{
		unsigned int target = i * 2;
		const wchar_t& frontChar = m_Str[target];
		const wchar_t& rearChar = m_Str[target + 1];

		buffer[i] =
			(static_cast<unsigned char>((frontChar < L'A') ? (frontChar - L'0') : (frontChar - L'7')) << 4) |
			static_cast<unsigned char>((rearChar < L'A') ? (rearChar - L'0') : (rearChar - L'7'));
	}
	return true;
}

//------------------------------------------------------------------------------------------------//

bool MkStr::Exist(const MkStr& keyword) const
{
	return m_Str.Exist(MkArraySection(0), keyword.GetBodyBlockDescriptor());
}

bool MkStr::Exist(const MkArraySection& section, const MkStr& keyword) const
{
	return m_Str.Exist(section, keyword.GetBodyBlockDescriptor());
}

bool MkStr::Equals(unsigned int position, const MkStr& keyword) const
{
	if (keyword.Empty())
		return Empty();

	return m_Str.Equals(position, keyword.GetBodyBlockDescriptor());
}

unsigned int MkStr::CountKeyword(const MkStr& keyword) const
{
	return CountKeyword(MkArraySection(0), keyword);
}

unsigned int MkStr::CountKeyword(const MkArraySection& section, const MkStr& keyword) const
{
	MkArray<unsigned int> positions;
	return CountKeyword(section, keyword, positions);
}

unsigned int MkStr::CountKeyword(const MkStr& keyword, MkArray<unsigned int>& positions) const
{
	return CountKeyword(MkArraySection(0), keyword, positions);
}

unsigned int MkStr::CountKeyword(const MkArraySection& section, const MkStr& keyword, MkArray<unsigned int>& positions) const
{
	if (keyword.Empty())
		return MKDEF_ARRAY_ERROR;

	MkArray< MkMemoryBlockDescriptor<wchar_t> > descList;
	descList.PushBack(keyword.GetBodyBlockDescriptor());
	return m_Str.FindAllInclusions(section, descList, positions);
}

unsigned int MkStr::CountWord(const MkStr& word) const
{
	if (Empty())
		return 0;

	if (word.Empty())
		return MKDEF_ARRAY_ERROR;

	MkArray<MkStr> tokens;
	MkStr buffer = *this;
	buffer.Tokenize(tokens);

	unsigned int count = 0;
	MK_INDEXING_LOOP(tokens, i)
	{
		if (tokens[i] == word)
		{
			++count;
		}
	}
	return count;
}

bool MkStr::CheckPrefix(const MkStr& prefix) const
{
	return Equals(0, prefix);
}

bool MkStr::CheckPostfix(const MkStr& postfix) const
{
	unsigned int srcSize = GetSize();
	unsigned int keySize = postfix.GetSize();
	if ((keySize == 0) || (srcSize < keySize))
		return false;

	return m_Str.Equals(srcSize - keySize, postfix.GetBodyBlockDescriptor());
}

unsigned int MkStr::GetFirstKeywordPosition(const MkStr& keyword) const
{
	return GetFirstKeywordPosition(MkArraySection(0), keyword);
}

unsigned int MkStr::GetFirstKeywordPosition(const MkArraySection& section, const MkStr& keyword) const
{
	MkArray< MkMemoryBlockDescriptor<wchar_t> > descList;
	descList.PushBack(keyword.GetBodyBlockDescriptor());
	return m_Str.FindFirstInclusion(section, descList);
}

unsigned int MkStr::GetFirstValidPosition(unsigned int position) const
{
	unsigned int count = GetSize();
	for (unsigned int i=position; i<count; ++i)
	{
		if (!_IsBlank(m_Str[i]))
			return i;
	}
	return MKDEF_ARRAY_ERROR;
}

unsigned int MkStr::GetFirstBlankPosition(unsigned int position) const
{
	return m_Str.FindFirstTagInclusion(MkArraySection(position), gBlankTag);
}

unsigned int MkStr::GetFirstLineFeedPosition(unsigned int position) const
{
	return m_Str.FindFirstInclusion(MkArraySection(position), MKDEF_WCHAR_LINEFEED);
}

unsigned int MkStr::GetLastValidPosition(unsigned int position) const
{
	for (unsigned int i=GetSize()-1; (i>=position) && m_Str.IsValidIndex(i); --i)
	{
		if (!_IsBlank(m_Str[i]))
			return i;
	}
	return MKDEF_ARRAY_ERROR;
}

unsigned int MkStr::CountBacksideBlank(unsigned int position) const
{
	unsigned int cnt = 0;
	for (unsigned int i=GetSize()-1; (i>=position) && m_Str.IsValidIndex(i); --i)
	{
		if (_IsBlank(m_Str[i]))
		{
			++cnt;
		}
		else
			break;
	}
	return cnt;
}

unsigned int MkStr::GetLineNumber(unsigned int position) const
{
	if (Empty())
		return 0;

	unsigned int count = 0;
	unsigned int lastIndex = GetSize() - 1;
	if (position > lastIndex)
	{
		position = lastIndex;
	}
	++position;
	for (unsigned int i=0; i<position; ++i)
	{
		if (m_Str[i] == '\n')
		{
			++count;
		}
	}
	return count;
}

//------------------------------------------------------------------------------------------------//

void MkStr::ReplaceKeyword(const MkStr& keywordFrom, const MkStr& keywordTo)
{
	ReplaceKeyword(MkArraySection(0), keywordFrom, keywordTo);
}

void MkStr::ReplaceKeyword(const MkArraySection& section, const MkStr& keywordFrom, const MkStr& keywordTo)
{
	if ((keywordFrom.GetSize() == 1) && (keywordTo.GetSize() == 1))
	{
		m_Str.ReplaceAll(section, keywordFrom.GetAt(0), keywordTo.GetAt(0));
	}
	else
	{
		m_Str.ReplaceAll(section, keywordFrom.GetBodyBlockDescriptor(), keywordTo.GetBodyBlockDescriptor());
	}
}

void MkStr::ReplaceCRLFtoTag(void)
{
	//ReplaceKeyword(MkStr::CRLF, L"\\r\\n");
	//ReplaceKeyword(MkStr::CR, L"\\r");
	//ReplaceKeyword(MkStr::LF, L"\\n");
	ReplaceKeyword(MkStr::CRLF, MkStr::TAG_CRLF);
	ReplaceKeyword(MkStr::CR, MkStr::TAG_CR);
	ReplaceKeyword(MkStr::LF, MkStr::TAG_LF);
}

void MkStr::ReplaceTagtoCRLF(void)
{
	//ReplaceKeyword(L"\\r\\n", MkStr::CRLF);
	//ReplaceKeyword(L"\\r", MkStr::CR);
	//ReplaceKeyword(L"\\n", MkStr::LF);
	ReplaceKeyword(MkStr::TAG_CRLF, MkStr::CRLF);
	ReplaceKeyword(MkStr::TAG_CR, MkStr::CR);
	ReplaceKeyword(MkStr::TAG_LF, MkStr::LF);
}

void MkStr::RemoveKeyword(const MkStr& keyword)
{
	m_Str.EraseAllInclusions(MkArraySection(0), keyword.GetBodyBlockDescriptor());
}

void MkStr::RemoveKeyword(const MkArraySection& section, const MkStr& keyword)
{
	m_Str.EraseAllInclusions(section, keyword.GetBodyBlockDescriptor());
}

void MkStr::RemoveBlank(void)
{
	m_Str.EraseAllTagInclusions(MkArraySection(0), gBlankTag);
}

void MkStr::RemoveBlank(const MkArraySection& section)
{
	m_Str.EraseAllTagInclusions(section, gBlankTag);
}

unsigned int MkStr::RemoveFrontSideBlank(void)
{
	unsigned int cnt = 0;
	if (!Empty())
	{
		unsigned int removeSize = GetFirstValidPosition(0);
		if (removeSize == MKDEF_ARRAY_ERROR)
		{
			cnt = GetSize();
			Flush(); // 유효문자 없음
		}
		else if (removeSize > 0)
		{
			cnt = removeSize;
			m_Str.Erase(MkArraySection(0, removeSize));
		}
	}
	return cnt;
}

unsigned int MkStr::RemoveRearSideBlank(void)
{
	unsigned int cnt = 0;
	if (!Empty())
	{
		unsigned int removePos = GetLastValidPosition(0);
		if (removePos == MKDEF_ARRAY_ERROR)
		{
			cnt = GetSize();
			Flush(); // 유효문자 없음
		}
		else if (removePos < (GetSize() - 1))
		{
			cnt = GetSize() - 1 - removePos;
			m_Str.Erase(MkArraySection(removePos + 1));
			m_Str.PushBack(NULL);
		}
	}
	return cnt;
}

//------------------------------------------------------------------------------------------------//

void MkStr::GetSubStr(const MkArraySection& section, MkStr& buffer) const
{
	if (section.GetSize() > 0)
	{
		m_Str.GetSubArray(section, buffer.m_Str);
		if (buffer.m_Str[buffer.GetSize()] != NULL) // 중간복사의 경우 NULL까지 복사가 이루어지지 않으므로 NULL 추가
		{
			buffer.m_Str.PushBack(NULL);
		}
	}
}

unsigned int MkStr::GetFirstWord(unsigned int position, MkStr& buffer) const
{
	unsigned int currsize = GetSize();
	if (position >= currsize)
		return MKDEF_ARRAY_ERROR;

	unsigned int beginPos = GetFirstValidPosition(position);
	if (beginPos == MKDEF_ARRAY_ERROR)
		return MKDEF_ARRAY_ERROR; // 유효문자 없음

	unsigned int endPos = GetFirstBlankPosition(beginPos + 1);
	if (endPos == MKDEF_ARRAY_ERROR)
	{
		endPos = currsize; // 유효문자로 종료
	}

	if (endPos > beginPos)
	{
		GetSubStr(MkArraySection(beginPos, endPos - beginPos), buffer);
	}
	return endPos;
}

unsigned int MkStr::GetFirstBlock(unsigned int position, const MkStr& endKeyword, MkStr& buffer) const
{
	if (position >= GetSize())
		return MKDEF_ARRAY_ERROR;

	unsigned int endPos = GetFirstKeywordPosition(MkArraySection(position), endKeyword);
	if (endPos == MKDEF_ARRAY_ERROR)
		return MKDEF_ARRAY_ERROR;

	unsigned int blockSize = endPos - position;
	unsigned int totalSize = blockSize + endKeyword.GetSize();
	if (blockSize > 0)
	{
		GetSubStr(MkArraySection(position, blockSize), buffer);
	}
	return (position + totalSize);
}

unsigned int MkStr::GetFirstBlock(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, MkStr& buffer) const
{
	if (position >= GetSize())
		return MKDEF_ARRAY_ERROR;

	unsigned int targetPosBegin = GetFirstKeywordPosition(MkArraySection(position), beginKeyword);
	if (targetPosBegin != MKDEF_ARRAY_ERROR)
	{
		unsigned int strBeginPos = targetPosBegin + beginKeyword.GetSize();
		unsigned int targetPosEnd = GetFirstKeywordPosition(MkArraySection(strBeginPos), endKeyword);
		if (targetPosEnd != MKDEF_ARRAY_ERROR)
		{
			if (targetPosEnd > strBeginPos)
			{
				GetSubStr(MkArraySection(strBeginPos, targetPosEnd - strBeginPos), buffer);
			}
			return (targetPosEnd + endKeyword.GetSize());
		}
	}
	return MKDEF_ARRAY_ERROR;
}

unsigned int MkStr::FindFirstStackPosition
(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, unsigned int& blockBegin, unsigned int& blockEnd) const
{
	if (position >= GetSize())
		return MKDEF_ARRAY_ERROR;

	unsigned int targetPosBegin = GetFirstKeywordPosition(MkArraySection(position), beginKeyword);
	if (targetPosBegin != MKDEF_ARRAY_ERROR)
	{
		// 내용 시작점
		unsigned int beginKeywordSize = beginKeyword.GetSize();
		unsigned int endKeywordSize = endKeyword.GetSize();
		unsigned int strBeginPos = targetPosBegin + beginKeywordSize;
		unsigned int seekPos = strBeginPos;
		unsigned int openerCount = 0;
		unsigned int closerCount = 0;

		while (true)
		{
			// 종료점 탐색
			seekPos = GetFirstKeywordPosition(MkArraySection(seekPos), endKeyword);
			if (seekPos != MKDEF_ARRAY_ERROR)
			{
				openerCount += CountKeyword(MkArraySection(strBeginPos, seekPos - strBeginPos), beginKeyword);

				// 블록 안의 begin/end keyword 수가 같아야 동일 깊이
				if (openerCount == closerCount)
				{
					blockBegin = targetPosBegin + beginKeywordSize;
					blockEnd = seekPos;
					return (seekPos + endKeywordSize);
				}

				seekPos += beginKeywordSize;
				strBeginPos = seekPos;
				++closerCount;
			}
			else
				break; // 종료 키워드가 없음
		}
	}
	return MKDEF_ARRAY_ERROR;
}

unsigned int MkStr::FindFirstStackPosition
(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, MkArraySection& section) const
{
	unsigned int blockBegin, blockEnd;
	unsigned int nextPos = FindFirstStackPosition(position, beginKeyword, endKeyword, blockBegin, blockEnd);
	if (nextPos != MKDEF_ARRAY_ERROR)
	{
		section.SetUp(blockBegin, blockEnd - blockBegin);
	}
	return nextPos;
}

unsigned int MkStr::GetFirstStackBlock(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, MkStr& buffer) const
{
	MkArraySection section;
	unsigned int nextPos = FindFirstStackPosition(position, beginKeyword, endKeyword, section);
	if (nextPos != MKDEF_ARRAY_ERROR)
	{
		GetSubStr(section, buffer);
	}
	return nextPos;
}

unsigned int MkStr::EraseFirstBlock(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword)
{
	if (position < GetSize())
	{
		unsigned int targetPosBegin = GetFirstKeywordPosition(MkArraySection(position), beginKeyword);
		if (targetPosBegin != MKDEF_ARRAY_ERROR)
		{
			unsigned int targetPosEnd = GetFirstKeywordPosition(MkArraySection(targetPosBegin + beginKeyword.GetSize()), endKeyword);
			if (targetPosEnd != MKDEF_ARRAY_ERROR)
			{
				m_Str.Erase(MkArraySection(targetPosBegin, targetPosEnd - targetPosBegin + endKeyword.GetSize()));
				return targetPosBegin;
			}
		}
	}
	return MKDEF_ARRAY_ERROR;
}

unsigned int MkStr::EraseFirstBlock(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, MkStr& buffer)
{
	if (position < GetSize())
	{
		unsigned int targetPosBegin = GetFirstKeywordPosition(MkArraySection(position), beginKeyword);
		if (targetPosBegin != MKDEF_ARRAY_ERROR)
		{
			unsigned int blockStartPosition = targetPosBegin + beginKeyword.GetSize();
			unsigned int targetPosEnd = GetFirstKeywordPosition(MkArraySection(targetPosBegin + beginKeyword.GetSize()), endKeyword);
			if (targetPosEnd != MKDEF_ARRAY_ERROR)
			{
				GetSubStr(MkArraySection(blockStartPosition, targetPosEnd - blockStartPosition), buffer);
				m_Str.Erase(MkArraySection(targetPosBegin, targetPosEnd - targetPosBegin + endKeyword.GetSize()));
				return targetPosBegin;
			}
		}
	}
	return MKDEF_ARRAY_ERROR;
}

void MkStr::EraseAllBlocks(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword)
{
	while (position != MKDEF_ARRAY_ERROR)
	{
		position = EraseFirstBlock(position, beginKeyword, endKeyword);
	}
}

unsigned int MkStr::ReplaceFirstBlock(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, const MkStr& replaceBlock)
{
	unsigned int beginPos = EraseFirstBlock(position, beginKeyword, endKeyword);
	if (beginPos != MKDEF_ARRAY_ERROR)
	{
		beginPos = Insert(beginPos, replaceBlock);
	}
	return beginPos;
}

unsigned int MkStr::ReplaceFirstBlock
(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, const MkStr& replaceBlock, MkStr& buffer)
{
	unsigned int beginPos = EraseFirstBlock(position, beginKeyword, endKeyword, buffer);
	if (beginPos != MKDEF_ARRAY_ERROR)
	{
		beginPos = Insert(beginPos, replaceBlock);
	}
	return beginPos;
}

void MkStr::ReplaceAllBlocks(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, const MkStr& replaceBlock)
{
	while (position != MKDEF_ARRAY_ERROR)
	{
		position = ReplaceFirstBlock(position, beginKeyword, endKeyword, replaceBlock);
	}
}

void MkStr::RemoveAllComment(void)
{
	EraseAllBlocks(0, L"/*", L"*/");

	// protocol 예외시 L"//" 처리. ex> https://...
	unsigned int pos = 0;
	while (true)
	{
		pos = GetFirstKeywordPosition(MkArraySection(pos), L"://");
		if (pos == MKDEF_ARRAY_ERROR)
			break;
		
		if (pos > 0)
		{
			const wchar_t& c = m_Str[pos - 1]; // 직전 문자가 알파벳이면
			if (((c >= L'A') && (c <= L'Z')) || ((c >= L'a') && (c <= L'z')))
			{
				m_Str[pos] = L'_'; // L"://" -> L"__#PK_"
				m_Str[pos + 1] = L'_';
				m_Str[pos + 2] = L'#';

				pos = Insert(pos + 3, L"PK_");
			}
		}
	}

	ReplaceAllBlocks(0, L"//", MkStr::LF, MkStr::CRLF);

	pos = GetFirstKeywordPosition(L"//");
	if (pos != MKDEF_ARRAY_ERROR) // L"//" ~ EOF
	{
		BackSpace(GetSize() - pos);
	}

	ReplaceKeyword(L"__#PK_", L"://");
}

//------------------------------------------------------------------------------------------------//

unsigned int MkStr::Tokenize(MkArray<MkStr>& tokens, const MkStr& separator, bool ignoreEmptyToken) const
{
	unsigned int sepCnt = CountKeyword(separator);
	tokens.Reserve(sepCnt + 1);

	if (sepCnt == 0)
	{
		tokens.PushBack(*this);
	}
	else
	{
		unsigned int currSize = GetSize();
		unsigned int sepSize = separator.GetSize();
		unsigned int beginPos = 0;
		while (true)
		{
			unsigned int found = GetFirstKeywordPosition(MkArraySection(beginPos), separator);
			if (found != MKDEF_ARRAY_ERROR)
			{
				if (beginPos < found)
				{
					MkStr subset;
					GetSubStr(MkArraySection(beginPos, found - beginPos), subset);
					tokens.PushBack(subset);
				}
				else if (!ignoreEmptyToken) // beginPos == found
				{
					tokens.PushBack(MkStr::EMPTY);
				}
				beginPos = found + sepSize;
			}
			else
			{
				if (beginPos < currSize)
				{
					MkStr subset;
					GetSubStr(MkArraySection(beginPos, currSize - beginPos), subset);
					tokens.PushBack(subset);
				}
				else if (!ignoreEmptyToken) // beginPos == found
				{
					tokens.PushBack(MkStr::EMPTY);
				}
				break;
			}
		}
	}

	return tokens.GetSize();
}

unsigned int MkStr::Tokenize(MkArray<MkStr>& tokens, const MkArray<MkStr>& separators, bool ignoreEmptyToken) const
{
	if (separators.Empty())
		return 0;

	const MkStr& firstSeparator = separators[0];
	unsigned int size = separators.GetSize();
	if (size > 1) // 구분자가 두개 이상일 경우만 복사본을 만들어 구분자를 한가지로 치환해 사용
	{
		MkStr tmpBuf = *this;
		for (unsigned int i=1; i<size; ++i)
		{
			tmpBuf.ReplaceKeyword(separators[i], firstSeparator); // 첫번째 구분자로 다른 구분자 치환
		}
		return tmpBuf.Tokenize(tokens, firstSeparator, ignoreEmptyToken);
	}
	return Tokenize(tokens, firstSeparator, ignoreEmptyToken);
}

unsigned int MkStr::Tokenize(MkArray<MkStr>& tokens) const
{
	MkArray<MkStr> separators(4);
	separators.PushBack(MkStr::SPACE);
	separators.PushBack(MkStr::TAB);
	separators.PushBack(MkStr::LF);
	separators.PushBack(MkStr::CR);
	return Tokenize(tokens, separators, true);
}

//------------------------------------------------------------------------------------------------//

void MkStr::PopFront(unsigned int count)
{
	unsigned int size = GetSize();
	if ((size == 0) || (count == 0) || (count == MKDEF_ARRAY_ERROR))
		return;

	if (size <= count)
	{
		Flush();
	}
	else
	{
		m_Str.Erase(MkArraySection(0, count));
	}
}

void MkStr::BackSpace(unsigned int count)
{
	unsigned int size = GetSize();
	if ((size == 0) || (count == 0) || (count == MKDEF_ARRAY_ERROR))
		return;

	if (size <= count)
	{
		Flush();
	}
	else
	{
		m_Str.PopBack(count + 1);
		m_Str.PushBack(NULL);
	}
}

void MkStr::AddRepetition(const MkStr& source, unsigned int count)
{
	if ((!source.Empty()) && (count > 0))
	{
		unsigned int addSize = source.GetSize() * count;
		MkArray<wchar_t> buffer(addSize);
		MkMemoryBlockDescriptor<wchar_t> desc = source.GetBodyBlockDescriptor();
		for (unsigned int i=0; i<count; ++i)
		{
			buffer.PushBack(desc);
		}

		Reserve(GetSize() + addSize);
		m_Str.PopBack(1);
		m_Str.PushBack(buffer);
		m_Str.PushBack(NULL);
	}
}

unsigned int MkStr::Insert(unsigned int position, const MkStr& source)
{
	unsigned int size = GetSize();
	if (position == size)
	{
		if (!source.Empty())
		{
			*this += source;
			return GetSize();
		}
		else
			return size;
	}
	else if (position < size)
	{
		if (!source.Empty())
		{
			m_Str.Insert(position, source.GetBodyBlockDescriptor());
			return (position + source.GetSize());
		}
		else
			return size;
	}
	return MKDEF_ARRAY_ERROR;
}

MkStr& MkStr::ToUpper(void)
{
	MK_INDEXING_LOOP(m_Str, i)
	{
		wchar_t& c = m_Str[i];
		if ((c >= L'a') && (c <= L'z'))
		{
			c -= 32;
		}
	}
	return *this;
}

MkStr& MkStr::ToUpper(unsigned int position)
{
	if (m_Str.IsValidIndex(position))
	{
		wchar_t& c = m_Str[position];
		if ((c >= L'a') && (c <= L'z'))
		{
			c -= 32;
		}
	}
	return *this;
}

MkStr& MkStr::ToLower(void)
{
	MK_INDEXING_LOOP(m_Str, i)
	{
		wchar_t& c = m_Str[i];
		if ((c >= L'A') && (c <= L'Z'))
		{
			c += 32;
		}
	}
	return *this;
}

MkStr& MkStr::ToLower(unsigned int position)
{
	if (m_Str.IsValidIndex(position))
	{
		wchar_t& c = m_Str[position];
		if ((c >= L'A') && (c <= L'Z'))
		{
			c += 32;
		}
	}
	return *this;
}

bool MkStr::ToBool(void) const
{
	MkStr tmpBuf = *this;
	tmpBuf.RemoveBlank();
	tmpBuf.ToLower();
	return ((tmpBuf == L"true") || (tmpBuf == L"yes"));
}

int MkStr::ToInteger(void) const
{
	MkStr tmpBuf = *this;
	tmpBuf.RemoveBlank();
	return _wtoi(tmpBuf);
}

unsigned int MkStr::ToUnsignedInteger(void) const
{
	MkStr tmpBuf = *this;
	tmpBuf.RemoveBlank();
	wchar_t* tmp = NULL;
	return static_cast<unsigned int>(wcstoul(tmpBuf, &tmp, 10));
}

__int64 MkStr::ToDoubleInteger(void) const
{
	MkStr tmpBuf = *this;
	tmpBuf.RemoveBlank();
	return _wtoi64(tmpBuf);
}

unsigned __int64 MkStr::ToDoubleUnsignedInteger(void) const
{
	MkStr tmpBuf = *this;
	tmpBuf.RemoveBlank();
	wchar_t* tmp = NULL;
	return _wcstoui64(tmpBuf, &tmp, 10);
}

float MkStr::ToFloat(void) const
{
	MkStr tmpBuf = *this;
	tmpBuf.RemoveBlank();
	unsigned int size = tmpBuf.GetSize();
	bool percentage = false;
	if ((size > 0) && (tmpBuf[size - 1] == L'%'))
	{
		tmpBuf.BackSpace(1); // delete '%'
		percentage = true;
	}
	float result = static_cast<float>(_wtof(tmpBuf));
	if (percentage)
	{
		result /= 100.f; // 100% -> 1.f
	}
	return result;
}

MkInt2 MkStr::ToInt2(void) const
{
	MkStr tmpBuf = *this;
	tmpBuf.RemoveBlank();
	tmpBuf.RemoveKeyword(L"(");
	tmpBuf.RemoveKeyword(L")");

	MkInt2 result;
	MkArray<MkStr> token;
	if (tmpBuf.Tokenize(token, L",") == 2)
	{
		result.x = _wtoi(token[0]);
		result.y = _wtoi(token[1]);
	}
	return result;
}

MkUInt2 MkStr::ToUInt2(void) const
{
	MkStr tmpBuf = *this;
	tmpBuf.RemoveBlank();
	tmpBuf.RemoveKeyword(L"(");
	tmpBuf.RemoveKeyword(L")");

	MkUInt2 result;
	MkArray<MkStr> token;
	if (tmpBuf.Tokenize(token, L",") == 2)
	{
		wchar_t* tmp = NULL;
		result.x = static_cast<unsigned int>(wcstoul(token[0], &tmp, 10));
		result.y = static_cast<unsigned int>(wcstoul(token[1], &tmp, 10));
	}
	return result;
}

MkVec2 MkStr::ToVec2(void) const
{
	MkStr tmpBuf = *this;
	tmpBuf.RemoveBlank();
	tmpBuf.RemoveKeyword(L"(");
	tmpBuf.RemoveKeyword(L")");

	MkVec2 result;
	MkArray<MkStr> token;
	if (tmpBuf.Tokenize(token, L",") == 2)
	{
		result.x = token[0].ToFloat();
		result.y = token[1].ToFloat();
	}
	return result;
}

MkVec3 MkStr::ToVec3(void) const
{
	MkStr tmpBuf = *this;
	tmpBuf.RemoveBlank();
	tmpBuf.RemoveKeyword(L"(");
	tmpBuf.RemoveKeyword(L")");

	MkVec3 result;
	MkArray<MkStr> token;
	if (tmpBuf.Tokenize(token, L",") == 3)
	{
		result.x = token[0].ToFloat();
		result.y = token[1].ToFloat();
		result.z = token[2].ToFloat();
	}
	return result;
}

//------------------------------------------------------------------------------------------------//

bool MkStr::ReadTextFile(const MkPathName& filePath, bool ignoreComment, bool ANSI)
{
	MkByteArray byteArray;
	if (!MkFileManager::GetFileData(filePath, byteArray))
		return false;

	ReadTextStream(byteArray, ignoreComment, ANSI);
	return true;
}

void MkStr::ReadTextStream(const MkByteArray& byteArray, bool ignoreComment, bool ANSI)
{
	// text 파일은 멀티바이트 폼으로 저장되므로 std::wifstream를 사용하면 자동으로 인코딩 해 주지만
	// 파일 시스템을 통해 std::ifstream(unsigned char)로 읽은 상태라 별도 디코딩을 함
	unsigned int srcSize = byteArray.GetSize();
	MkByteArray srcData;
	unsigned int codePage;

	// BOM : UTF-8
	if ((srcSize >= 3) && (byteArray[0] == 0xEF) && (byteArray[1] == 0xBB) && (byteArray[2] == 0xBF))
	{
		byteArray.GetSubArray(MkArraySection(3), srcData);
		codePage = CP_UTF8;
	}
	else
	{
		srcData = byteArray;
		codePage = (ANSI) ? gCurrentCodePage : CP_UTF8;
	}
	srcData.PushBack(NULL);

	std::wstring buffer;
	MkStringOp::ConvertString(std::string(reinterpret_cast<const char*>(srcData.GetPtr())), buffer, codePage);
	MkStr tempStr = buffer.c_str();

	// tempStr 에서 주석 삭제
	if (ignoreComment)
	{
		tempStr.RemoveAllComment();
	}

	// 문자열에 추가
	*this += tempStr;
}

bool MkStr::WriteToTextFile(const MkPathName& filePath, bool overwrite, bool ANSI) const
{
	if (ANSI)
	{
		MkPathName absoluteFilePath;
		absoluteFilePath.ConvertToRootBasisAbsolutePath(filePath);

		std::wofstream outStream;
		outStream.open(absoluteFilePath, std::ios::out | (overwrite ? std::ios::trunc : std::ios::app));
		if (!outStream.is_open())
			return false;

		outStream.write(GetPtr(), GetSize());
		outStream.close();
		return true;
	}
	else
	{
		// 변환
		std::string buffer;
		MkStringOp::ConvertString(std::wstring(GetPtr()), buffer, CP_UTF8);

		// line feed를 window format(CR+LF)로 정규화
		MkStringOp::RemoveKeyword(buffer, "\r", 0);
		MkStringOp::ReplaceKeyword(buffer, "\n", "\r\n", 0);
		unsigned int strSize = static_cast<unsigned int>(buffer.size());

		// 출력용 데이터
		MkByteArray outData;
		outData.Reserve(3 + strSize);

		outData.PushBack(0xEF); // BOM
		outData.PushBack(0xBB);
		outData.PushBack(0xBF);

		outData.PushBack(MkMemoryBlockDescriptor<unsigned char>(reinterpret_cast<const unsigned char*>(buffer.c_str()), strSize));

		// 파일로 저장
		MkInterfaceForFileWriting fwInterface;
		if (!fwInterface.SetUp(filePath, overwrite, false))
			return false;

		return (fwInterface.Write(outData, MkArraySection(0)) == outData.GetSize());
	}
}

//------------------------------------------------------------------------------------------------//

void MkStr::GetLastErrorMessage(void)
{
	DWORD errCode = ::GetLastError();

	// 영어가 가장 정확함
	wchar_t* errMsg;
	::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errCode, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), reinterpret_cast<LPWSTR>(&errMsg), 0, NULL);

	Clear();
	Reserve(512);
	*this += L"(";
	*this += static_cast<int>(errCode);
	*this += L") ";
	*this += errMsg;

	::LocalFree(errMsg);
}

//------------------------------------------------------------------------------------------------//

bool MkStr::_IsBlank(wchar_t character) const
{
	return ((character == MKDEF_WCHAR_TAP) ||
		(character == MKDEF_WCHAR_LINEFEED) ||
		(character == MKDEF_WCHAR_RETURN) ||
		(character == MKDEF_WCHAR_SPACE));
}

//------------------------------------------------------------------------------------------------//
