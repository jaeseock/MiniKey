#pragma once

//---------------------------------------------------------------------------------//
// std::string을 베이스로 한 펑션셋
// MkStr이 문자열 처리에는 더 유리하지만 사용하지 못 할 경우도 있기에 보조한다는 의미
// - 키워드 포함 및 위치 검사
// - 키워드 검사 후 수정
// - 블록단위 처리
// - 토큰화
// - 변환
// - 파일 및 경로 처리
// - std::wstring과의 변환
//
// (NOTE!!!) 멀티바이트기반이므로 다국어 처리시 주의 요망!!!
//---------------------------------------------------------------------------------//

#include <string>
#include <vector>
#include <atlcoll.h>
#include <windef.h>


class MkStringOp
{
public:

	//---------------------------------------------------------------------------------//
	// 키워드 포함 및 위치 검사
	//---------------------------------------------------------------------------------//

	// 특정 문자열이 포함된 갯수 반환
	// (in) source : 검사 대상 문자열
	// (in) keyword : 검사할 문자열
	// return : 포함 갯수
	// ex> MkStringOp::CountKeyword("abc##def##gh", "##") -> 2
	static int CountKeyword(const std::string& source, const std::string& keyword);

	// prefix 포함 검사
	// (in) source : 검사 대상 문자열
	// (in) prefix : 검사할 문자열
	// return : 포함 여부
	// ex> MkStringOp::CheckPrefix("abc_ka", "abc_") -> true
	static bool CheckPrefix(const std::string& source, const std::string& prefix);

	// postfix 포함 검사
	// (in) source : 검사 대상 문자열
	// (in) postfix : 검사할 문자열
	// return : 포함 여부
	// ex> MkStringOp::CheckPostfix("ka_abc", "_abc") -> true
	static bool CheckPostfix(const std::string& source, const std::string& postfix);

	// 최초 검색된 특정 문자열 위치 검사 (앞에서부터 뒤로 탐색)
	// (in) source : 검사 대상 문자열
	// (in) keyword : 검사할 문자열
	// return : 포함되지 않았으면 -1, 포함되었으면 위치(0 이상) 리턴
	// ex> MkStringOp::GetFirstKeywordPosition("012##5##89", "##") -> 3
	static int GetFirstKeywordPosition(const std::string& source, const std::string& keyword);

	// 마지막으로 검색된 특정 문자열 위치 검사 (뒤에서부터 앞으로 탐색)
	// (in) source : 검사 대상 문자열
	// (in) keyword : 검사할 문자열
	// return : 포함되지 않았으면 -1, 포함되었으면 위치(0 이상) 리턴
	// ex> MkStringOp::GetLastKeywordPosition("012##5##89", "##") -> 6
	static int GetLastKeywordPosition(const std::string& source, const std::string& keyword);

	// startPosition이후 존재하는 최초 유효문자(공문자가 아닌) 위치를 반환
	// (in) source : 검사 대상 문자열
	// (in) startPosition : source의 검사 시작 위치
	// return : 포함되지 않았으면 -1, 포함되었으면 위치(0 이상) 리턴
	static int GetFirstValidPosition(const std::string& source, int startPosition = 0);

	// startPosition이후 존재하는 마지막 유효문자(공문자가 아닌) 위치를 반환
	// (in) source : 검사 대상 문자열
	// (in) startPosition : source의 검사 시작 위치
	// return : 포함되지 않았으면 -1, 포함되었으면 위치(0 이상) 리턴
	static int GetLastValidPosition(const std::string& source, int startPosition = 0);

	//---------------------------------------------------------------------------------//
	// 키워드 검사 후 수정
	//---------------------------------------------------------------------------------//

	// keyword_a를 keyword_b로 치환
	// (in/out) source : 검사 대상 문자열
	// (in) keyword_a : 치환 대상 문자열
	// (in) keyword_b : 치환될 문자열
	// (in) startPosition : source의 검사 시작 위치
	// return : 치환된 갯수
	// ex>
	//   tmpBuffer = "12abc3abc456";
	//   MkStringOp::ReplaceKeyword(tmpBuffer, "abc", "@", 0) -> tmpBuffer == "12@3@456"
	static int ReplaceKeyword(std::string& source, const std::string& keyword_a, const std::string& keyword_b, int startPosition = 0);

	// 키워드 제거
	// (in/out) source : 검사 대상 문자열
	// (in) keyword : 제거 대상 문자열
	// (in) startPosition : source의 검사 시작 위치
	// ex>
	//   tmpBuffer = "12abc3abc456";
	//   MkStringOp::RemoveKeyword(tmpBuffer, "abc", 0) -> tmpBuffer == "123456"
	static void RemoveKeyword(std::string& source, const std::string& keyword, int startPosition = 0);

	// 모든 블랭크(tab, space, line feed, return)를 삭제
	// (in/out) source : 검사 대상 문자열
	// (in) startPosition : source의 검사 시작 위치
	static void RemoveBlank(std::string& source, int startPosition = 0);

	// 첫 유효문자까지의 블랭크(tab, space, line feed, return)를 삭제
	// (in/out) source : 검사 대상 문자열
	static void RemoveFrontSideBlank(std::string& source);

	// 마지막 유효문자 이후의 블랭크(tab, space, line feed, return)를 삭제
	// (in/out) source : 검사 대상 문자열
	static void RemoveRearSideBlank(std::string& source);

	//---------------------------------------------------------------------------------//
	// 블록단위 처리
	//---------------------------------------------------------------------------------//

	// startPosition부터 endKeyword 까지의 최초 블록을 검색해 endKeyword가 제외된 블록 문자열을 buffer에 할당
	// (in/out) source : 검사 대상 문자열
	// (in) endKeyword : 블록 종료 문자열
	// (out) buffer : 할당될 문자열 버퍼
	// (in) startPosition : 검사 시작위치
	// (in) cut : buffer에 대상 문자열을 할당 한 뒤, source에서 키워드까지 포함된 해당 블록을 삭제할지의 여부
	// return : 할당 성공여부
	// ex>
	//   std::string tmpBuffer = "abc#123";
	//   std::string buffer;
	//   MkStringOp::GetFirstBlock(tmpBuffer, "#", buffer, 1, true) -> tmpBuffer == "a123", buffer == "bc"
	static bool GetFirstBlock(std::string& source, const std::string& endKeyword, std::string& buffer, int startPosition = 0, bool cut = true);

	// startPosition부터 beginKeyword 와 endKeyword 사이의 최초 블록을 검색해 키워드가 제외된 블록 문자열을 buffer에 할당
	// (in/out) source : 검사 대상 문자열
	// (in) beginKeyword : 블록 시작 문자열
	// (in) endKeyword : 블록 종료 문자열
	// (out) buffer : 할당될 문자열 버퍼
	// (in) startPosition : 검사 시작위치
	// (in) cut : buffer에 대상 문자열을 할당 한 뒤, source에서 키워드까지 포함된 해당 블록을 삭제할지의 여부
	// return :
	//	- 해당 문자열이 존재하고 cut이 false면 beginKeyword 직후 위치를 반환
	//	- 해당 문자열이 존재하고 cut이 true면 이전 beginKeyword 시작위치를 반환
	//	- startPosition이 source의 크기를 넘어가거나 블록이 존재하지 않으면 -1 리턴
	// ex>
	//   std::string tmpBuffer = "abc{123}de";
	//   std::string buffer;
	//   MkStringOp::GetFirstBlock(tmpBuffer, "{", "}", buffer, 0, true) -> tmpBuffer == "abcde", buffer == "123"
	static int GetFirstBlock(std::string& source, const std::string& beginKeyword, const std::string& endKeyword, std::string& buffer, int startPosition = 0, bool cut = true);

	// startPosition부터 동일 depth의 beginKeyword 와 endKeyword 사이의 블록을 검색해 키워드가 제외된 블록 문자열을 buffer에 할당
	// (in/out) source : 검사 대상 문자열
	// (in) beginKeyword : 블록 시작 문자열
	// (in) endKeyword : 블록 종료 문자열
	// (out) buffer : 할당될 문자열 버퍼
	// (in) startPosition : 검사 시작위치
	// (in) cut : buffer에 대상 문자열을 할당 한 뒤, source에서 키워드까지 포함된 해당 블록을 삭제할지의 여부
	// return :
	//	- 해당 문자열이 존재하고 cut이 false면 beginKeyword 직후 위치를 반환
	//	- 해당 문자열이 존재하고 cut이 true면 이전 beginKeyword 시작위치를 반환
	//	- startPosition이 source의 크기를 넘어가거나 블록이 존재하지 않으면 -1 리턴
	// ex>
	//   std::string tmpBuffer = "abc{12{##}3}de";
	//   std::string buffer;
	//   MkStringOp::GetFirstStackBlock(tmpBuffer, "{", "}", buffer, 0, true) -> tmpBuffer == "abcde", buffer == "12{##}3"
	//   (NOTE!!!) 동일 샘플로 GetFirstBlock()을 실행하면, tmpBuffer == "abc3}de", buffer == "12{##" 가 나옴
	static int GetFirstStackBlock(std::string& source, const std::string& beginKeyword, const std::string& endKeyword, std::string& buffer, int startPosition = 0, bool cut = true);

	// startPosition부터 beginKeyword 와 endKeyword 사이의 블록을 keyword 포함하여 replaceBlock으로 대체
	// replaceKeyword가 공문자열일 경우 삭제와 동일
	// (in/out) source : 검사 대상 문자열
	// (in) beginKeyword : 블록 시작 문자열
	// (in) endKeyword : 블록 종료 문자열
	// (in) replaceBlock : 치환될 문자열
	// (in) startPosition : 검사 시작위치
	// return : 치환 성공 여부
	// ex>
	//   std::string tmpBuffer = "abc{123}de";
	//   치환 : ReplaceBlock(tmpBuffer, "{", "}", "##", 0) -> tmpBuffer == "abc##de"
	//   삭제 : ReplaceBlock(tmpBuffer, "{", "}", "", 0) -> tmpBuffer == "abcde"
	static bool ReplaceBlock(std::string& source, const std::string& beginKeyword, const std::string& endKeyword, const std::string& replaceBlock, int startPosition = 0);

	// 주석 삭제. 라인 코멘트("//" ~ line feed)와 청크 코멘트("/*" ~ "*/")를 삭제 함
	// (in/out) source : 검사 대상 문자열
	static void RemoveAllComment(std::string& source);

	//---------------------------------------------------------------------------------//
	// 토큰화
	//---------------------------------------------------------------------------------//

	// 정해진 separator를 기준으로 문자열을 토큰들로 자름
	// separator가 존재하지 않을 경우 자신 복사
	// (in) source : 검사 대상 문자열
	// (out) tokens : 분리된 토큰들이 담길 vector container
	// (in) separator : 구분자
	// return : 토큰 갯수 반환
	// ex>
	//   std::string tmpBuffer = "12;34;;;5;678;;9";
	//   MkStringOp::Tokenize(tmpBuffer, tokens, ";") -> tokens == "12", "34", "5", "678", "9"
	static int Tokenize(const std::string& source, std::vector<std::string>& tokens, const std::string& separator);

	// 공문자(tab, space, line feed, return)를 separator로 지정하여 토큰들로 자름
	// 공문자가 존재하지 않을 경우 자신 복사
	// (in) source : 검사 대상 문자열
	// (out) tokens : 분리된 토큰들이 담길 vector container
	// return : 토큰 갯수 반환
	static int Tokenize(const std::string& source, std::vector<std::string>& tokens);

	//---------------------------------------------------------------------------------//
	// 변환
	//---------------------------------------------------------------------------------//

	// 주어진 갯수만큼 앞으로부터 지워줌
	// (in/out) source : 검사 대상 문자열
	// (in) count : 삭제할 글자 수
	static void PopFront(std::string& source, int count);

	// 주어진 갯수만큼 뒤로부터 지워줌
	// (in/out) source : 검사 대상 문자열
	// (in) count : 삭제할 글자 수
	static void BackSpace(std::string& source, int count);

	// 대문자로 변환
	// (in/out) source : 검사 대상 문자열
	static void ToUpper(std::string& source);

	// 소문자로 변환
	// (in/out) source : 검사 대상 문자열
	static void ToLower(std::string& source);

	// bool형으로 변환
	// 대소문자 상관 없이 "true"거나 "yes"일 경우 참, 아니면 거짓
	// (in) source : 검사 대상 문자열
	// return : 변환된 bool 형
	static bool ToBool(std::string& source);

	// int형으로 변환
	// (in) source : 검사 대상 문자열
	// return : 변환된 int 형
	static int ToInteger(std::string& source);

	// float으로 변환
	// %로 종결될 경우 percentage 형태라 가정하고 100:1의 비율로 변환(100% -> 1.f, 50% -> 0.5f)
	// (in) source : 검사 대상 문자열
	// return : 변환된 float 형
	static float ToFloat(std::string& source);

	// std::string으로 변환
	// (in) source : 검사 대상 값
	// return : 변환된 문자열
	static std::string ToString(bool source);
	static std::string ToString(int source);
	static std::string ToString(float source);
	
	//---------------------------------------------------------------------------------//
	// 파일 및 경로 처리
	//---------------------------------------------------------------------------------//

	// 파일 존재여부 검사
	// (in) filePath : 검사할 파일
	static bool CheckFileEnable(const char* filePath);

	// 파일 경로의 경우 확장자에 해당하는 문자열을 리턴 (마지막 dot 이후의 문자열)
	// (in) source : 검사 대상 문자열
	// return : 확장자 문자열
	// ex> MkStringOp::GetFileExtension("D:\\test\\test.txt") -> "txt"
	// (NOTE!!!) source의 형태에 따라 확장자가 없는 경우도 있으므로 실행 후 반드시 empty() 체크 할 것
	static std::string GetFileExtension(const std::string& source);

	// 디렉토리 탐색 다이얼로그(SHGetPathFromIDList)
	// (in) msg : 다이얼로그에 출력될 메세지 문자열
	// (in) initialPath : 기본선택 경로
	// ex> std::string path = MkStringOp::GetDirectoryPathFromDialog(NULL, "오우야", "D:\\Solutions\\MiniKey\\Include");
	// return : 지정된 경로. 취소될 경우 공문자열
	static std::string GetDirectoryPathFromDialog(HWND owner, const std::string& msg, const std::string& initialPath = "");

	// 파일 경로의 경우 경로, 이름, 확장자의 세 부분으로 분리해 리턴
	// (in) source : 검사 대상 문자열
	// (out) path : 경로가 담길 버퍼 (\ 포함)
	// (out) name : 이름이 담길 버퍼
	// (out) extension : 확장자가 담길 버퍼
	// ex>
	//   std::string path, name, extension;
	//   MkStringOp::GetPathInformation("D:\\test\\test.txt", path, name, extension) -> "D:\test\test\", "test", "txt"
	// (NOTE!!!) source의 형태에 따라 실패 가능성이 존재하므로 실행 후 반드시 유효성 체크 할 것
	static void GetPathInformation(const std::string& source, std::string& path, std::string& name, std::string& extension);

	// 파일로부터 텍스트를 읽어들여 삽입
	// (out) target : 내용이 할당될 문자열
	// (in) filePath : 읽어들일 파일
	// (in) ignoreComment : 주석 자동 삭제 여부
	// (in) position : 파일 내용을 삽입할 위치. 음수이거나 범위를 넘어갈 경우 마지막에 추가함
	// return : op. 성공 여부
	static bool AttachFromTextFile(std::string& target, const char* filePath, bool ignoreComment = true, int position = -1);

	// 텍스트를 파일로 출력
	// (in) source : 출력 내용이 담긴 문자열
	// (in) filePath : 출력될 파일 경로
	// (in) overwrite : 이미 동일 이름의 파일이 존재할 경우 덮어씌울지 여부. false 인 경우 뒤에 덧붙힘
	// return : op. 성공 여부
	static bool WriteToTextFile(const std::string& source, const char* filePath, bool overwrite = true);

	//---------------------------------------------------------------------------------//
	// wstring과의 변환
	//---------------------------------------------------------------------------------//

	// multibyte -> wide string
	static void ConvertString(const std::string& source, std::wstring& buffer, unsigned int codePage = CP_ACP);

	// wide string -> multibyte
	static void ConvertString(const std::wstring& source, std::string& buffer, unsigned int codePage = CP_ACP);

	//---------------------------------------------------------------------------------//

protected:

	// GetDirectoryPathFromDialog()용 콜백. SHBrowseForFolder에서 초기경로 지정을 위해서는 콜백외엔 답이 없다고 한다. M$를 깝시다.
	static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM dwData);
};

