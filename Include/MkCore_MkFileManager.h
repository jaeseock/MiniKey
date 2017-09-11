#ifndef __MINIKEY_CORE_MKFILEMANAGER_H__
#define __MINIKEY_CORE_MKFILEMANAGER_H__


//------------------------------------------------------------------------------------------------//
// global instance - file manager
//
// MkFileSystem 기반 패킹 시스템을 포함한 파일 관리
// 사용 편의성을 위해 제한 존재(자유도를 원한다면 MkFileSystem을 사용해 직접 구성)
// - 하나의 어플리케이션은 하나의 패킹 시스템만 사용
// - chunk file들은 root directory에 위치
//   (개발시 root directory 하위에 대상 원본 파일들을 위치시켜 그대로 패킹하면 개발과 사용시 모두
//    동일한 경로의 절대/상대 경로를 갖게 되므로 관리면에서 유리)
// - 최초 풀 패킹, 파일 조작, 추출 등은 예외적인 케이스이므로 지원하지 않고 사용에만 집중
//
// (NOTE) 객체 생성 이전 chunk file들이 존재하는 root directory가 세팅되어 있어야 함(MkPathName::SetUp())
//
// MkFileManager 객체가 존재하지 않을 수도 있으므로 직접 객체를 호출하지 않고 static api를 사용해 간접 접근
//
// 파일 참조시 실제 경로 우선. 실제 경로에 없다면 패킹 시스템을 참조해 반환
// MkFileManager 객체가 존재하지 않을 경우 실제 경로만 대상으로 함
//
// 생성(초기화)/삭제를 제외하면 어떠한 파일/내부값도 변경시키지 않고 읽기만 수행하므로 객체 생성/삭제는
// 단 한번씩, 단 한 스레드에서만 실행된다고 가정하고 thread 이슈는 신경쓰지 않음
//------------------------------------------------------------------------------------------------//
// ex>
//	// 실행 어플리케이션의 루트 디렉토리에 파일 패키징. 한 번만 실행하면 됨
//	MkFileSystem pack;
//	pack.SetChunkSizeGuideline(5);
//	pack.SetPercentageForCompressing(70);
//	pack.SetUpFromOriginalDirectory(L"D:\\Solutions\\CurrentProject\\MiniKey\\Samples\\EmptyConsoleProject\\bin\\Src\\", L"");
//	pack.Clear();
//
// //...
//
//	// 어플리케이션 초기화시 패키징 된 청크로 매니져 초기화
//	new MkFileManager(L"", true);
//
//	// 청크로부터 L"a\\src_2.bmp" 추출 후 L"..\\out\\src_2.bmp" 경로에 기록해 추출 확인
//	MkByteArray buffer;
//	if (MkFileManager::GetFileData(L"a\\src_2.bmp", buffer))
//	{
//		MkInterfaceForFileWriting fwInterface;
//		fwInterface.SetUp(L"..\\out\\src_2.bmp");
//		fwInterface.Write(buffer, MkArraySection(0));
//		buffer.Clear();
//	}
//
//	// 텍스트 파일 추출 확인
//	MkStr str;
//	str.ReadTextFile(L"test_0.txt");
//	str += L"\n\n";
//	str.ReadTextFile(L"test_1.txt", false);
//	str.WriteToTextFile(L"..\\out\\test_merge.txt");
//
//------------------------------------------------------------------------------------------------//


#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkFileSystem.h"


class MkFileManager : public MkSingletonPattern<MkFileManager>
{
public:

	//------------------------------------------------------------------------------------------------//
	// 사용
	//------------------------------------------------------------------------------------------------//

	// 실제 경로 혹은 패키징 시스템 내 파일 존재여부 반환
	// (in) filePath : 절대, 혹은 root directory 기준 상대 파일 경로
	// return : 존재 여부
	static bool CheckAvailable(const MkPathName& filePath);

	// 실제 경로 혹은 패키징 시스템 내의 파일 데이터 반환
	// (in) filePath : 절대, 혹은 root directory 기준 상대 파일 경로
	// (out) buffer : 데이터가 담길 버퍼
	// return : 성공 여부
	// (NOTE) 파일 크기가 0이더라도 파일이 존재하면 true 반환
	static bool GetFileData(const MkPathName& filePath, MkByteArray& buffer);

	// 실제 경로의 파일로 데이터 출력
	// (in) filePath : 절대, 혹은 root directory 기준 상대 파일 경로
	// (in) buffer : 데이터가 담길 버퍼
	// (in) overwrite : 기존 파일이 존재 할 경우 true면 덮어쓰고 false면 뒤에 덧붙힘
	// (in) makeDirectoryPath : 파일까지의 경로가 구성되어 있지 않은 경우 자동 경로 구성 여부
	// return : 성공 여부
	static bool SaveDataToFile(const MkPathName& filePath, const MkByteArray& buffer, bool overwrite = true, bool makeDirectoryPath = true);

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	static bool __GetFileDataInRealPath(const MkPathName& filePath, MkByteArray& buffer);
	
	bool __CheckAvailable(const MkPathName& filePath) const;
	bool __GetOriginalFileData(const MkPathName& filePath, MkByteArray& buffer) const;

	void __PrintSystemInformationToLog(void) const;

	inline MkFileSystem& GetFileSystem(void) { return m_FileSystem; }

	MkFileManager(const MkPathName& workingDirectoryPath, bool loadChunk); // 패킹 시스템 설정 수정을 원할 경우 여기서 수정
	virtual ~MkFileManager() { m_FileSystem.Clear(); }

protected:

	MkFileSystem m_FileSystem;
};

#endif
