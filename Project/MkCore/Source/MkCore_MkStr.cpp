
#include <assert.h>
#include <locale>
#include <atlcoll.h>
#include <fstream>
#include <time.h>
#include "MkCore_MkVec2.h"
#include "MkCore_MkVec3.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkStr.h"


// CP_ACP 0
// CP_KOREAN 949
// CP_JAPANESE 932
// CP_CHINESE_SIMPLIFIED 936 // 간체. 중국 본토
// CP_CHINESE_TRADITIONAL 950 // 번체. 한국, 대만, 일본
// CP_USA 437
// CP_THAILAND 874
// CP_GERMANY 1252

#define MKDEF_WCHAR_TAP L'\t'
#define MKDEF_WCHAR_LINEFEED L'\n'
#define MKDEF_WCHAR_RETURN L'\r'
#define MKDEF_WCHAR_SPACE L' '
#define MKDEF_STR_TAP L"\t"
#define MKDEF_STR_LINEFEED L"\n"
#define MKDEF_STR_RETURN L"\r"
#define MKDEF_STR_SPACE L" "

// static value
static unsigned int gCurrentCodePage = CP_ACP;
static MkArray<wchar_t> gBlankTag;

//------------------------------------------------------------------------------------------------//

void MkStr::SetUp(unsigned int codePage)
{
	// code page가 별도 설정되어 있지 않으면 OS 설정을 따름
	if (codePage == 0)
	{
		std::locale newLocale("");
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

	SetCodePage(codePage);

	gBlankTag.Reserve(4);
	gBlankTag.PushBack(MKDEF_WCHAR_TAP);
	gBlankTag.PushBack(MKDEF_WCHAR_LINEFEED);
	gBlankTag.PushBack(MKDEF_WCHAR_RETURN);
	gBlankTag.PushBack(MKDEF_WCHAR_SPACE);
}

void MkStr::SetCodePage(unsigned int codePage) { gCurrentCodePage = codePage; }

unsigned int MkStr::GetCodePage(void) { return gCurrentCodePage; }

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
	*this = (value) ? L"Yes" : L"No";
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
	swprintf_s(tmp, 50, L"%.3f", value);
	*this = tmp;
	return *this;
}

MkStr& MkStr::operator = (const double& value)
{
	wchar_t tmp[320];
	swprintf_s(tmp, 320, L"%.3f", value);
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
	wchar_t tmp[100];
	swprintf_s(tmp, 100, L"(%.3f, %.3f)", value.x, value.y);
	*this = tmp;
	return *this;
}

MkStr& MkStr::operator = (const MkVec3& value)
{
	wchar_t tmp[150];
	swprintf_s(tmp, 150, L"(%-5.3f, %-5.3f, %-5.3f)", value.x, value.y, value.z);
	*this = tmp;
	return *this;
}

MkStr& MkStr::operator = (const MkMemoryBlockDescriptor<unsigned char>& byteBlock)
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
	int sizeOfMultiByte = static_cast<int>(str.size());
	int sizeOfWideChar = MultiByteToWideChar(gCurrentCodePage, 0, str.c_str(), sizeOfMultiByte, NULL, 0);
	wchar_t* buffer = SysAllocStringLen(NULL, sizeOfWideChar);
	// 완성형으로 변환하고 싶을때는 두번째 인자로 MB_PRECOMPOSED 사용 할 것
	MultiByteToWideChar(gCurrentCodePage, 0, str.c_str(), sizeOfMultiByte, buffer, sizeOfWideChar);
	buffer[sizeOfWideChar] = NULL;
	*this = buffer;
	SysFreeString(buffer);
}

void MkStr::ExportMultiByteString(std::string& str) const
{
	const wchar_t* sourcePtr = m_Str.GetPtr();
	int sizeOfChar = WideCharToMultiByte(gCurrentCodePage, 0, sourcePtr, -1, NULL, 0, NULL, NULL);
	if (sizeOfChar > 0)
	{
		char* buffer = new char[sizeOfChar];
		WideCharToMultiByte(gCurrentCodePage, 0, sourcePtr, -1, buffer, sizeOfChar, NULL, NULL);
		str = buffer;
		delete [] buffer;
	}
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
	m_Str.ReplaceAll(MkArraySection(0), keywordFrom.GetBodyBlockDescriptor(), keywordTo.GetBodyBlockDescriptor());
}

void MkStr::ReplaceKeyword(const MkArraySection& section, const MkStr& keywordFrom, const MkStr& keywordTo)
{
	m_Str.ReplaceAll(section, keywordFrom.GetBodyBlockDescriptor(), keywordTo.GetBodyBlockDescriptor());
}

void MkStr::ReplaceCRtoTag(void)
{
	ReplaceKeyword(CR, L"\\r\\n");
	ReplaceKeyword(L"\r", L"\\r");
	ReplaceKeyword(L"\n", L"\\n");
}

void MkStr::ReplaceTagtoCR(void)
{
	ReplaceKeyword(L"\\r\\n", CR);
	ReplaceKeyword(L"\\r", L"\r");
	ReplaceKeyword(L"\\n", L"\n");
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
	ReplaceAllBlocks(0, L"//", MKDEF_STR_LINEFEED, MKDEF_STR_LINEFEED);

	unsigned int pos = GetFirstKeywordPosition(L"//");
	if (pos != MKDEF_ARRAY_ERROR) // L"//" ~ EOF
	{
		BackSpace(GetSize() - pos);
	}
}

//------------------------------------------------------------------------------------------------//

unsigned int MkStr::Tokenize(MkArray<MkStr>& tokens, const MkStr& separator) const
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
				break;
			}
		}
	}

	return tokens.GetSize();
}

unsigned int MkStr::Tokenize(MkArray<MkStr>& tokens, const MkArray<MkStr>& separators) const
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
		return tmpBuf.Tokenize(tokens, firstSeparator);
	}
	return Tokenize(tokens, firstSeparator);
}

unsigned int MkStr::Tokenize(MkArray<MkStr>& tokens) const
{
	MkArray<MkStr> separators(4);
	separators.PushBack(MKDEF_STR_SPACE);
	separators.PushBack(MKDEF_STR_TAP);
	separators.PushBack(MKDEF_STR_LINEFEED);
	separators.PushBack(MKDEF_STR_RETURN);
	return Tokenize(tokens, separators);
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
	wchar_t* tmp;
	return static_cast<unsigned int>(wcstoul(tmpBuf, &tmp, 10));
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
		result *= 0.01f; // 100% -> 1.f
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
		wchar_t* tmp;
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

bool MkStr::ReadTextFile(const MkPathName& filePath, bool ignoreComment)
{
	MkByteArray byteArray;
	if (!MkFileManager::GetFileData(filePath, byteArray))
		return false;

	ReadTextStream(byteArray, ignoreComment);
	return true;
}

void MkStr::ReadTextStream(const MkByteArray& byteArray, bool ignoreComment)
{
	MkByteArray buffer = byteArray;

	// text 파일은 멀티바이트 폼으로 저장되므로 std::wifstream를 사용하면 자동으로 wchar_t 기반으로 변환해 주지만
	// 파일 시스템을 통해 std::ifstream(unsigned char)로 읽은 상태라 std::string를 한 번 거치게 함
	unsigned int srcSize = buffer.GetSize();
	buffer.Fill(srcSize + 1);
	buffer.GetPtr()[srcSize] = NULL;
	MkStr tempStr;
	tempStr.ImportMultiByteString(std::string(reinterpret_cast<const char*>(buffer.GetPtr())));
/*	
	// 파일 오픈
	std::wifstream inStream;
	inStream.open(fullPath, std::ios::in);
	if (!inStream.is_open())
		return false;

	// 텍스트 크기
	unsigned int stringCount = static_cast<unsigned int>(std::distance(std::istreambuf_iterator<wchar_t>(inStream), std::istreambuf_iterator<wchar_t>()));
	if (stringCount > 0)
	{
		// 파일 스트림 -> 버퍼
		inStream.seekg(0, std::ios::beg);
		wchar_t* charBuf = new wchar_t[stringCount + 1];
		inStream.read(charBuf, stringCount);
		charBuf[stringCount] = NULL;
		inStream.close();
		tempStr = charBuf;
		delete [] charBuf;
	}
*/
	// tempStr 에서 주석 삭제
	if (ignoreComment)
	{
		tempStr.RemoveAllComment();
	}

	// 문자열에 추가
	*this += tempStr;
}

bool MkStr::WriteToTextFile(const MkPathName& filePath, bool overwrite) const
{
	MkPathName absoluteFilePath;
	absoluteFilePath.ConvertToRootBasisAbsolutePath(filePath);

	// 파일 오픈
	std::wofstream outStream;
	outStream.open(absoluteFilePath, std::ios::out | (overwrite ? std::ios::trunc : std::ios::app));
	if (!outStream.is_open())
		return false;

	// 변환 후 기록
	outStream.write(GetPtr(), GetSize());
	outStream.close();
	return true;
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
