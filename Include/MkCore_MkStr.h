#ifndef __MINIKEY_CORE_MKSTR_H__
#define __MINIKEY_CORE_MKSTR_H__

//------------------------------------------------------------------------------------------------//
// MkArray<wchar_t>를 베이스로 한 유니코드 기반 문자열
// - 키워드 포함 및 위치 검사
// - 키워드 검사 후 수정
// - 블록단위 처리
// - 토큰화
// - 변환
// - 텍스트 파일 입출력
//
// OS 기본 설정된 언어외 타 언어를 사용 할 경우 인코딩시 코드페이지를 별도로 설정 해 주던가 UTF-8 사용 권장
//
// char와의 변환은 의도적으로 엄격히 관리(무심결에 혼용하는 행위 방지)하며 사용하더라도
// std::string를 통해 간접적으로 입출력
//
// 이론상으로는 바이트 정렬된 유니코드 처리가 멀티바이트보다 빠른게 정상이고 실제적으로도
// g++로 돌리면 평균적으로 유니코드가 2~3배 정도 더 빠르지만, VS의 문자열 처리 api에서
// 멀티바이트 처리는 어셈으로, 유니코드는 c로 만들다보니 VS에서만큼은 유니코드가 더 느림
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
	// 초기화. 프로그램 시작시 단 한 번만 필요
	// 원칙적으로는 lock을 걸어야 하지만 사용자가 적절히 초기화 해 줄 것이라 기대
	//------------------------------------------------------------------------------------------------//

	static void SetUp(unsigned int codePage = 0); // 0 : OS default

	// 코드 페이지
	static void SetGlobalCodePage(unsigned int codePage);
	static unsigned int GetGlobalCodePage(void);

	// LANGID
	// ex> primary id : LANG_SPANISH, sub id : SUBLANG_SPANISH_MEXICAN
	static unsigned short GetPrimaryLanguageID(void);
	static unsigned short GetSubLanguageID(void);

	//------------------------------------------------------------------------------------------------//
	// 생성자
	// 원칙적으로 char 타입은 배제
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
	// 연산자
	//------------------------------------------------------------------------------------------------//

	// 할당
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
	
	// 비교
	bool operator == (const MkStr& str) const;
	bool operator == (const wchar_t* str) const;
	bool operator != (const MkStr& str) const;
	bool operator != (const wchar_t* str) const;
	bool operator < (const MkStr& str) const;
	bool operator > (const MkStr& str) const;
	bool operator <= (const MkStr& str) const;
	bool operator >= (const MkStr& str) const;

	// 추가
	MkStr operator + (const MkStr& str) const;
	MkStr& operator += (const MkStr& str);

	friend MkStr operator + (const wchar_t* charArray, const MkStr& str)
	{
		MkStr tmp(charArray);
		tmp += str;
		return tmp;
	}

	// 참조
	wchar_t& operator [] (unsigned int offset);
	const wchar_t& operator [] (unsigned int offset) const;

	// 캐스팅
	inline operator const wchar_t*() const { return m_Str.GetPtr(); }

	// XOR
	MkStr operator ^ (const MkStr& keyStr) const;
	MkStr& operator ^= (const MkStr& keyStr);

	//------------------------------------------------------------------------------------------------//
	// 메모리 관리
	//------------------------------------------------------------------------------------------------//

	// 크기 예약
	void Reserve(unsigned int size);

	// 메모리 최적화
	void OptimizeMemory(void);

	// 해제(메모리 남김)
	void Flush(void);

	// 해제(메모리 해제)
	void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// 정보 참조
	//------------------------------------------------------------------------------------------------//

	// 문자열이 비었는지 여부 반환
	bool Empty(void) const;

	// 글자 수 반환
	unsigned int GetSize(void) const;

	// 정수형 문자열인지 여부 반환
	// (NOTE) 공문자 허용하지 않음
	bool IsDigit(void) const;

	// 해당 문자열의 유니코드 대역폭을 조사해 코드 페이지 반환
	// (NOTE) 다국어가 섞여 있는 경우 우선순위가 높은 CP를 반환(ex> 영어, 한자, 일본어가 섞여 있을 경우 일본어로 인식)
	unsigned int GetRepresentativeCodePage(void) const;

	// 포인터 반환
	// std::wstring 형태를 원할 경우 직접할당 가능
	// ex>
	//	MkStr str = L"가나다";
	//	std::wstring tmp = str.GetPtr();
	inline wchar_t* GetPtr(void) { return m_Str.GetPtr(); }
	inline const wchar_t* GetPtr(void) const { return m_Str.GetPtr(); }

	// 글자 참조(== operator[])
	const wchar_t& GetAt(unsigned int offset) const;

	// 마지막 공문자가 제외된 메모리 블록 반환
	MkMemoryBlockDescriptor<wchar_t> GetBodyBlockDescriptor(void) const;

	// 마지막 공문자가 포함된 메모리 블록 반환
	MkMemoryBlockDescriptor<wchar_t> GetWholeBlockDescriptor(void) const;

	// 일정 영역의 메모리 블록 반환
	MkMemoryBlockDescriptor<wchar_t> GetMemoryBlockDescriptor(const MkArraySection& section) const;

	//------------------------------------------------------------------------------------------------//
	// std::string를 통한 char형 컨버팅
	//------------------------------------------------------------------------------------------------//

	// *this <- std::string
	void ImportMultiByteString(const std::string& str);

	// *this -> std::string
	void ExportMultiByteString(std::string& str) const;

	//------------------------------------------------------------------------------------------------//
	// MkByteArray 기호화
	// 1byte(0x00~0xFF)를 L"00"~L"FF"로 바꾸므로 1byte -> 4byte(wchar_t * 2)로 용량 증가
	// 비효율적이지만 이진값을 임시로 문자열 형태로 저장하기 위함
	//------------------------------------------------------------------------------------------------//

	// *this <- MkByteArray
	void ImportByteArray(const MkByteArray& buffer);

	// *this -> MkByteArray
	bool ExportByteArray(MkByteArray& buffer) const;

	//------------------------------------------------------------------------------------------------//
	// 키워드 포함 및 위치 검사
	//------------------------------------------------------------------------------------------------//

	// 특정 문자열 포함여부 반환
	// (in) section : 검사 범위
	// (in) keyword : 검사할 문자열
	// return : 포함 여부
	// ex> (L"abc##def##gh").Exist(L"##") -> true
	bool Exist(const MkStr& keyword) const;
	bool Exist(const MkArraySection& section, const MkStr& keyword) const;

	// 특정 문자열이 지정된 위치에 존재하는지 여부 반환
	// (in) position : source의 검사 위치
	// (in) keyword : 검사할 문자열
	// return : 포함 여부
	// (NOTE) 동일 여부가 아닌 포함 여부임
	// ex> (L"abc##def##gh").Equals(3, L"##") -> true
	// ex> (L"abc####def##gh").Equals(3, L"##") -> true
	// ex> (L"abc#!!#def##gh").Equals(3, L"##") -> false
	bool Equals(unsigned int position, const MkStr& keyword) const;

	// 특정 문자열이 포함된 갯수 반환
	// (in) section : 검사 범위
	// (in) keyword : 검사할 문자열
	// (out) positions : 검색된 위치가 담겨질 버퍼
	// return : 포함 갯수. keyword가 비었을 경우 MKDEF_ARRAY_ERROR 반환
	// ex> (L"abc##def##gh").CountKeyword(L"##") -> 1
	// ex> (L"abc##def##gh").CountKeyword(L"##", positions) -> 2, positions(3, 8)
	unsigned int CountKeyword(const MkStr& keyword) const;
	unsigned int CountKeyword(const MkArraySection& section, const MkStr& keyword) const;
	unsigned int CountKeyword(const MkStr& keyword, MkArray<unsigned int>& positions) const;
	unsigned int CountKeyword(const MkArraySection& section, const MkStr& keyword, MkArray<unsigned int>& positions) const;

	// 특정 단어(공문자로 구분되는 문자열)가 포함된 갯수 반환
	// (in) word : 검사할 단어
	// return : 포함 갯수. 단어가 비었을 경우 MKDEF_ARRAY_ERROR 반환
	// ex> (L"abc ## def ##gh").CountWord(L"##") -> 1, (L"abc ## def ## gh").CountKeyword(L"##") -> 2
	unsigned int CountWord(const MkStr& word) const;

	// prefix 포함 검사
	// (in) prefix : 검사할 문자열
	// return : 포함 여부
	// ex> (L"abc_ka").CheckPrefix(L"abc_") -> true
	bool CheckPrefix(const MkStr& prefix) const;

	// postfix 포함 검사
	// (in) postfix : 검사할 문자열
	// return : 포함 여부
	// ex> (L"ka_abc").CheckPostfix(L"_abc") -> true
	bool CheckPostfix(const MkStr& postfix) const;

	// 최초 검색된 특정 문자열 위치 검사
	// (in) section : 검사 범위
	// (in) keyword : 검사할 문자열
	// return : keyword가 비었거나 포함되지 않았으면 MKDEF_ARRAY_ERROR, 포함되었으면 위치 반환
	// ex> (L"012##5##89").GetFirstKeywordPosition(L"##") -> 3
	unsigned int GetFirstKeywordPosition(const MkStr& keyword) const;
	unsigned int GetFirstKeywordPosition(const MkArraySection& section, const MkStr& keyword) const;

	// position이후 존재하는 최초 유효문자(공문자가 아닌) 위치를 반환
	// (in) position : source의 검사 시작 위치
	// return : 존재하지 않으면 MKDEF_ARRAY_ERROR, 포함되었으면 위치 반환
	unsigned int GetFirstValidPosition(unsigned int position = 0) const;

	// position이후 존재하는 최초 공문자 위치를 반환
	// (in) position : source의 검사 시작 위치
	// return : 존재하지 않으면 MKDEF_ARRAY_ERROR, 포함되었으면 위치 반환
	unsigned int GetFirstBlankPosition(unsigned int position = 0) const;

	// position이후 존재하는 최초 line feed(\n) 위치를 반환
	// (in) position : source의 검사 시작 위치
	// return : 존재하지 않으면 MKDEF_ARRAY_ERROR, 포함되었으면 위치 반환
	unsigned int GetFirstLineFeedPosition(unsigned int position = 0) const;

	// position이후 존재하는 마지막 유효문자(공문자가 아닌) 위치를 반환
	// (in) position : source의 검사 시작 위치
	// return : 존재하지 않으면 MKDEF_ARRAY_ERROR, 포함되었으면 위치 반환
	unsigned int GetLastValidPosition(unsigned int position = 0) const;

	// position이후 문자열의 끝부터 마지막 유효문자까지 존재하는 공문자 갯수를 반환
	// (in) position : source의 검사 시작 위치
	// return : 공문자 갯수
	unsigned int CountBacksideBlank(unsigned int position = 0) const;

	// position이 텍스트의 몇 번째 라인인지 반환
	// (in) position : cursor 위치
	// return : 해당 라인 번호. 0부터 시작. position이 범위를 넘어가면 마지막 라인 번호 반환
	unsigned int GetLineNumber(unsigned int position = 0) const;

	//------------------------------------------------------------------------------------------------//
	// 키워드 검사 후 수정
	//------------------------------------------------------------------------------------------------//

	// keywordFrom를 keywordTo로 치환
	// (in) section : 검사 범위
	// (in) keywordFrom : 치환 대상 문자열
	// (in) keywordTo : 치환될 문자열
	// ex> (L"12abc3abc456").ReplaceKeyword(L"abc", L"@") -> L"12@3@456"
	void ReplaceKeyword(const MkStr& keywordFrom, const MkStr& keywordTo);
	void ReplaceKeyword(const MkArraySection& section, const MkStr& keywordFrom, const MkStr& keywordTo);

	// CRLF를 tag(L"\\r\\n")으로 변환
	void ReplaceCRLFtoTag(void);

	// tag(L"\\r\\n")를 CRLF로 변환
	void ReplaceTagtoCRLF(void);

	// 키워드 제거
	// (in) section : 검사 범위
	// (in) keyword : 제거 대상 문자열
	// ex> (L"12abc3abc456").RemoveKeyword(L"abc") -> L"123456"
	void RemoveKeyword(const MkStr& keyword);
	void RemoveKeyword(const MkArraySection& section, const MkStr& keyword);

	// 모든 블랭크(tab, space, line feed, return)를 삭제
	// (in) section : 검사 범위
	// ex> (L"a \nb  c \nd  ").RemoveBlank() -> L"abcd"
	void RemoveBlank(void);
	void RemoveBlank(const MkArraySection& section);

	// 첫 유효문자까지의 블랭크(tab, space, line feed, return)를 삭제
	// 삭제한 공문자 수 반환
	// ex> (L"  a bc  ").RemoveFrontSideBlank() -> L"a bc  "
	unsigned int RemoveFrontSideBlank(void);

	// 마지막 유효문자 이후의 블랭크(tab, space, line feed, return)를 삭제
	// 삭제한 공문자 수 반환
	// ex> (L"  a bc  ").RemoveRearSideBlank() -> L"  a bc"
	unsigned int RemoveRearSideBlank(void);

	//------------------------------------------------------------------------------------------------//
	// 블록단위 처리
	//------------------------------------------------------------------------------------------------//

	// 부분 문자열 복사반환
	// (in) section : 복사 범위
	// (out) buffer : 할당될 문자열 버퍼
	void GetSubStr(const MkArraySection& section, MkStr& buffer) const;

	// position부터 최초 단어(공문자로 구분되는 유효문자 집합)를 검색해 buffer에 할당
	// (in) position : 검사 시작위치
	// (out) buffer : 할당될 문자열 버퍼
	// return :
	//	- 해당 문자열이 존재하면 단어 직후 위치를 반환
	//	- position이 source의 크기를 넘어가거나 단어가 존재하지 않으면 MKDEF_ARRAY_ERROR 반환
	// ex>
	//	MkStr buffer;
	//	(L"  abc 123").GetFirstWord(0, buffer) -> 5, buffer == L"abc"
	unsigned int GetFirstWord(unsigned int position, MkStr& buffer) const;

	// position부터 endKeyword 까지의 최초 블록을 검색해 endKeyword가 제외된 블록 문자열을 buffer에 할당
	// (in) position : 검사 시작위치
	// (in) endKeyword : 블록 종료 문자열
	// (out) buffer : 할당될 문자열 버퍼
	// return :
	//	- 해당 문자열이 존재하면 endKeyword 직후 위치를 반환
	//	- position이 source의 크기를 넘어가거나 블록이 존재하지 않으면 MKDEF_ARRAY_ERROR 반환
	// ex>
	//	MkStr buffer;
	//	(L"abc #123").GetFirstBlock(1, L"#", buffer) -> 5, buffer == L"bc "
	//	(L"abc{123}de").GetFirstBlock(0, L"{", L"}", buffer) -> 8, buffer == L"123"
	unsigned int GetFirstBlock // position ~ endKeyword
		(unsigned int position, const MkStr& endKeyword, MkStr& buffer) const;

	unsigned int GetFirstBlock // beginKeyword ~ endKeyword
		(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, MkStr& buffer) const;

	// position부터 동일 depth의 beginKeyword 와 endKeyword 사이의 블록을 검색해 키워드가 제외된 블록 위치를 반환
	// (in) position : 검사 시작위치
	// (in) beginKeyword : 블록 시작 문자열
	// (in) endKeyword : 블록 종료 문자열
	// (out) blockBegin : beginKeyword 직후 위치
	// (out) blockEnd : endKeyword 위치
	// return :
	//	- 해당 문자열이 존재하면 endKeyword 직후 위치를 반환
	//	- position이 source의 크기를 넘어가거나 블록이 존재하지 않으면 MKDEF_ARRAY_ERROR 반환
	// ex>
	//	unsigned int begin, end;
	//	(L"abc{12{##}3}de").FindFirstStackPosition(0, L"{", L"}", begin, end) -> 12, begin == 4, end == 11
	unsigned int FindFirstStackPosition
		(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, unsigned int& blockBegin, unsigned int& blockEnd) const;

	unsigned int FindFirstStackPosition // 위치 대신 구간 정보를 반환
		(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, MkArraySection& section) const;

	// position부터 동일 depth의 beginKeyword 와 endKeyword 사이의 블록을 검색해 키워드가 제외된 블록 문자열을 buffer에 할당
	// (in) position : 검사 시작위치
	// (in) beginKeyword : 블록 시작 문자열
	// (in) endKeyword : 블록 종료 문자열
	// (out) buffer : 할당될 문자열 버퍼
	// return :
	//	- 해당 문자열이 존재하면 endKeyword 직후 위치를 반환
	//	- position이 source의 크기를 넘어가거나 블록이 존재하지 않으면 MKDEF_ARRAY_ERROR 반환
	// ex>
	//	MkStr buffer;
	//	(L"abc{12{##}3}de").GetFirstStackBlock(0, L"{", L"}", buffer) -> buffer == L"12{##}3"
	//	동일 샘플로 GetFirstBlock()을 실행하면 buffer == L"12{##" 가 나옴
	unsigned int GetFirstStackBlock(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, MkStr& buffer) const;

	// position부터 beginKeyword 와 endKeyword 사이의 첫번째 블록을 keyword 포함하여 삭제
	// (in) position : 검사 시작위치
	// (in) beginKeyword : 블록 시작 문자열
	// (in) endKeyword : 블록 종료 문자열
	// return : 삭제된 위치. position이 source의 크기를 넘어가거나 매칭 블록이 존재하지 않으면 MKDEF_ARRAY_ERROR 반환
	// ex>
	//	(L"abc{123}de").EraseFirstBlock(0, L"{", L"}") -> *this == L"abcde"
	unsigned int EraseFirstBlock(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword);

	unsigned int EraseFirstBlock // 삭제 전 대상 문자열을 buffer에 할당
		(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, MkStr& buffer);

	// position부터 beginKeyword 와 endKeyword 사이의 모든 블록을 keyword 포함하여 삭제
	// (in) position : 검사 시작위치
	// (in) beginKeyword : 블록 시작 문자열
	// (in) endKeyword : 블록 종료 문자열
	// (NOTE) 많이 느림
	// ex>
	//	(L"abc{123}de{}f{45}").EraseAllBlocks(0, L"{", L"}") -> *this == L"abcdef"
	void EraseAllBlocks(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword);

	// position부터 beginKeyword 와 endKeyword 사이의 첫번째 블록을 keyword 포함하여 replaceBlock으로 대체
	// replaceKeyword가 공문자열일 경우 삭제와 동일
	// (in) position : 검사 시작위치
	// (in) beginKeyword : 블록 시작 문자열
	// (in) endKeyword : 블록 종료 문자열
	// (in) replaceBlock : 치환될 문자열
	// return : 대체된 replaceBlock 바로 다음 위치. position이 source의 크기를 넘어가거나 매칭 블록이 존재하지 않으면 MKDEF_ARRAY_ERROR 반환
	// ex>
	//	치환 : (L"abc{123}de").ReplaceFirstBlock(0, L"{", L"}", L"##") -> *this == L"abc##de"
	//	삭제 : (L"abc{123}de").ReplaceFirstBlock(0, L"{", L"}", MkStr::EMPTY) -> *this == L"abcde"
	unsigned int ReplaceFirstBlock(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, const MkStr& replaceBlock);

	unsigned int ReplaceFirstBlock // 치환 전 대상 문자열을 buffer에 할당
		(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, const MkStr& replaceBlock, MkStr& buffer);

	// position부터 beginKeyword 와 endKeyword 사이의 모든 블록을 keyword 포함하여 replaceBlock으로 대체
	// (in) position : 검사 시작위치
	// (in) beginKeyword : 블록 시작 문자열
	// (in) endKeyword : 블록 종료 문자열
	// (in) replaceBlock : 치환될 문자열
	// (NOTE) 많이 느림
	// ex>
	//	(L"abc{123}de{}{fg}h").ReplaceAllBlocks(0, L"{", L"}", L"#") -> *this == L"abc#de##h"
	void ReplaceAllBlocks(unsigned int position, const MkStr& beginKeyword, const MkStr& endKeyword, const MkStr& replaceBlock);

	// 주석 삭제. 라인 코멘트("//" ~ line feed)와 청크 코멘트("/*" ~ "*/")를 삭제 함
	// (NOTE) 많이 느림
	// (NOTE) protocol 처리를 위해 알파벳다음에 오는 L"://" 는 주석으로 인식하지 않고 보존함
	void RemoveAllComment(void);

	//------------------------------------------------------------------------------------------------//
	// 토큰화
	//------------------------------------------------------------------------------------------------//

	// 정해진 separator를 기준으로 문자열을 토큰들로 자름
	// separator가 존재하지 않을 경우 자신 복사
	// (out) tokens : 분리된 토큰들이 담길 MkArray container
	// (in) separator : 구분자
	// (in) ignoreEmptyToken : true일 경우 공문자열 토큰 무시(ex> separator가 겹쳐져 있을 경우 등)
	// return : 토큰 갯수 반환
	// ex> (L";12;34;;;5;678;;9;")
	//		.Tokenize(tokens, L";", true) -> tokens == L"12", L"34", L"5", L"678", L"9"
	//		.Tokenize(tokens, L";", false) -> tokens == L"", L"12", L"34", L"", L"", L"5", L"678", L"", L"9", L""
	unsigned int Tokenize(MkArray<MkStr>& tokens, const MkStr& separator, bool ignoreEmptyToken = true) const;

	// 정해진 separator 리스트를 기준으로 문자열을 토큰들로 자름
	// separator가 존재하지 않을 경우 자신 복사
	// (out) tokens : 분리된 토큰들이 담길 MkArray container
	// (in) separators : 구분자 리스트
	// (in) ignoreEmptyToken : true일 경우 공문자열 토큰 무시(ex> separator가 겹쳐져 있을 경우 등)
	// return : 토큰 갯수 반환
	// ex>
	//	MkArray<MkStr> separators;
	//	separators.PushBack(L";");
	//	separators.PushBack(L"=");
	//	separators.PushBack(L"#");
	//	(L"12;34=;#5#678==9;")
	//		.Tokenize(tokens, separators, true) -> tokens == L"12", L"34", L"5", L"678", L"9"
	//		.Tokenize(tokens, separators, false) -> tokens == L"12", L"34", L"", L"", L"5", L"678", L"", L"9", L""
	unsigned int Tokenize(MkArray<MkStr>& tokens, const MkArray<MkStr>& separators, bool ignoreEmptyToken = true) const;

	// 공문자(tab, space, line feed, return)를 separator로 지정하여 토큰들로 자름
	// 공문자가 존재하지 않을 경우 자신 복사
	// (out) tokens : 분리된 토큰들이 담길 MkArray container
	// return : 토큰 갯수 반환
	unsigned int Tokenize(MkArray<MkStr>& tokens) const;

	//------------------------------------------------------------------------------------------------//
	// 변환
	//------------------------------------------------------------------------------------------------//

	// 주어진 갯수만큼 앞으로부터 지워줌
	// (in) count : 삭제할 글자 수
	void PopFront(unsigned int count);

	// 주어진 갯수만큼 뒤로부터 지워줌
	// (in) count : 삭제할 글자 수
	void BackSpace(unsigned int count);

	// source를 count만큼 반복해 덧붙힘
	// (in) source : 덧붙힐 키워드
	// (in) count : 반복회수
	// ex> (L"ab").AddRepetition(L"12", 2) -> L"ab1212"
	void AddRepetition(const MkStr& source, unsigned int count);

	// 지정한 위치에 삽입
	// (in) position : 위치
	// (in) source : 덧붙힐 키워드
	// return : 삽입된 source 바로 다음 위치
	unsigned int Insert(unsigned int position, const MkStr& source);

	// 대문자로 변환
	MkStr& ToUpper(void);
	MkStr& ToUpper(unsigned int position);

	// 소문자로 변환
	MkStr& ToLower(void);
	MkStr& ToLower(unsigned int position);

	// bool형으로 변환
	// 대소문자 상관 없이 "true"거나 "yes"일 경우 참, 아니면 거짓
	// return : 변환된 bool 형
	bool ToBool(void) const;

	// int형으로 변환
	// return : 변환된 int 형
	int ToInteger(void) const;

	// unsigned int형으로 변환
	// return : 변환된 unsigned int 형
	unsigned int ToUnsignedInteger(void) const;

	// __int64형으로 변환
	// return : 변환된 __int64 형
	__int64 ToDoubleInteger(void) const;

	// unsigned __int64형으로 변환
	// return : 변환된 unsigned __int64 형
	unsigned __int64 ToDoubleUnsignedInteger(void) const;

	// float으로 변환
	// %로 종결될 경우 percentage 형태라 가정하고 100:1의 비율로 변환(100% -> 1.f, 50% -> 0.5f)
	// return : 변환된 float 형
	float ToFloat(void) const;

	// MkInt2로 변환
	// 구분자는 컴마 ','로 공문자, '(', ')' 무시
	// return : 변환된 MkInt2 형
	MkInt2 ToInt2(void) const;

	// MkUInt2로 변환
	// 구분자는 컴마 ','로 공문자, '(', ')' 무시
	// return : 변환된 MkUInt2 형
	MkUInt2 ToUInt2(void) const;

	// MkVec2로 변환
	// 구분자는 컴마 ','로 공문자, '(', ')' 무시
	// return : 변환된 MkVec2 형
	MkVec2 ToVec2(void) const;

	// MkVec3로 변환
	// 구분자는 컴마 ','로 공문자, '(', ')' 무시
	// return : 변환된 MkVec3 형
	MkVec3 ToVec3(void) const;

	//------------------------------------------------------------------------------------------------//
	// 텍스트 파일 입출력
	//------------------------------------------------------------------------------------------------//

	// 파일로부터 텍스트를 읽어들여 현 문자열 뒤에 추가
	// 파일관리자가 존재하면 파일관리자를 통해서, 그렇지 않으면 직접 경로에서 읽어들임
	// (in) filePath : 읽어들일 절대, 혹은 root directory 기준 상대 파일 경로
	// (in) ignoreComment : 주석 자동 삭제 여부
	// return : 성공 여부
	bool ReadTextFile(const MkPathName& filePath, bool ignoreComment = true);

	// 텍스트가 저장된 MkByteArray를 현 문자열 뒤에 추가
	// (in) byteArray : 텍스트가 저장된 MkByteArray
	// (in) ignoreComment : 주석 자동 삭제 여부
	void ReadTextStream(const MkByteArray& byteArray, bool ignoreComment = true);

	// 현 문자열을 실제 경로의 텍스트 파일로 출력
	// (in) filePath : 출력 할 절대, 혹은 root directory 기준 상대 파일 경로
	// (in) overwrite : 이미 동일 이름의 파일이 존재할 경우 덮어씌울지 여부. false 인 경우 뒤에 덧붙힘
	// (in) ANSI : 인코딩 방식. true면 ANSI, false면 UTF-8
	// return : 성공 여부
	bool WriteToTextFile(const MkPathName& filePath, bool overwrite = true, bool ANSI = true) const;

	//------------------------------------------------------------------------------------------------//
	// utility
	//------------------------------------------------------------------------------------------------//

	// 현 시점에서의 GetLastError() 번호와 메세지로 채움
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
