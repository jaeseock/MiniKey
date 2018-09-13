#ifndef __MINIKEY_CORE_MKFILESYSTEM_H__
#define __MINIKEY_CORE_MKFILESYSTEM_H__


//------------------------------------------------------------------------------------------------//
// 파일 패킹 시스템 관리자
// 다수의 original 파일을 소수의 chunk로 묶어주기만 해도 로딩시 시간 이득 발생
// 파일은 압축률을 고려해 압축/비압축 상태로 저장됨
//
// 하나의 MkFileSystem은 하나의 파일 시스템을 관리. 어플리케이션 대부분이 하나만 사용해도 충분
//
// 구성된 청크 파일에 대한 삭제/추가시 기존 블록을 disable시키고 끝부분에 추가하는 식으로 청크 파일 수정량을 최소화
// 어느 시점에서 disable 부분이 일정량을 넘어갈 경우 청크 파일 자체를 최적화해 재구성 할 수 있음
//
// 청크 파일 뒤에 데이터 기록시 오류 발생하면 이후 재초기화시 오류 블록은 자동 삭제됨
//
// 하나의 청크 파일이 크면 참조 효율은 높아지지만 최적화시 파일 쓰기 비용이 커짐
// 반대로 작게 유지하면 패키징의 이점이 사라지므로 적절한 파일 크기 설정 필요
//
// 패킹된 파일에 대한 접근은 상대경로로 이루어짐
// ex>
//	L"D:\\OriginalRoot\\local\\map\\test.dds" 파일이 SetUpFromOriginalDirectory(L"D:\\OriginalRoot", ...)로
//	패킹되었으면 이후 이 파일에 대한 시스템 상 경로는 L"local\\map\\test.dds" 가 됨
//	-> GetOriginalFileData(L"local\\map\\test.dds", buffer);
//	-> ExtractAvailableFile(L"local\\map\\test.dds", L"..\\out_0\\");
//
//	만약 실행중인 어플의 root directory가 L"C:\\applications\\clock\\FileRoot\\"인 상태에서 filePath가
//	L"C:\\applications\\clock\\FileRoot\\local\\map\\test.dds"처럼 절대경로로 들어왔다면,
//	내부적으로 현 root directory 기준 상대 경로(L"local\\map\\test.dds")로 변환해 사용
//
// 따라서 가장 편한 형태의 사용은 패킹 대상 파일들을 root directory 하위에 놓고 개발한 다음
// root directory를 그대로 패킹, 이후 다른 어플리케이션에서 청크 파일들을 root directory에 놓고
// 파일 시스템을 구성해 사용 하면 개발과 사용시 모두 동일한 경로의 절대/상대 경로 혼용 가능
//------------------------------------------------------------------------------------------------//


#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkFileChunk.h"


class MkFilePathListContainer;
class MkDataNode;

class MkFileSystem
{
public:

	//------------------------------------------------------------------------------------------------//
	// 초기 설정
	//------------------------------------------------------------------------------------------------//

	// 하나의 청크에 대한 크기 설정(MB 단위 : 1 ~ 4095)
	// 가이드라인이므로 엄격한 규제는 아님(보통은 살짝 넘어감)
	void SetChunkSizeGuideline(unsigned int chunkSizeInMB = 512);

	// 압축 경계 지정(% 단위 : 0 ~ 100)
	// 압축률(압축 후 크기/원본 크기)이 좋지 않다면 원본 유지가 오히려 유리하기때문에 압축 경계 이하일 경우만 압축해 저장
	// 0이면 전혀 압축을 하지 않고 100 이상이면 대부분 압축(드물게 압축후 용량이 약간 늘어나는 경우도 존재)
	// 감소율이 좋은 단색조 bmp가 20~40% 정도고 이미 압축되어 있는 jpg, mp3 등은 90~100% 정도이므로 적절하게 설정
	void SetPercentageForCompressing(unsigned int percentageForCompressing = 70);

	// 청크 파일명 규칙 설정
	// ex> prefix == L"MK_CHUNK_SYSTEM_", extension == L"mcs" -> "MK_CHUNK_SYSTEM_0.mcs", "MK_CHUNK_SYSTEM_1.mcs", "MK_CHUNK_SYSTEM_2.mcs", ...
	void SetChunkFileNamingRule(const MkStr& prefix = L"MK_CHUNK_SYSTEM_", const MkStr& extension = L"mcs");

	// SetUpFromOriginalDirectory(), UpdateFromOriginalDirectory() 호출시 적용 될 필터링 규칙
	// 세부 항목에 대해서는 MkPathName::GetBlackFileList() 참조
	void SetFilterForOriginalFiles
		(const MkArray<MkPathName>* exceptionFilter, const MkArray<MkPathName>* nameFilter, const MkArray<MkPathName>* extensionFilter, const MkArray<MkStr>* prefixFilter);

	// 주어진 시스템 정보 data node(파일)을 사용해 설정
	// (in) node/settingFilePath : 시스템 정보 data node/파일의 절대, 혹은 root directory 기준 상대 디렉토리 경로
	// return : 성공 여부
	// node ex>
	//	uint ChunkSizeInMB = 512;
	//	uint PercentageForCompressing = 70;
	//	str Prefix = "MK_CHUNK_SYSTEM_";
	//	str Extension = "mcs";
	//	str NameFilter = "datanode_00.bin" / "abc\src_2.bmp" / "def/";
	//	str ExtensionFilter = "exe" / "txt";
	//	//str PrefixFilter = "";
	//	str ExceptionFilter = "test_0.txt";
	void SetSystemSetting(const MkDataNode& node);
	bool SetSystemSetting(const MkPathName& settingFilePath);
	bool SetSystemSetting(void); // root directory의 DefaultSettingFileName 파일을 사용하여 로드

	//------------------------------------------------------------------------------------------------//
	// 초기화
	//------------------------------------------------------------------------------------------------//

	// 최초 패키징
	// 원본 디렉토리의 하위 파일들로부터 pack을 구성하고 청크 파일들을 생성
	// 대상 파일 검색은 black filter 사용
	// 파일 크기가 0인 파일은 포함시키지 않음
	// (NOTE) SetFilterForOriginalFiles()에서 설정된 필터링 규칙 적용 받음
	// (in) absolutePathOfBaseDirectory : 패킹 할 원본 디렉토리 절대경로
	// (in) workingDirectoryPath : 청크 파일들을 생성할 절대, 혹은 root directory 기준 상대 디렉토리 경로
	// return : 성공 여부
	bool SetUpFromOriginalDirectory(const MkPathName& absolutePathOfBaseDirectory, const MkPathName& workingDirectoryPath);

	// 설정된 시스템 정보를 사용해 패키징 된 청크 파일들로부터 구성
	// (in) workingDirectoryPath : 청크 파일들이 존재하는 절대, 혹은 root directory 기준 상대 디렉토리 경로
	// return : 성공 여부
	bool SetUpFromChunkFiles(const MkPathName& workingDirectoryPath);

	//------------------------------------------------------------------------------------------------//
	// 참조
	//------------------------------------------------------------------------------------------------//

	// 등록된 모든 청크 수
	inline unsigned int GetTotalChunkCount(void) const { return m_ChunkTable.GetSize(); }

	// 등록된 모든 파일 수
	inline unsigned int GetTotalFileCount(void) const { return m_SearchTable.GetSize(); }

	// 청크 파일 별 무효 크기 비중 반환
	// (out) buffer : <청크 파일 명, 무효 크기 비중 퍼센트> 형태의 테이블
	// return : 청크 존재여부
	bool GetBlankPercentage(MkMap<MkStr, unsigned int>& buffer) const;

	// 청크 파일 별 무효 크기 비중을 blankPercentage와 비교해 초과하는 청크 인덱스 리스트를 반환
	// (in) blankPercentage : 청크파일의 빈 공간 비율
	// return : 대상 청크 존재여부
	bool GetBlankChunks(unsigned int blankPercentage, MkArray<unsigned int>& chunkIndice) const;

	// 해당 경로명을 가진 파일의 패키징 내 존재여부 반환
	// (in) filePath : 파일 경로. 상대경로면 그대로, 절대경로면 현 root directory 기준 상대경로로 변환
	// return : 존재여부
	bool CheckAvailable(const MkPathName& filePath) const;

	// 해당 경로명이 세팅된 파일 필터를 통과하는지 여부(패킹되도록 지정된 파일인지) 반환
	// (in) filePath : 파일/디렉토리 경로. 상대경로면 그대로, 절대경로면 현 root directory 기준 상대경로로 변환
	// return : 통과여부
	bool CheckFileFilter(const MkPathName& fileOrDirPath) const;

	// 해당 경로명을 가진 파일의 패키징 내 정보 반환
	// (in) filePath : 파일 경로. 상대경로면 그대로, 절대경로면 현 root directory 기준 상대경로로 변환
	// (out) originalSize : 파일의 원본 크기
	// (out) dataSize : 파일의 실제 크기. originalSize과 동일하면 압축되지 않은 상태임을 의미
	// (out) writtenTime : 파일의 수정 일시
	// return : 존재여부
	bool GetFileInfo(const MkPathName& filePath, unsigned int& originalSize, unsigned int& dataSize, unsigned int& writtenTime) const;

	// 해당 경로명을 가진 파일의 패키징 내 정보와 동일한지 여부 반환
	// (in) filePath : 파일 경로. 상대경로면 그대로, 절대경로면 현 root directory 기준 상대경로로 변환
	// (in) originalSize : 파일의 원본 크기
	// (in) writtenTime : 파일의 수정 일시
	// return : 파일이 없으면 -1, 존재하고 동일하면 0, 존재하지만 다르면 1 반환
	int GetFileDifference(const MkPathName& filePath, unsigned int originalSize, unsigned int writtenTime) const;

	// 패키징 된 유효한 파일 원본 데이터를 buffer에 담아 반환
	// (in) filePath : 파일 경로. 상대경로면 그대로, 절대경로면 현 root directory 기준 상대경로로 변환
	// (out) buffer : 데이터가 담길 버퍼
	// return : 성공여부
	bool GetOriginalFileData(const MkPathName& filePath, MkByteArray& buffer) const;

	// 패키징 된 유효한 파일을 원본 파일로 복원
	// (in) filePath : 파일 경로. 상대경로면 그대로, 절대경로면 현 root directory 기준 상대경로로 변환
	// (in) destinationDirectoryPath : 복원될 절대, 혹은 root directory 기준 상대 디렉토리 경로
	// return : 성공 여부
	bool ExtractAvailableFile(const MkPathName& filePath, const MkPathName& destinationDirectoryPath) const;

	// 패키징 된 모든 유효 파일들을 원본 파일로 복원
	// (in) destinationDirectoryPath : 복원될 절대, 혹은 root directory 기준 상대 디렉토리 경로
	// return : 성공 여부
	bool ExtractAllAvailableFiles(const MkPathName& destinationDirectoryPath) const;

	// 현재 시스템 정보를 MkDevPanel에 출력
	// (NOTE) 호출 시점은 당연하게도 MkDevPabnel 생성 이후에 가능
	void PrintSystemInfoToDevPanel(bool printFilterInfo = false) const;

	// MkDataNode에 폴더와 파일 정보 출력
	// (out) node : 출력 될 MkDataNode
	// (in) includeChunkInfo : 파일별로 소속 chunk와 block index정보도 포함시킬지 여부
	void ExportSystemStructure(MkDataNode& node, bool includeChunkInfo = false) const;

	//------------------------------------------------------------------------------------------------//
	// 수정
	//------------------------------------------------------------------------------------------------//

	// 패킹 시스템에서 파일 제거
	// 실제 청크 파일에서 제거되지는 않고 상태만 disable로 변경
	// (in) filePath : 파일 경로. 상대경로면 그대로, 절대경로면 현 root directory 기준 상대경로로 변환
	// return : 성공 여부
	bool RemoveFile(const MkPathName& filePath);

	// 단일 파일 갱신
	// 존재하지 않는 파일이면 추가. 이미 존재하는 파일의 경우 파일 크기가 0일 경우 삭제, 그렇지 않으면 갱신
	// (in) absolutePathOfBaseDirectory : root로 간주 될 원본 디렉토리 절대경로
	// (in) relativePathOfFile : root이후의 파일 상대 경로. 이후 검색의 key(file path)로 사용됨
	// (in) writtenTime : 주어진 파일 수정 시간. 0일 경우 원본 파일의 것을 사용
	// return : 성공 여부
	// ex>
	//	L"c:\\_Tmp\\Text\\test.txt" 파일을 파일 시스템에 L"Text\\test.txt"로 갱신하려 할 경우,
	//	absolutePathOfBaseDirectory : L"c:\\_Tmp\\"
	//	relativePathOfFile : L"Text\\test.txt"
	bool UpdateFromOriginalFile(const MkPathName& absolutePathOfBaseDirectory, const MkPathName& relativePathOfFile, unsigned int writtenTime = 0);

	// 패킹 시스템에 주어진 디렉토리 하위의 모든 파일 갱신(디렉토리 구조대로 갱신 됨)
	// 존재하지 않는 파일이면 추가. 이미 존재하는 파일의 경우 파일 크기가 0일 경우 삭제, 그렇지 않으면 갱신
	// 다수의 파일을 한꺼번에 갱신하고자 할 때 효율적
	// (NOTE) SetFilterForOriginalFiles()에서 설정된 필터링 규칙 적용 받음
	// (in) absolutePathOfBaseDirectory : 갱신 할 원본 디렉토리 절대경로. 경로가 존재하지 않으면 갱신 파일이 없다고 간주
	// (in) removeOrgFilesAfterUpdating : 갱신 완료 후 원본 파일 삭제여부
	// return : 성공 여부
	bool UpdateFromOriginalDirectory(const MkPathName& absolutePathOfBaseDirectory, bool removeOrgFilesAfterUpdating = false);

	// 모든 청크를 대상으로 공극률 검사해 최적화
	// RemoveFile(), UpdateFromOriginalDirectory()의 호출로 청크내 빈 공간이 일정 비율 이상 차지할 경우 파일 최적화 실행
	// (in) percentageForOptimizing : 청크파일의 빈 공간 비율이 주어진 퍼센트 비율 이상일 경우 최적화 실행
	// return : 성공 여부
	// ex> percentageForOptimizing == 30%, chunk_0(25%), chunk_1(34%), chunk_2(30%) 일 경우 chunk_1, chunk_2 최적화 실행
	bool OptimizeAllChunks(unsigned int percentageForOptimizing);

	// 단일 청크 최적화
	// (in) chunkIndex : 해당 청크 인덱스
	// return : 성공 여부
	bool OptimizeChunk(unsigned int chunkIndex);

	// 해제
	void Clear(void);

	MkFileSystem();
	~MkFileSystem() { Clear(); }

protected:

	typedef struct _FileBlockIndex
	{
		unsigned int chunkIndex;
		unsigned int blockIndex;
	}
	FileBlockIndex;

	void _GenerateChunkFilePath(MkPathName& chunkFilePath, unsigned int index) const;

	bool _GetFileBlockIndex(const MkPathName& filePath, FileBlockIndex& fbi) const;

	bool _CreateNewChunkFromOriginalFiles(unsigned int chunkIndex, MkFilePathListContainer& filePathListContainer);

	void _RegisterPathListToSearchTable(const MkArray<MkPathName>& memberFilePathList, unsigned int chunkIndex, unsigned int blockOffset);

	void _ExportSystemStructure(MkDataNode& node, const MkArray<MkStr>& token, unsigned int depth, const FileBlockIndex& fbi, bool includeChunkInfo) const;

	bool _UpdateFilesToAvailableChunk(const MkArray<unsigned int>& availableChunks, MkFilePathListContainer& filePathListContainer);
	bool _UpdateFilesToNewChunk(MkFilePathListContainer& filePathListContainer);

protected:

	// 구성 정보
	unsigned int m_ChunkSizeGuideline;
	unsigned int m_PercentageForCompressing;
	MkStr m_ChunkFilePrefix;
	MkStr m_ChunkFileExtension;
	MkPathName::BlackFilter m_PathFilter;

	// 구성된 청크 파일이 존재하는 절대 디렉토리 경로
	MkPathName m_AbsoluteWorkingDirectoryPath;

	// 청크 테이블. 청크 하나가 청크 파일 하나와 매치됨
	MkMap<unsigned int, MkFileChunk> m_ChunkTable;

	// 검색 테이블. hash map(속도 우선)
	// 비활성 블록(MkFileBlock::eDisable)은 포함되지 않음
	MkHashMap<MkHashStr, FileBlockIndex> m_SearchTable;

public:

	// 파일 시스템 기본 설정파일명
	static const MkPathName DefaultSettingFileName;
};

//------------------------------------------------------------------------------------------------//

#endif
