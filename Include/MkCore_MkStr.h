#ifndef __MINIKEY_CORE_MKSTR_H__
#define __MINIKEY_CORE_MKSTR_H__

//------------------------------------------------------------------------------------------------//
// MkArray<wchar_t>�� ���̽��� �� �����ڵ� ��� ���ڿ�
// - Ű���� ���� �� ��ġ �˻�
// - Ű���� �˻� �� ����
// - ��ϴ��� ó��
// - ��ūȭ
// - ��ȯ
// - �ؽ�Ʈ ���� �����
//
// OS �⺻ ������ ���� Ÿ �� ��� �� ��� ���ڵ��� �ڵ��������� ������ ���� �� �ִ��� UTF-8 ��� ����
//
// char���� ��ȯ�� �ǵ������� ������ ����(���ɰῡ ȥ���ϴ� ���� ����)�ϸ� ����ϴ���
// std::string�� ���� ���������� �����
//
// �̷л����δ� ����Ʈ ���ĵ� �����ڵ� ó���� ��Ƽ����Ʈ���� ������ �����̰� ���������ε�
// g++�� ������ ��������� �����ڵ尡 2~3�� ���� �� ��������, VS�� ���ڿ� ó�� api����
// ��Ƽ����Ʈ ó���� �������, �����ڵ�� c�� ����ٺ��� VS������ŭ�� �����ڵ尡 �� ����
//------------------------------------------------------------------------------------------------//

#include <string>
#include "MkCore_MkArray.h"
#include "MkCore_MkType2.h"


class MkVec2;
class MkVec3;
class MkHashStr;
class MkPathName;

class MkStr
{
public:

	//------------------------------------------------------------------------------------------------//
	// �ʱ�ȭ. ���α׷� ���۽� �� �� ���� �ʿ�
	// ��Ģ�����δ� lock�� �ɾ�� ������ ����ڰ� ������ �ʱ�ȭ �� �� ���̶� ���
	//------------------------------------------------------------------------------------------------//

	static void SetUp(unsigned int codePage = 0); // 0 : OS default

	// �ڵ� ������
	static void SetGlobalCodePage(unsigned int codePage);
	static unsigned int GetGlobalCodePage(void);

	// LANGID
	// ex> primary id : LANG_SPANISH, sub id : SUBLANG_SPANISH_MEXICAN
	static unsigned short GetPrimaryLanguageID(void);
	static unsigned short GetSubLanguageID(void);

	//------------------------------------------------------------------------------------------------//
	// ������
	// ��Ģ������ char Ÿ���� ����
	//------------------------------------------------------------------------------------------------//

	MkStr(void);
	MkStr(const MkStr& str);
	MkStr(const MkPathName& str);
	MkStr(const MkHashStr& str);
	MkStr(const wchar_t& character);
	MkStr(const wchar_t* wcharArray);
	MkStr(const std::wstring& str);
	MkStr(const MkByteArray& str);
	MkStr(const bool& value);
	MkStr(const int& value);
	MkStr(const unsigned int& value);
	MkStr(const float& value);
	MkStr(const double& value);
	MkStr(const __int64& value);
	MkStr(const unsigned __int64& value);
	MkStr(const MkInt2& pt);
	MkStr(const MkUInt2& pt);
	MkStr(const MkVec2& value);
	MkStr(const MkVec3& value);

	virtual ~MkStr() { m_Str.Clear(); }

	//------------------------------------------------------------------------------------------------//
	// ������
	//------------------------------------------------------------------------------------------------//

	// �Ҵ�
	MkStr& operator = (const MkStr& str);
	MkStr& operator = (const MkPathName& str);
	MkStr& operator = (const MkHashStr& str);
	MkStr& operator = (const wchar_t& character);
	MkStr& operator = (const wchar_t* wcharArray);
	MkStr& operator = (const std::wstring& str);
	MkStr& operator = (const MkByteArray& str);
	MkStr& operator = (const bool& value);
	MkStr& operator = (const int& value);
	MkStr& operator = (const unsigned int& value);
	MkStr& operator = (const float& value);
	MkStr& operator = (const double& value);
	MkStr& operator = (const __int64& value);
	MkStr& operator = (const unsigned __int64& value);
	MkStr& operator = (const MkInt2& pt);
	MkStr& operator = (const MkUInt2& pt);
	MkStr& operator = (const MkVec2& value);
	MkStr& operator = (const MkVec3& value);
	MkStr& operator = (const MkByteArrayDescriptor& byteBlock);
	
	// ��
	bool operator == (const MkStr& str) const;
	bool operator == (const wchar_t* str) const;
	bool operator != (const MkStr& str) const;
	bool operator != (const wchar_t* str) const;
	bool operator < (const MkStr& str) const;
	bool operator > (const MkStr& str) const;
	bool operator <= (const MkStr& str) const;
	bool operator >= (const MkStr& str) const;

	// �߰�
	MkStr operator + (const MkStr& str) const;
	MkStr& operator += (const MkStr& str);

	friend MkStr operator + (const wchar_t* charArray, const MkStr& str)
	{
		MkStr tmp(charArray);
		tmp += str;
		return tmp;
	}

	// ����
	wchar_t& operator [] (unsigned int offset);
	const wchar_t& operator [] (unsigned int offset) const;

	// ĳ����
	inline operator const wchar_t*() const { return m_Str.GetPtr(); }

	// XOR
	MkStr operator ^ (const MkStr& keyStr) const;
	MkStr& operator ^= (const MkStr& keyStr);

	//------------------------------------------------------------------------------------------------//
	// �޸� ����
	//------------------------------------------------------------------------------------------------//

	// ũ�� ����
	void Reserve(unsigned int size);

	// �޸� ����ȭ
	void OptimizeMemory(void);

	// ����(�޸� ����)
	void Flush(void);

	// ����(�޸� ����)
	void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// ���� ����
	//------------------------------------------------------------------------------------------------//

	// ���ڿ��� ������� ���� ��ȯ
	bool Empty(void) const;

	// ���� �� ��ȯ
	unsigned int GetSize(void) const;

	// ������ ���ڿ����� ���� ��ȯ
	// (NOTE) ������ ������� ����
	bool IsDigit(void) const;

	// �ش� ���ڿ��� �����ڵ� �뿪���� ������ �ڵ� ������ ��ȯ
	// (NOTE) �ٱ�� ���� �ִ� ��� �켱������ ���� CP�� ��ȯ(ex> ����, ����, �Ϻ�� ���� ���� ��� �Ϻ���� �ν�)
	unsigned int GetRepresentativeCodePage(void) const;

	// ������ ��ȯ
	// std::wstring ���¸� ���� ��� �����Ҵ� ����
	// ex>
	//	MkStr str = L"������";
	//	std::wstring tmp = str.GetPtr();
	inline wchar_t* GetPtr(void) { return m_Str.GetPtr(); }
	inline const wchar_t* GetPtr(void) const { return m_Str.GetPtr(); }

	// ���� ����(== operator[])
	const wchar_t& GetAt(unsigned int offset) const;

	// ������ �����ڰ� ���ܵ� �޸� ��� ��ȯ
	MkMemoryBlockDescriptor<wchar_t> GetBodyBlockDescriptor(void) const;

	// ������ �����ڰ� ���Ե� �޸� ��� ��ȯ
	MkMemoryBlockDescriptor<wchar_t> GetWholeBlockDescriptor(void) const;

	// ���� ������ �޸� ��� ��ȯ
	MkMemoryBlockDescriptor<wchar_t> GetMemoryBlockDescriptor(const MkArraySection& section) const;

	//------------------------------------------------------------------------------------------------//
	// std::string�� ���� char�� ������
	//------------------------------------------------------------------------------------------------//

	// *this <- std::string
	void ImportMultiByteString(const std::string& str);

	// *this -> std::string
	void ExportMultiByteString(std::string& str) const;

	//------------------------------------------------------------------------------------------------//
	// MkByteArray ��ȣȭ
	// 1byte(0x00~0xFF)�� L"00"~L"FF"�� �ٲٹǷ� 1byte -> 4byte(wchar_t * 2)�� �뷮 ����
	// ��ȿ���������� �������� �ӽ÷� ���ڿ� ���·� �����ϱ� ����
	//------------------------------------------------------------------------------------------------//

	// *this <- MkByteArray
	void ImportByteArray(const MkByteArray& buffer);

	// *this -> MkByteArray
	bool ExportByteArray(MkByteArray& buffer) const;

	//------------------------------------------------------------------------------------------------//
	// Ű���� ���� �� ��ġ �˻�
	//------------------------------------------------------------------------------------------------//

	// Ư�� ���ڿ� ���Կ��� ��ȯ
	// (in) section : �˻� ����
	// (in) keyword : �˻��� ���ڿ�
	// return : ���� ����
	// ex> (L"abc##def##gh").Exist(L"##") -> true
	bool Exist(const MkStr& keyword) const;
	bool Exist(const MkArraySection& section, const MkStr& keyword) const;

	// Ư�� ���ڿ��� ������ ��ġ�� �����ϴ��� ���� ��ȯ
	// (in) position : source�� �˻� ��ġ
	// (in) keyword : �˻��� ���ڿ�
	// return : ���� ����
	// (NOTE) ���� ���ΰ� �ƴ� ���� ������
	// ex> (L"abc##def##gh").Equals(3, L"##") -> true
	// ex> (L"abc####def##gh").Equals(3, L"##") -> true
	// ex> (L"abc#!!#def##gh").Equals(3, L"##") -> false
	bool Equals(unsigned int position, const MkStr& keyword) const;

	// Ư�� ���ڿ��� ���Ե� ���� ��ȯ
	// (in) section : �˻� ����
	// (in) keyword : �˻��� ���ڿ�
	// (out) positions : �˻��� ��ġ�� ����� ����
	// return : ���� ����. keyword�� ����� ��� MKDEF_ARRAY_ERROR ��ȯ
	// ex> (L"abc##def##gh").CountKeyword(L"##") -> 1
	// ex> (L"abc##def##gh").CountKeyword(L"##", positions) -> 2, positions(3, 8)
	unsigned int CountKeyword(const MkStr& keyword) const;
	unsigned int CountKeyword(const MkArraySection& section, const MkStr& keyword) const;
	unsigned int CountKeyword(const MkStr& keyword, MkArray<unsigned int>& positions) const;
	unsigned int CountKeyword(const MkArraySection& section, const MkStr& keyword, MkArray<unsigned int>& positions) const;

	// Ư�� �ܾ�(�����ڷ� ���еǴ� ���ڿ�)�� ���Ե� ���� ��ȯ
	// (in) word : �˻��� �ܾ�
	// return : ���� ����. �ܾ ����� ��� MKDEF_ARRAY_ERROR ��ȯ
	// ex> (L"abc ## def ##gh").CountWord(L"##") -> 1, (L"abc ## def ## gh").CountKeyword(L"##") -> 2
	unsigned int CountWord(const MkStr& word) const;

	// prefix ���� �˻�
	// (in) prefix : �˻��� ���ڿ�
	// return : ���� ����
	// ex> (L"abc_ka").CheckPrefix(L"abc_") -> true
	bool CheckPrefix(const MkStr& prefix) const;

	// postfix ���� �˻�
	// (in) postfix : �˻��� ���ڿ�
	// return : ���� ����
	// ex> (L"ka_abc").CheckPostfix(L"_abc") -> true
	bool CheckPostfix(const MkStr& postfix) const;

	// ���� �˻��� Ư�� ���ڿ� ��ġ �˻�
	// (in) section : �˻� ����
	// (in) keyword : �˻��� ���ڿ�
	// return : keyword�� ����ų� ���Ե��� �ʾ����� MKDEF_ARRAY_ERROR, ���ԵǾ����� ��ġ ��ȯ
	// ex> (L"012##5##89").GetFirstKeywordPosition(L"##") -> 3
	unsigned int GetFirstKeywordPosition(const MkStr& keyword) const;
	unsigned int GetFirstKeywordPosition(const MkArraySection& section, const MkStr& keyword) const;

	// position���� �����ϴ� ���� ��ȿ����(�����ڰ� �ƴ�) ��ġ�� ��ȯ
	// (in) position : source�� �˻� ���� ��ġ
	// return : �������� ������ MKDEF_ARRAY_ERROR, ���ԵǾ����� ��ġ ��ȯ
	unsigned int GetFirstValidPosition(unsigned int position = 0) const;

	// position���� �����ϴ� ���� ������ ��ġ�� ��ȯ
	// (in) position : source�� �˻� ���� ��ġ
	// return : �������� ������ MKDEF_ARRAY_ERROR, ���ԵǾ����� ��ġ ��ȯ
	unsigned int GetFirstBlankPosition(unsigned int position = 0) const;

	// position���� �����ϴ� ���� line feed(\n) ��ġ�� ��ȯ
	// (in) position : source�� �˻� ���� ��ġ
	// return : �������� ������ MKDEF_ARRAY_ERROR, ���ԵǾ����� ��ġ ��ȯ
	unsigned int GetFirstLineFeedPosition(unsigned int position = 0) const;

	// position���� �����ϴ� ������ ��ȿ����(�����ڰ� �ƴ�) ��ġ�� ��ȯ
	// (in) position : source�� �˻� ���� ��ġ
	// return : �������� ������ MKDEF_ARRAY_ERROR, ���ԵǾ����� ��ġ ��ȯ
	unsigned int GetLastValidPosition(unsigned int position = 0) const;

	// position���� ���ڿ��� ������ ������ ��ȿ���ڱ��� �����ϴ� ������ ������ ��ȯ
	// (in) position : source�� �˻� ���� ��ġ
	// return : ������ ����
	unsigned int CountBacksideBlank(unsigned int position = 0) const;

	// position�� �ؽ�Ʈ�� �� ��° �������� ��ȯ
	// (in) position : cursor ��ġ
	// return : �ش� ���� ��ȣ. 0���� ����. position�� ������ �Ѿ�� ������ ���� ��ȣ ��ȯ
	unsigned int GetLineNumber(unsigned int position = 0) const;

	//------------------------------------------------------------------------------------------------//
	// Ű���� �˻� �� ����
	//------------------------------------------------------------------------------------------------//

	// keywordFrom�� keywordTo�� ġȯ
	// (in) section : �˻� ����
	// (in) keywordFrom : ġȯ ��� ���ڿ�
	// (in) keywordTo : ġȯ�� ���ڿ�
	// ex> (L"12abc3abc456").ReplaceKeyword(L"abc", L"@") -> L"12@3@456"
	void ReplaceKeyword(const MkStr& keywordFrom, const MkStr& keywordTo);
	void ReplaceKeyword(const MkArraySection& section, const MkStr& keywordFrom, const MkStr& keywordTo);

	// CRLF�� tag(L"\\r\\n")���� ��ȯ
	void ReplaceCRLFtoTag(void);

	// tag(L"\\r\\n")�� CRLF�� ��ȯ
	void ReplaceTagtoCRLF(void);

	// Ű���� ����
	// (in) section : �˻� ����
	// (in) keyword : ���� ��� ���ڿ�
	// ex> (L"12abc3abc456").RemoveKeyword(L"abc") -> L"123456"
	void RemoveKeyword(const MkStr& keyword);
	void RemoveKeyword(const MkArraySection& section, const MkStr& keyword);

	// ��� ��ũ(tab, space, line feed, return)�� ����
	// (in) section : �˻� ����
	// ex> (L"a \nb  c \nd  ").RemoveBlank() -> L"abcd"
	void RemoveBlank(void);
	void RemoveBlank(const MkArraySection& section);

	// ù ��ȿ���ڱ����� ��ũ(tab, space, line feed, return)�� ����
	// ������ ������ �� ��ȯ
	// ex> (L"  a bc  ").RemoveFrontSideBlank() -> L"a bc  "
	unsigned int RemoveFrontSideBlank(void);

	// ������ ��ȿ���� ������ ��ũ(tab, space, line feed, return)�� ����
	// ������ ������ �� ��ȯ
	// ex> (L"  a bc  ").RemoveRearSideBlank() -> L"  a bc"
	unsigned int RemoveRearSideBlank(void);

	//------------------------------------------------------------------------------------------------//
	// ��ϴ��� ó��
	//------------------------------------------------------------------------------------------------//

	// �κ� ���ڿ� �����ȯ
	// (in) section : ���� ����
	// (out) buffer : �Ҵ�� ���ڿ� ����
	void GetSubStr(const MkArraySection& section, MkStr& buffer) const;

	// position���� ���� �ܾ�(�����ڷ� ���еǴ� ��ȿ���� ����)�� �˻��� buffer�� �Ҵ�
	// (in) position : �˻� ������ġ
	// (out) buffer : �Ҵ�� ���ڿ� ����
	// return :
	//	- �ش� ���ڿ��� �����ϸ� �ܾ� ���� ��ġ�� ��ȯ
	//	- position�� source�� ũ�⸦ �Ѿ�ų� �ܾ �������� ������ MKDEF_ARRAY_ERROR ��ȯ
	// ex>
	//	MkStr buffer;
	//	(L"  abc 123").GetFirstWord(0, buffer) -> 5, buffer == L"abc"
	unsigned int GetFirstWord(unsigned int position, MkStr& buffer) const;

	// position���� endKeyword ������ ���� ����� �˻��� endKeyword�� ���ܵ� ��� ���ڿ��� buffer�� �Ҵ�
	// (in) position : �˻� ������ġ
	// (in) endKeyword : ��� ���� ���ڿ�
	// (out) buffer : �Ҵ�� ���ڿ� ����
	// return :
	//	- �ش� ���ڿ��� �����ϸ� endKeyword ���� ��ġ�� ��ȯ
	//	- position�� source�� ũ�⸦ �Ѿ�ų� ����� �������� ������ MKDEF_ARRAY_ERROR ��ȯ
	// ex>
	//	MkStr buffer;
	//	(L"abc #123").GetFirstBlock(1, L"#", buffer) -> 5, buffer == L"bc "
	//	(L"abc{123}de").GetFirstBlock(0, L"{", L"}", buffer) -> 8, buffer == L"123"
	unsigned int GetFirstBlock // position ~ endKeyword
		(unsigned int position, const MkStr& endKeyword, MkStr& buffer) const;

	unsigned int GetFirstBlock // beginKeyword ~ endKeyword
		(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, MkStr& buffer) const;

	// position���� ���� depth�� beginKeyword �� endKeyword ������ ����� �˻��� Ű���尡 ���ܵ� ��� ��ġ�� ��ȯ
	// (in) position : �˻� ������ġ
	// (in) beginKeyword : ��� ���� ���ڿ�
	// (in) endKeyword : ��� ���� ���ڿ�
	// (out) blockBegin : beginKeyword ���� ��ġ
	// (out) blockEnd : endKeyword ��ġ
	// return :
	//	- �ش� ���ڿ��� �����ϸ� endKeyword ���� ��ġ�� ��ȯ
	//	- position�� source�� ũ�⸦ �Ѿ�ų� ����� �������� ������ MKDEF_ARRAY_ERROR ��ȯ
	// ex>
	//	unsigned int begin, end;
	//	(L"abc{12{##}3}de").FindFirstStackPosition(0, L"{", L"}", begin, end) -> 12, begin == 4, end == 11
	unsigned int FindFirstStackPosition
		(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, unsigned int& blockBegin, unsigned int& blockEnd) const;

	unsigned int FindFirstStackPosition // ��ġ ��� ���� ������ ��ȯ
		(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, MkArraySection& section) const;

	// position���� ���� depth�� beginKeyword �� endKeyword ������ ����� �˻��� Ű���尡 ���ܵ� ��� ���ڿ��� buffer�� �Ҵ�
	// (in) position : �˻� ������ġ
	// (in) beginKeyword : ��� ���� ���ڿ�
	// (in) endKeyword : ��� ���� ���ڿ�
	// (out) buffer : �Ҵ�� ���ڿ� ����
	// return :
	//	- �ش� ���ڿ��� �����ϸ� endKeyword ���� ��ġ�� ��ȯ
	//	- position�� source�� ũ�⸦ �Ѿ�ų� ����� �������� ������ MKDEF_ARRAY_ERROR ��ȯ
	// ex>
	//	MkStr buffer;
	//	(L"abc{12{##}3}de").GetFirstStackBlock(0, L"{", L"}", buffer) -> buffer == L"12{##}3"
	//	���� ���÷� GetFirstBlock()�� �����ϸ� buffer == L"12{##" �� ����
	unsigned int GetFirstStackBlock(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, MkStr& buffer) const;

	// position���� beginKeyword �� endKeyword ������ ù��° ����� keyword �����Ͽ� ����
	// (in) position : �˻� ������ġ
	// (in) beginKeyword : ��� ���� ���ڿ�
	// (in) endKeyword : ��� ���� ���ڿ�
	// return : ������ ��ġ. position�� source�� ũ�⸦ �Ѿ�ų� ��Ī ����� �������� ������ MKDEF_ARRAY_ERROR ��ȯ
	// ex>
	//	(L"abc{123}de").EraseFirstBlock(0, L"{", L"}") -> *this == L"abcde"
	unsigned int EraseFirstBlock(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword);

	unsigned int EraseFirstBlock // ���� �� ��� ���ڿ��� buffer�� �Ҵ�
		(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, MkStr& buffer);

	// position���� beginKeyword �� endKeyword ������ ��� ����� keyword �����Ͽ� ����
	// (in) position : �˻� ������ġ
	// (in) beginKeyword : ��� ���� ���ڿ�
	// (in) endKeyword : ��� ���� ���ڿ�
	// (NOTE) ���� ����
	// ex>
	//	(L"abc{123}de{}f{45}").EraseAllBlocks(0, L"{", L"}") -> *this == L"abcdef"
	void EraseAllBlocks(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword);

	// position���� beginKeyword �� endKeyword ������ ù��° ����� keyword �����Ͽ� replaceBlock���� ��ü
	// replaceKeyword�� �����ڿ��� ��� ������ ����
	// (in) position : �˻� ������ġ
	// (in) beginKeyword : ��� ���� ���ڿ�
	// (in) endKeyword : ��� ���� ���ڿ�
	// (in) replaceBlock : ġȯ�� ���ڿ�
	// return : ��ü�� replaceBlock �ٷ� ���� ��ġ. position�� source�� ũ�⸦ �Ѿ�ų� ��Ī ����� �������� ������ MKDEF_ARRAY_ERROR ��ȯ
	// ex>
	//	ġȯ : (L"abc{123}de").ReplaceFirstBlock(0, L"{", L"}", L"##") -> *this == L"abc##de"
	//	���� : (L"abc{123}de").ReplaceFirstBlock(0, L"{", L"}", MkStr::EMPTY) -> *this == L"abcde"
	unsigned int ReplaceFirstBlock(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, const MkStr& replaceBlock);

	unsigned int ReplaceFirstBlock // ġȯ �� ��� ���ڿ��� buffer�� �Ҵ�
		(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, const MkStr& replaceBlock, MkStr& buffer);

	// position���� beginKeyword �� endKeyword ������ ��� ����� keyword �����Ͽ� replaceBlock���� ��ü
	// (in) position : �˻� ������ġ
	// (in) beginKeyword : ��� ���� ���ڿ�
	// (in) endKeyword : ��� ���� ���ڿ�
	// (in) replaceBlock : ġȯ�� ���ڿ�
	// (NOTE) ���� ����
	// ex>
	//	(L"abc{123}de{}{fg}h").ReplaceAllBlocks(0, L"{", L"}", L"#") -> *this == L"abc#de##h"
	void ReplaceAllBlocks(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, const MkStr& replaceBlock);

	// �ּ� ����. ���� �ڸ�Ʈ("//" ~ line feed)�� ûũ �ڸ�Ʈ("/*" ~ "*/")�� ���� ��
	// (NOTE) ���� ����
	// (NOTE) protocol ó���� ���� ���ĺ������� ���� L"://" �� �ּ����� �ν����� �ʰ� ������
	void RemoveAllComment(void);

	//------------------------------------------------------------------------------------------------//
	// ��ūȭ
	//------------------------------------------------------------------------------------------------//

	// ������ separator�� �������� ���ڿ��� ��ū��� �ڸ�
	// separator�� �������� ���� ��� �ڽ� ����
	// (out) tokens : �и��� ��ū���� ��� MkArray container
	// (in) separator : ������
	// (in) ignoreEmptyToken : true�� ��� �����ڿ� ��ū ����(ex> separator�� ������ ���� ��� ��)
	// return : ��ū ���� ��ȯ
	// ex> (L";12;34;;;5;678;;9;")
	//		.Tokenize(tokens, L";", true) -> tokens == L"12", L"34", L"5", L"678", L"9"
	//		.Tokenize(tokens, L";", false) -> tokens == L"", L"12", L"34", L"", L"", L"5", L"678", L"", L"9", L""
	unsigned int Tokenize(MkArray<MkStr>& tokens, const MkStr& separator, bool ignoreEmptyToken = true) const;

	// ������ separator ����Ʈ�� �������� ���ڿ��� ��ū��� �ڸ�
	// separator�� �������� ���� ��� �ڽ� ����
	// (out) tokens : �и��� ��ū���� ��� MkArray container
	// (in) separators : ������ ����Ʈ
	// (in) ignoreEmptyToken : true�� ��� �����ڿ� ��ū ����(ex> separator�� ������ ���� ��� ��)
	// return : ��ū ���� ��ȯ
	// ex>
	//	MkArray<MkStr> separators;
	//	separators.PushBack(L";");
	//	separators.PushBack(L"=");
	//	separators.PushBack(L"#");
	//	(L"12;34=;#5#678==9;")
	//		.Tokenize(tokens, separators, true) -> tokens == L"12", L"34", L"5", L"678", L"9"
	//		.Tokenize(tokens, separators, false) -> tokens == L"12", L"34", L"", L"", L"5", L"678", L"", L"9", L""
	unsigned int Tokenize(MkArray<MkStr>& tokens, const MkArray<MkStr>& separators, bool ignoreEmptyToken = true) const;

	// ������(tab, space, line feed, return)�� separator�� �����Ͽ� ��ū��� �ڸ�
	// �����ڰ� �������� ���� ��� �ڽ� ����
	// (out) tokens : �и��� ��ū���� ��� MkArray container
	// return : ��ū ���� ��ȯ
	unsigned int Tokenize(MkArray<MkStr>& tokens) const;

	//------------------------------------------------------------------------------------------------//
	// ��ȯ
	//------------------------------------------------------------------------------------------------//

	// �־��� ������ŭ �����κ��� ������
	// (in) count : ������ ���� ��
	void PopFront(unsigned int count);

	// �־��� ������ŭ �ڷκ��� ������
	// (in) count : ������ ���� ��
	void BackSpace(unsigned int count);

	// source�� count��ŭ �ݺ��� ������
	// (in) source : ������ Ű����
	// (in) count : �ݺ�ȸ��
	// ex> (L"ab").AddRepetition(L"12", 2) -> L"ab1212"
	void AddRepetition(const MkStr& source, unsigned int count);

	// ������ ��ġ�� ����
	// (in) position : ��ġ
	// (in) source : ������ Ű����
	// return : ���Ե� source �ٷ� ���� ��ġ
	unsigned int Insert(unsigned int position, const MkStr& source);

	// �빮�ڷ� ��ȯ
	MkStr& ToUpper(void);
	MkStr& ToUpper(unsigned int position);

	// �ҹ��ڷ� ��ȯ
	MkStr& ToLower(void);
	MkStr& ToLower(unsigned int position);

	// bool������ ��ȯ
	// ��ҹ��� ��� ���� "true"�ų� "yes"�� ��� ��, �ƴϸ� ����
	// return : ��ȯ�� bool ��
	bool ToBool(void) const;

	// int������ ��ȯ
	// return : ��ȯ�� int ��
	int ToInteger(void) const;

	// unsigned int������ ��ȯ
	// return : ��ȯ�� unsigned int ��
	unsigned int ToUnsignedInteger(void) const;

	// __int64������ ��ȯ
	// return : ��ȯ�� __int64 ��
	__int64 ToDoubleInteger(void) const;

	// unsigned __int64������ ��ȯ
	// return : ��ȯ�� unsigned __int64 ��
	unsigned __int64 ToDoubleUnsignedInteger(void) const;

	// float���� ��ȯ
	// %�� ����� ��� percentage ���¶� �����ϰ� 100:1�� ������ ��ȯ(100% -> 1.f, 50% -> 0.5f)
	// return : ��ȯ�� float ��
	float ToFloat(void) const;

	// MkInt2�� ��ȯ
	// �����ڴ� �ĸ� ','�� ������, '(', ')' ����
	// return : ��ȯ�� MkInt2 ��
	MkInt2 ToInt2(void) const;

	// MkUInt2�� ��ȯ
	// �����ڴ� �ĸ� ','�� ������, '(', ')' ����
	// return : ��ȯ�� MkUInt2 ��
	MkUInt2 ToUInt2(void) const;

	// MkVec2�� ��ȯ
	// �����ڴ� �ĸ� ','�� ������, '(', ')' ����
	// return : ��ȯ�� MkVec2 ��
	MkVec2 ToVec2(void) const;

	// MkVec3�� ��ȯ
	// �����ڴ� �ĸ� ','�� ������, '(', ')' ����
	// return : ��ȯ�� MkVec3 ��
	MkVec3 ToVec3(void) const;

	//------------------------------------------------------------------------------------------------//
	// �ؽ�Ʈ ���� �����
	//------------------------------------------------------------------------------------------------//

	// ���Ϸκ��� �ؽ�Ʈ�� �о�鿩 �� ���ڿ� �ڿ� �߰�
	// ���ϰ����ڰ� �����ϸ� ���ϰ����ڸ� ���ؼ�, �׷��� ������ ���� ��ο��� �о����
	// (in) filePath : �о���� ����, Ȥ�� root directory ���� ��� ���� ���
	// (in) ignoreComment : �ּ� �ڵ� ���� ����
	// return : ���� ����
	bool ReadTextFile(const MkPathName& filePath, bool ignoreComment = true);

	// �ؽ�Ʈ�� ����� MkByteArray�� �� ���ڿ� �ڿ� �߰�
	// (in) byteArray : �ؽ�Ʈ�� ����� MkByteArray
	// (in) ignoreComment : �ּ� �ڵ� ���� ����
	void ReadTextStream(const MkByteArray& byteArray, bool ignoreComment = true);

	// �� ���ڿ��� ���� ����� �ؽ�Ʈ ���Ϸ� ���
	// (in) filePath : ��� �� ����, Ȥ�� root directory ���� ��� ���� ���
	// (in) overwrite : �̹� ���� �̸��� ������ ������ ��� ������� ����. false �� ��� �ڿ� ������
	// (in) ANSI : ���ڵ� ���. true�� ANSI, false�� UTF-8
	// return : ���� ����
	bool WriteToTextFile(const MkPathName& filePath, bool overwrite = true, bool ANSI = true) const;

	//------------------------------------------------------------------------------------------------//
	// utility
	//------------------------------------------------------------------------------------------------//

	// �� ���������� GetLastError() ��ȣ�� �޼����� ä��
	void GetLastErrorMessage(void);

protected:

	bool _IsBlank(wchar_t character) const;

public:

	MkArray<wchar_t> m_Str;

	// empty
	static const MkStr EMPTY;

	// tab
	static const MkStr TAB;

	// line feed
	static const MkStr LF;

	// carriage return
	static const MkStr CR;

	// space
	static const MkStr SPACE;

	// window style line feed(CR + LF)
	static const MkStr CRLF;

	// text tag
	static const MkStr TAG_CR;
	static const MkStr TAG_LF;
	static const MkStr TAG_CRLF;
};

//------------------------------------------------------------------------------------------------//

#endif
