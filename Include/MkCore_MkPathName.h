#ifndef __MINIKEY_CORE_MKPATHNAME_H__
#define __MINIKEY_CORE_MKPATHNAME_H__

//------------------------------------------------------------------------------------------------//
// MkStr을 베이스로 한 경로명
// - 경로 정보 탐색
// - 경로 수정
// - 파일 처리
// - 시스템 경로
// - 경로 실행
// - 다이얼로그 지원
//
// 상대경로의 경우 절대경로 변환을 위해서는 두가지 기준이 존재
// - module directory : 프로세스가 실행된 경로. 프로세스 실행중 불변
// - working directory : 현재 작업중인 경로. 수시로 변경 가능
// 다수가 손대는 어플의 경우 working directory는 신뢰성이 부족하므로 module directory에
// 사용자 지정된 상대경로를 더한 root directory를 상대경로 처리의 근간으로 삼음
//
//****************************************************************************************************
// (NOTE) 별도의 언급이 없는 한 '해당 경로'는 절대경로, 혹은 root directory 기반 상대경로임!!!
//****************************************************************************************************
//
// 확장자가 없는 파일명과 디렉토리 경로가 혼란의 가능성이 있으므로 규칙을 정해 이를 명확히 함
// - 파일 경로 : 확장자가 존재(L"abc\\test.exe")
// - 디렉토리 경로 : L"\\"로 종료되거나 확장자가 없음(L"abc\\test\\", L"abc\\test"), 혹은 공문자열
//
// 절대/상대 경로 판단 규칙
// - 절대 경로 : 로컬은 드라이브명으로 시작(L"d:\\file\\test.exe"), 네트워크는 L"\\\\"로 시작(L"\\\\patchSrv\\")
// - 상대 경로 : 시작 구분자 없음
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"


class MkDataNode;

class MkPathName : public MkStr
{
public:

	//------------------------------------------------------------------------------------------------//
	// 초기화 : 내부 static 값들 정의
	// 원칙적으로는 lock을 걸어야 하지만(data race 가능), 사용자가 적절히 초기화 해 줄 것이라 기대
	//------------------------------------------------------------------------------------------------//

	// rootPath : 절대, 혹은 module directory 기반 상대 디렉토리 경로. NULL일 경우 module directory와 동일
	// ex> MkPathName::SetUp(L"..\\FileRoot\\");
	static void SetUp(const wchar_t* rootPath = NULL);

	//------------------------------------------------------------------------------------------------//
	// 생성자
	//------------------------------------------------------------------------------------------------//

	MkPathName(void);
	MkPathName(const MkPathName& str);
	MkPathName(const MkStr& str);
	MkPathName(const MkHashStr& str);
	MkPathName(const std::wstring& str);
	MkPathName(const wchar_t* wcharArray);

	//------------------------------------------------------------------------------------------------//
	// 상태 정보
	//------------------------------------------------------------------------------------------------//

	// 실제 경로 존재여부 검사
	// return : 존재여부
	bool CheckAvailable(void) const;

	// 디렉토리 경로 여부 반환
	// return : true면 디렉토리, false면 파일 경로
	bool IsDirectoryPath(void) const;

	// 절대경로 여부 반환
	// return : true면 절대, false면 상대 경로
	bool IsAbsolutePath(void) const;

	// 파일 크기 반환
	// return : 해당 파일의 크기. 디렉토리 경로이거나 파일이 없으면 0 반환
	unsigned int GetFileSize(void) const;

	// 경로에 해당하는 문자열을 반환
	// return : 경로 문자열
	// ex> (L"D:\\test\\test.txt").GetPath() -> L"D:\\test\\"
	// ex> (L"D:\\test\\test").GetPath() -> L"D:\\test\\"
	// ex> (L"test\\test\\").GetPath() -> L"test\\test\\"
	// ex> (L"test.txt").GetPath() -> L""
	MkPathName GetPath(void) const;

	// 파일 경로의 경우 파일명에 해당하는 문자열을 반환
	// (in) includeExtension : 확장자 포함여부
	// return : 파일명 문자열
	// ex> (L"D:\\test\\test.txt").GetFileName(true) -> L"test.txt"
	// ex> (L"D:\\test\\test.txt").GetFileName(false) -> L"test"
	MkPathName GetFileName(bool includeExtension = true) const;

	// 파일 경로의 경우 확장자에 해당하는 문자열을 반환
	// return : 파일 확장자 문자열
	// ex> (L"D:\\test\\test.txt").GetFileExtension() -> L"txt"
	// ex> (L"D:\\test\\test").GetFileExtension() -> L""
	MkStr GetFileExtension(void) const;

	// 경로, 파일이름, 확장자로 구분해 반환
	// (NOTE) 확장자 없이 종료될 경우 마지막이 '\'나 '/'로 종료되면 디렉토리 경로, 아니면 파일명으로 반환됨
	// (out) path : 경로
	// (out) name : 확장자가 제외된 파일명
	// (out) extension : 확장자
	// ex> (L"D:\\test\\test.txt").SplitPath(path, name, extension) -> path == L"D:\\test\\", name == L"test", extension == L"txt"
	// ex> (L"D:\\test\\test").SplitPath(path, name, extension) -> path == L"D:\\test\\", name == L"test", extension == L""
	// ex> (L"test\\test\\").SplitPath(path, name, extension) -> path == L"test\\test", name == L"", extension == L""
	// ex> (L"test\\.txt").SplitPath(path, name, extension) -> path == L"test\\", name == L"", extension == L".txt"
	void SplitPath(MkPathName& path, MkStr& name, MkStr& extension) const;

	//------------------------------------------------------------------------------------------------//
	// 경로 필터 처리용 유틸리티
	//------------------------------------------------------------------------------------------------//

	class Filter
	{
	public:

		bool SetUp(const MkArray<MkPathName>* exceptionFilter, const MkArray<MkPathName>* nameFilter, const MkArray<MkPathName>* extensionFilter, const MkArray<MkStr>* prefixFilter);
		bool CheckAvailable(void) const; // 필터 존재여부
		bool CheckAvailableFile(const MkPathName& relativePath) const; // 해당 파일 경로 유효 여부
		bool CheckAvailableDirectory(const MkPathName& relativePath) const; // 해당 디렉토리 경로 유효 여부
		
		bool GetExceptionFilter(MkArray<MkStr>& buffer) const;
		bool GetNameFilter(MkArray<MkStr>& buffer) const;
		bool GetExtensionFilter(MkArray<MkStr>& buffer) const;
		bool GetPrefixFilter(MkArray<MkStr>& buffer) const;

		void Clear(void);

		Filter(bool result);

	protected:

		static bool _ConvertArrayToHashMap(const MkArray<MkPathName>& source, MkHashMap<MkHashStr, int>& target);
		static bool _ConvertArrayToArray(const MkArray<MkStr>& source, MkArray<MkStr>& target);
		static bool _GetKeyList(const MkHashMap<MkHashStr, int>& source, MkArray<MkStr>& buffer);

	protected:

		MkHashMap<MkHashStr, int> m_ExceptionFilter;
		MkHashMap<MkHashStr, int> m_NameFilter;
		MkHashMap<MkHashStr, int> m_ExtensionFilter;
		MkArray<MkStr> m_PrefixFilter;

		bool m_OnExceptionFilter;
		bool m_OnNameFilter;
		bool m_OnExtensionFilter;
		bool m_OnPrefixFilter;

		bool m_PassingResult;
	};

	class WhiteFilter : public Filter
	{
	public:
		WhiteFilter();
	};

	class BlackFilter : public Filter
	{
	public:
		BlackFilter();
	};

	//------------------------------------------------------------------------------------------------//
	// 실제 파일 검색
	// 디렉토리 경로만 유효. 히든 속성 파일 제외
	// 필터관련 문자열은 대소문자 구별 없음
	// 빈 필터 배열이 지정될 경우 해당 검사는 하지 않음
	//------------------------------------------------------------------------------------------------//

	// 모든 파일 개수 반환
	// (in) includeSubDirectory : 하위 폴더 탐색여부
	// return : 디렉토리 하위 파일 수
	unsigned int GetFileCount(bool includeSubDirectory = true) const;

	// 모든 파일 개수와 크기의 총 합 반환
	// (out) totalFileSize : 디렉토리 하위 모든 파일의 크기 합
	// (in) includeSubDirectory : 하위 폴더 탐색여부
	// return : 디렉토리 하위 파일 수
	unsigned int GetFileCountAndTotalSize(unsigned __int64& totalFileSize, bool includeSubDirectory = true) const;

	// 모든 파일 경로 리스트 반환
	// (out) filePathList : 파일 경로가 담길 버퍼
	// (in) includeSubDirectory : 하위 폴더 탐색여부. 하위 폴더내의 파일명에는 시작 디렉토리부터의 상대 경로가 포함됨
	// (in) includeZeroSizeFile : 파일 크기가 0인 파일도 포함 할 것인지의 여부
	// (in) filter(opt) : 필터링용 객체. NULL이면 필터링 무시
	void GetFileList(MkArray<MkPathName>& filePathList, bool includeSubDirectory = true, bool includeZeroSizeFile = true, const Filter* filter = NULL) const;

	// 필터링을 통과한 파일들만 검색(white filter)
	// (out) filePathList : 파일 경로가 담길 버퍼
	// (in) nameFilter : 통과 될 파일/디렉토리 이름 리스트. 상대경로 포함하며 '\'나 '/'로 종료되면 디렉토리 경로라 인식
	//			(ex> L"Item\\armor\\helmet.dds", L"Item\\weapon\\")
	// (in) extensionFilter : 통과 될 파일 확장자 리스트
	// (in) prefixFilter : 통과 될 파일 접두사 리스트
	// (in) exceptionFilter : 필터링과 상관 없이 예외적으로 제외 될 파일 이름 리스트
	// (in) includeSubDirectory : 하위 폴더 탐색여부. 하위 폴더내의 파일명에는 시작 디렉토리부터의 상대 경로가 포함됨
	void GetWhiteFileList(MkArray<MkPathName>& filePathList, const MkArray<MkPathName>* nameFilter, const MkArray<MkPathName>* extensionFilter,
		const MkArray<MkStr>* prefixFilter, const MkArray<MkPathName>* exceptionFilter, bool includeSubDirectory = true, bool includeZeroSizeFile = true) const;

	// 필터링을 통과하지 못 한 파일들만 검색(black filter)
	// (out) filePathList : 파일 경로가 담길 버퍼
	// (in) nameFilter : 제외 될 파일/디렉토리 이름 리스트. 상대경로 포함하며 '\'나 '/'로로 종료되면 디렉토리 경로라 인식
	//			(ex> L"Item\\armor\\helmet.dds", L"Item\\weapon\\")
	// (in) extensionFilter : 제외 될 파일 확장자 리스트
	// (in) prefixFilter : 제외 될 파일 접두사 리스트
	// (in) exceptionFilter : 필터링과 상관 없이 예외적으로 포함 될 파일 이름 리스트
	// (in) includeSubDirectory : 하위 폴더 탐색여부. 하위 폴더내의 파일명에는 시작 디렉토리부터의 상대 경로가 포함됨
	void GetBlackFileList(MkArray<MkPathName>& filePathList, const MkArray<MkPathName>* nameFilter, const MkArray<MkPathName>* extensionFilter,
		const MkArray<MkStr>* prefixFilter, const MkArray<MkPathName>* exceptionFilter, bool includeSubDirectory = true, bool includeZeroSizeFile = true) const;

	// white filter의 특수한 예로 한 폴더 내에서 "[prefix][index].[extension]" 형태의 파일들만 검색해 <index, 파일 경로> 형태로 반환
	// (out) filePathTable : index와 파일 경로가 담길 버퍼
	// (in) prefix : 대상 접두사. 반드시 존재해야 함
	// (in) extension : 대상 확장자. 반드시 존재해야 함
	// ex> "MAP_01.bin", "MAP_04.bin", "MAP_12.bin" 파일이 존재할 경우 GetIndexFormFileList(..., L"map_", L"bin")을 호출하면,
	//     <1, MAP_01.bin>, <4, MAP_04.bin>, <12, MAP_12.bin>가 담겨져 반환됨
	// (NOTE) 다른 이름이지만 동일 인덱스를 가진 파일들이 존재할 경우(ex> "MAP_01.bin", "MAP_001.bin") 정상동작을 보장하지 못함
	void GetIndexFormFileList
		(MkMap<unsigned int, MkPathName>& filePathTable, const MkStr& prefix, const MkStr& extension, bool includeZeroSizeFile = true) const;

	// 경로에 존재하는 파일의 마지막 수정일시를 2000년 이후 초단위로 변환해 반환
	// unsigned int 범위상 2136.2년까지 유효범위
	// 파일이 없거나 에러 발생하면 0 반환
	unsigned int GetWrittenTime(void) const;

	// GetWrittenTime()으로 얻은 시간을 연/월/일/시/분/초 단위로 변환
	// (NOTE) 2000년 1월 1일 이전의 시간은 2000년 1월 1일로 고정됨
	static void ConvertWrittenTime(unsigned int writtenTime, int& year, int& month, int& day, int& hour, int& min, int& sec);

	// 연/월/일/시/분/초 단위를을 GetWrittenTime() 시간으로 변환
	// (NOTE) 2000년 1월 1일 이전의 시간은 0으로 고정됨
	static unsigned int ConvertWrittenTime(int year, int month, int day, int hour, int min, int sec);

	//------------------------------------------------------------------------------------------------//
	// 경로 수정
	//------------------------------------------------------------------------------------------------//

	// 파일명 수정
	// 경로명에 파일명이 존재하는 경우 name으로 교체
	// return : 성공 여부. 디렉토리 경로의 경우 항상 false
	// ex> (L"D:\\test\\test.exe").ChangeFileName(L"newtest") -> L"D:\\test\\newtest.exe"
	bool ChangeFileName(const MkStr& name);

	// 확장자 수정
	// 경로명에 확장자가 존재하는 경우 extension으로 교체
	// return : 성공 여부. 디렉토리 경로의 경우 항상 false
	// ex> (L"D:\\test\\test.exe").ChangeFileExtension(L"bin") -> L"D:\\test\\test.bin"
	bool ChangeFileExtension(const MkStr& extension);

	// 디렉토리 경로화. 마지막을 검사하여 L"\\" 추가
	// (NOTE) 무조건 경로화 해 버리므로 파일 경로에 잘못 실행하는 경우 주의
	// ex> (L"D:\\test\\test").CheckAndAddBackslash() -> L"D:\\test\\test\\"
	// ex> (L"test\\test\\").CheckAndAddBackslash() -> L"test\\test\\" (변동 없음)
	// ex> (L"test\\test.exe").CheckAndAddBackslash() -> L"test\\test.exe\\", test.exe는 폴더명이 되버림
	void CheckAndAddBackslash(void);

	// 경로 최적화
	// ex> (L"D:\\abc\\test").OptimizePath() -> L"D:\\abc\\test\\"
	// ex> (L"D:\\abc\\test\\..\\..\\.\\test.exe").OptimizePath() -> L"D:\\test.exe"
	// ex> (L"..\\abc\\..\\.\\test").OptimizePath() -> L"..\\test\\"
	// ex> (L".\\test.exe").OptimizePath() -> L"test.exe"
	void OptimizePath(void);

	// path가 상대경로이면 module directory 기준 절대경로로, 절대경로이면 덮어쓴 후 경로 최적화
	// (in) path : 경로명
	// ex> L"D:\\test\\test.exe"에서 실행되었다면 module directory는 L"D:\\test\\"이기때문에
	//     ConvertToModuleBasisAbsolutePath(L"..\\test.txt") -> L"D:\\test.txt"
	//     ConvertToModuleBasisAbsolutePath(L"C:\\abc\\..\\") -> L"C:\\"
	void ConvertToModuleBasisAbsolutePath(const MkPathName& path);

	// module directory와 동일 장치의 절대경로인 경우 module directory 기준 최적화된 상대경로로 변환
	// 상대경로인 경우 최적화만 진행
	// return : 절대경로이지만 module directory와 다른 장치일 경우 false, 아니면 true
	bool ConvertToModuleBasisRelativePath(void);

	// path가 상대경로이면 working directory 기준 절대경로로, 절대경로이면 덮어쓴 후 경로 최적화
	// (in) path : 경로명
	void ConvertToWorkingBasisAbsolutePath(const MkPathName& path);

	// working directory와 동일 장치의 절대경로인 경우 working directory 기준 최적화된 상대경로로 변환
	// 상대경로인 경우 최적화만 진행
	// return : 절대경로이지만 working directory와 다른 장치일 경우 false, 아니면 true
	bool ConvertToWorkingBasisRelativePath(void);

	// path가 상대경로이면 root directory 기준 절대경로로, 절대경로이면 덮어쓴 후 경로 최적화
	// (in) path : 경로명
	void ConvertToRootBasisAbsolutePath(const MkPathName& path);

	// root directory와 동일 장치의 절대경로인 경우 root directory 기준 최적화된 상대경로로 변환
	// 상대경로인 경우 최적화만 진행
	// return : 절대경로이지만 root directory와 다른 장치일 경우 false, 아니면 true
	bool ConvertToRootBasisRelativePath(void);

	//------------------------------------------------------------------------------------------------//
	// 파일 처리
	// 실제 경로상 파일 조작
	//------------------------------------------------------------------------------------------------//

	// 파일 삭제
	// return : 최종적으로 파일이 존재하지 않는지 여부(이미 파일이 없는 상태여도 true)
	//          디렉토리 경로일 경우 false
	bool DeleteCurrentFile(void) const;

	// 디렉토리 삭제
	// 하위 디렉토리, 파일 포함 전부 삭제(read-only, system, hidden 등 속성 상관 없이 전부 삭제)
	// deleteAllFiles : true면 포함된 모든 파일까지 삭제. false면 빈 디렉토리만 삭제
	// return : 최종적으로 디렉토리가 존재하지 않는지 여부(이미 디렉토리가 없는 상태여도 true)
	//          파일 경로일 경우 false
	bool DeleteCurrentDirectory(bool deleteAllFiles = true) const;

	// 최종 경로에 이르기까지의 directory를 체크해 없으면 생성
	// return : 최종적으로 경로가 존재하는지 여부(이미 경로가 존재하는 상태여도 true)
	//          파일 경로일 경우 false
	bool MakeDirectoryPath(void) const;

	// 파일명 변경
	// (in) newFileName : 새 파일명. 중간경로 없는 파일명+확장자 형식
	// return : 최종적으로 파일명에 해당하는 파일 존재여부(파일명 변경 필요가 없어도 true)
	//          파일이 경로에 존재하지 않거나 디렉토리 경로이거나 이름 변경이 실패하면 false
	// ex (L"file\\abc\\test.bin").RenameCurrentFile(L"t_0.bin") -> L"file\\abc\\t_0.bin"
	bool RenameCurrentFile(const MkPathName& newFileName);

	// 파일 복사
	// (in) newPath : 해당 파일이 존재하는 디렉토리를 기준으로 한 상대 혹은 절대 파일/디렉토리 경로
	//                디렉토리 경로의 경우 원본과 동일한 파일명 사용
	// (in) failIfExists : newPath에 해당하는 파일이 이미 존재 할 경우 실패 처리할지 여부
	// return : 성공 여부. 원본 파일이 경로에 존재하지 않거나 복사가 실패하면 false
	// ex (L"file\\abc\\test.bin").CopyCurrentFile(L"..\\kk") -> L"file\\kk\\test.bin"
	// ex (L"file\\abc\\test.bin").CopyCurrentFile(L"..\\kk\\t_0.mmb") -> L"file\\kk\\t_0.mmb"
	bool CopyCurrentFile(const MkPathName& newPath, bool failIfExists = true) const;

	// 파일 이동(cut & paste)
	// (in) newPath : 해당 파일이 존재하는 디렉토리를 기준으로 한 상대 혹은 절대 파일/디렉토리 경로
	//                디렉토리 경로의 경우 원본과 동일한 파일명 사용
	// return : 성공 여부. 원본 파일이 경로에 존재하지 않거나 이동이 실패하면 false
	// ex (L"file\\abc\\test.bin").MoveCurrentFile(L"..\\kk") -> L"file\\kk\\test.bin"
	// ex (L"file\\abc\\test.bin").MoveCurrentFile(L"..\\kk\\t_0.mmb") -> L"file\\kk\\t_0.mmb"
	bool MoveCurrentFile(const MkPathName& newPath) const;

	// 수정 시간 변경
	// (in) writtenTime : 수정 시간. GetWrittenTime() 참조
	// return : 성공 여부
	bool SetWrittenTime(unsigned int writtenTime) const;

	// 수정 시간 변경
	// (in) year, month, day, hour, min, sec : 수정 시간
	// return : 성공 여부
	bool SetWrittenTime(int year, int month, int day, int hour, int min, int sec) const;

	//------------------------------------------------------------------------------------------------//
	// 시스템 경로
	// 개별 instance에 종속되지 않는 global 값이므로 static 접근
	//------------------------------------------------------------------------------------------------//

	// 현재 실행된 어플리케이션 이름을 얻음
	static const MkStr& GetApplicationName(void);

	// 실행파일이 위치한 디렉토리 경로(module directory) 얻기
	// 프로세스가 시작된 이후 불변
	// return : module directory의 절대경로
	// ex> D:\\test\\test.exe에서 실행되었다면, GetModuleDirectory() -> L"D:\\test\\"
	static const MkPathName& GetModuleDirectory(void);

	// 현재 작업 경로(working directory) 얻기
	// (NOTE) 외부코드에 의해 사용자도 모르게 변경 될 수 있으므로(ex> CFileDialog) 신뢰성이 떨어짐
	// return : working directory의 절대경로
	static MkPathName GetWorkingDirectory(void);

	// 작업 경로 설정
	// (in) workingPath : 작업 경로로 지정 될 실제 존재하는 절대 디렉토리 경로
	static void SetWorkingDirectory(const MkPathName& workingDirectory);

	// 유저가 설정한 루트 경로 (root directory) 얻기
	// return : root directory의 절대경로
	static const MkPathName& GetRootDirectory(void);

	// 루트 경로 설정
	// (in) rootPath : 실제 존재하는 절대, 혹은 module directory를 기준으로 한 상대 디렉토리 경로
	static void SetRootDirectory(const MkPathName& rootPath);

	//------------------------------------------------------------------------------------------------//
	// 경로 실행
	//------------------------------------------------------------------------------------------------//

	// 디렉토리 경로일 경우 해당 경로의 폴더 탐색창(explorer.exe) 열기
	void OpenDirectoryInExplorer(void) const;

	// 파일 경로일 경우 shell command로 열기
	// (NOTE) 여는 방식과 연결 프로그램은 클라이언트의 OS 세팅에 따라 다름
	// (in) argument : 전달될 인자
	void OpenFileInVerb(const MkStr& argument = L"") const;

	// 확장자가 존재하는 윈도우 실행파일(.exe) 경로가 담겨있을 경우 실행 명령
	// 프로세스 실행시 working directory를 실행된 파일이 존재하는 경로로 변경
	// (in) processTitle : 생성될 프로세스의 타이틀
	// (in) argument : 전달될 인자
	// return : 실행 성공여부
	bool ExcuteWindowProcess(const MkStr& processTitle, const MkStr& argument = L"") const;

	// 확장자가 존재하는 콘솔 실행파일(.exe) 경로가 담겨있을 경우 실행 명령
	// 프로세스 실행시 working directory를 실행된 파일이 존재하는 경로로 변경
	// (in) processTitle : 생성될 프로세스의 타이틀
	// (in) argument : 전달될 인자
	// return : 실행 성공여부
	bool ExcuteConsoleProcess(const MkStr& processTitle, const MkStr& argument = L"") const;

	//------------------------------------------------------------------------------------------------//
	// 다이얼로그 지원
	//------------------------------------------------------------------------------------------------//

	// 단일 파일 선택 다이얼로그
	// (in) extensionList : 필터링 확장자 목록. 비어 있으면 전체 대상(.*)
	// (in) owner : 다이얼로그 소유 윈도우 핸들
	// return : 저장 성공 여부
	bool GetSingleFilePathFromDialog(HWND owner = NULL); // 전체 확장자 대상(.*)
	bool GetSingleFilePathFromDialog(const MkStr& extension, HWND owner = NULL); // 단일 확장자 대상
	bool GetSingleFilePathFromDialog(const MkArray<MkStr>& extensionList, HWND owner = NULL);

	// 다중 파일 선택 다이얼로그
	// (out) directoryPath : 선택된 파일들이 위치한 디렉토리 경로
	// (out) fileNameList : 선택된 파일 이름 목록
	// (in) extensionList : 필터링 확장자 목록. 비어 있으면 전체 대상(.*)
	// (in) owner : 다이얼로그 소유 윈도우 핸들
	// return : 선택된 파일 수
	static unsigned int GetMultipleFilePathFromDialog
		(MkPathName& directoryPath, MkArray<MkPathName>& fileNameList, HWND owner = NULL); // 전체 확장자 대상(.*)
	static unsigned int GetMultipleFilePathFromDialog
		(MkPathName& directoryPath, MkArray<MkPathName>& fileNameList, const MkStr& extension, HWND owner = NULL); // 단일 확장자 대상
	static unsigned int GetMultipleFilePathFromDialog
		(MkPathName& directoryPath, MkArray<MkPathName>& fileNameList, const MkArray<MkStr>& extensionList, HWND owner = NULL);

	// 파일 저장 경로 반환 다이얼로그
	// (in) extensionList : 필터링 확장자 목록. 비어 있으면 전체 대상(.*)
	// (in) owner : 다이얼로그 소유 윈도우 핸들
	// return : 저장 성공 여부
	// (NOTE) 사용자가 존재하지 않는 파일을 직접 기입 할 수 있으므로 확장자가 존재하면 유효성 검사를 진행
	bool GetSaveFilePathFromDialog(HWND owner = NULL); // 전체 확장자 대상(.*). 유효성 검사 하지 않음
	bool GetSaveFilePathFromDialog(const MkStr& extension, HWND owner = NULL); // 단일 확장자 대상. 유효성 검사
	bool GetSaveFilePathFromDialog(const MkArray<MkStr>& extensionList, HWND owner = NULL); // 유효성 검사

	// 디렉토리 탐색 다이얼로그(SHBrowseForFolder)를 통한 유저 선택 경로 저장
	// (in) msg : 다이얼로그에 출력될 메세지 문자열
	// (in) owner : 다이얼로그 소유 윈도우 핸들
	// (in) initialPath : 기본선택용 절대 디렉토리 경로
	// return : 저장 성공 여부
	bool GetDirectoryPathFromDialog(const MkStr& msg = L"", HWND owner = NULL, const MkPathName& initialPath = L"");

	//------------------------------------------------------------------------------------------------//
	// structure(MkDataNode) 구성 및 지원 함수
	//------------------------------------------------------------------------------------------------//

	// 해당 디렉토리 경로에 속한 모든 파일, 디렉토리 정보를 data node에 출력
	// 크기가 4G가 넘는 단일 파일과 하위에 아무것도 없는 디렉토리는 제외함
	// (out) node : 출력 노드
	// return : 소속 파일이 마지막으로 수정된 시간
	// ex>
	//	MkPathName path = L"D:\\Docs\\";
	//	MkDataNode node;
	//	unsigned int wt = path.ExportSystemStructure(node);
	//	node.SaveToText(MkStr(wt) + L".txt");
	unsigned int ExportSystemStructure(MkDataNode& node) const;

	// node의 하위에 존재하는 디렉토리 key 반환
	static void __GetSubDirectories(const MkDataNode& node, MkArray<MkHashStr>& subDirs);

	// node의 하위에 존재하는 파일 key 반환
	static void __GetSubFiles(const MkDataNode& node, MkArray<MkHashStr>& subFiles);

	// node의 count 증가
	static void __IncreaseDirectoryCount(MkDataNode& node); // KeyDirCount
	static void __IncreaseFileCount(MkDataNode& node); // KeyFileCount

	// 두 file node의 size와 written time이 다른지 비교해 결과 반환
	// 같으면 currFileNode의 size정보를 lastFileNode와 동일하게 맞춤(lastFileNode는 압축 크기 정보 복사)
	static bool __CheckFileDifference(const MkDataNode& lastFileNode, MkDataNode& currFileNode);

	// 파일명을 node name으로 생성해 크기(KeyFileSize), 수정시간(KeyWrittenTime) 기록
	// (out) node : 파일이 존재하는 디렉토리 node
	// (in) compressed : 압축 여부
	// (in) origSize : 원본 파일 크기
	// (in) compSize : compressed가 true일 경우 압축 후 크기
	// (in) writtenTime : 파일 수정시간
	// return : 생성된 노드 포인터
	// ex 0>
	//	Node "28.mp3"
	//	{
	//		uint SZ = 1930819;
	//		uint WT = 534955830;
	//	}
	// ex 1>
	//	Node "datanode_00.bin"
	//	{
	//		uint SZ = // [2]
	//			2080 / 701;
	//		uint WT = 534955830;
	//	}
	MkDataNode* __CreateFileStructureInfo(MkDataNode& node, bool compressed, unsigned int origSize, unsigned int compSize, unsigned int writtenTime);

	//------------------------------------------------------------------------------------------------//

protected:

	// 확장자 위치 반환
	// 0 : 확장자 없음, 1 이상 : 확장자 위치, MKDEF_ARRAY_ERROR : 비정상 경로
	unsigned int _GetExtensionPosition(void) const;

	// basePath를 기준으로 sourcePath를 적용해 갱신
	void _UpdateCurrentPath(const MkPathName& basePath, const MkPathName& sourcePath);

	// basePath와 targetPath가 동일 장치환경 기반인지 판별
	// basePath와 targetPath는 절대 경로일 것
	bool _CheckOnSameDevice(const MkPathName& basePath, const MkPathName& targetPath) const;

	// basePath를 기준으로 한 상대경로로 변환
	// basePath와 다른 장치일 경우 false 반환 
	bool _ConvertToRelativePath(const MkPathName& basePath);

	// copy or move
	bool _CopyOrMoveCurrentFile(const MkPathName& newPath, bool copy, bool failIfExists) const;

	// 프로세스 생성
	bool _ExcuteProcess(const MkStr& processTitle, DWORD flag, const MkStr& argument) const;

	// GetDirectoryPathFromDialog()용 콜백
	// SHBrowseForFolder에서 초기경로 지정을 위해서는 콜백외엔 답이 없다고 한다
	static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM dwData);

	static unsigned int _GetFilePathFromDialog
		(MkPathName& directoryPath, MkArray<MkPathName>& fileNameList, const MkArray<MkStr>& extensionList, HWND owner, bool singleSelection, bool forOpen);

	static void _GetSubNodesByTag(const MkDataNode& node, MkArray<MkHashStr>& subNodes, const MkHashStr& countTag, const MkHashStr& typeTag, bool exist);
	static void _IncreaseUnitCountByTag(MkDataNode& node, const MkHashStr& key);

public:

	// data node 구성용 key
	static const MkHashStr KeyFileCount;
	static const MkHashStr KeyDirCount;
	static const MkHashStr KeyFileSize;
	static const MkHashStr KeyWrittenTime;
};

#endif
