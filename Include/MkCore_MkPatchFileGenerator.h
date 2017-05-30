#pragma once

//------------------------------------------------------------------------------------------------//
// 패킹된 개별 파일 관리자
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDataNode.h"


class MkPatchFileGenerator
{
public:

	inline void SetParentWindowHandle(HWND hWnd) { m_hWnd = hWnd; }

	// launcher file은 res의 root에 위치하여야 함. 따라서 fileName은 경로 없는 순수 파일명이어야 함(ex> "launcher.exe")
	void SetLauncherFileName(const MkPathName& fileName);

	// 실행파일 경로는 상대경로 포함 가능(ex> "bin\\client.exe")
	void SetRunFilePath(const MkPathName& filePath);
	
	bool SetHistoryDirectoryPath(const MkPathName& historyDirPath);
	bool SetPatchRootDirectoryPath(const MkPathName& patchDirPath);
	bool SetUpdatingRootDirectoryPath(const MkPathName& updatingDirPath, const MkArray<MkStr>& destList);

	bool GeneratePatchFiles(const MkPathName& sourceDirPath);

	bool Update(void);

	unsigned int GetTotalPatchFileCount(void) const { return m_TotalPatchFileCount; }
	unsigned int GetDonePatchFileCount(void) const { return m_DonePatchFileCount; }

	static MkStr ConvertWrittenTimeToStr(unsigned int writtenTime);

	static void ConvertFilePathToDownloadable(const MkPathName& filePath, MkPathName& buffer);

	// MkArray::IntersectionTest와는 달리 대소문자를 구별하지 않음
	//	ex> source가 "abc.txt"이고 target이 "abc.TXT"일 경우 동일하다 인식
	// intersection으로 source와 target의 해당 index를 반환
	static void IntersectionTest(const MkArray<MkHashStr>& sources, const MkArray<MkHashStr>& targets,
		MkArray<MkHashStr>& sourceOnly, MkArray<unsigned int>& sourceIntersection, MkArray<unsigned int>& targetIntersection, MkArray<MkHashStr>& targetOnly);

	MkPatchFileGenerator();

protected:

	//------------------------------------------------------------------------------------------------//

	enum ePatchState
	{
		eReady = 0,

		eStartPatchWorks,
		eMakePatchFile,
		eUpdateLauncherInfo,
		eReportPatchResult
	};

	enum eUpdateType
	{
		eUpdateTypeAdd = 0,
		eUpdateTypeRep,
		eUpdateTypeDel,

		eMaxUpdateType
	};

	//------------------------------------------------------------------------------------------------//

	bool _LoadCurrentStructure(const MkPathName& sourceDirPath);
	bool _LoadTargetHistoryFile(unsigned int targetIndex);

	unsigned int _GetPatchFileListByType(const MkHashStr& key, MkArray<MkHashStr>& buffer);

	MkDataNode* _GetSourceFileInfoNode(const MkPathName& filePath);

	static bool _CheckUpdateType(const MkDataNode& updateRoot, const MkHashStr& key, const MkStr& msgPrefix, MkStr& msg);

	static bool _FindDirectoryDifference
		(const MkStr& dirPath, MkArray<MkHashStr>& dirList, const MkDataNode* lastDirNode, MkDataNode* currDirNode, MkDataNode& updateRoot, unsigned int& writtenTime);

	static bool _FindDifferenceBetweenTwoNode
		(const MkStr& dirPath, const MkDataNode* lastDirNode, MkDataNode* currDirNode, MkDataNode& updateRoot, unsigned int& writtenTime);

	static bool _SetAllFilesToUpdate
		(const MkStr& dirPath, const MkDataNode& targetDirNode, const MkArray<MkHashStr>& fileList, MkDataNode& updateRoot,
		const MkHashStr& updateType, bool copySrcData, int purgeState, unsigned int& writtenTime);

	static bool _SetAllDirsToUpdate
		(const MkStr& dirPath, const MkDataNode& targetDirNode, MkDataNode& updateRoot, const MkHashStr& updateType, bool copySrcData, int purgeState, unsigned int& writtenTime);

	//------------------------------------------------------------------------------------------------//

protected:

	// parent HWND
	HWND m_hWnd;

	// launcher path
	MkPathName m_LauncherFileName;

	// run path
	MkPathName m_RunFilePath;

	// history
	MkPathName m_HistoryDirPath;
	MkArray<unsigned int> m_HistoryList; // 내림차순 정렬 된 상태로 저장

	// source data
	MkPathName m_SourcePath;

	// updating patch data
	MkPathName m_UpdatingRootPath;
	MkArray<MkStr> m_UpdatingDestName;
	MkArray<MkPathName> m_UpdatingDestPath; // m_UpdatingRootPath + m_UpdatingDestName[n]
	MkArray<MkPathName> m_UpdatingDataPath; // m_UpdatingDestPath[n] + PatchDataDirName

	// full patch data
	MkPathName m_PatchRootPath;
	MkPathName m_PatchDataPath; // m_PatchRootPath + PatchDataDirName

	// structure & update
	MkDataNode m_CurrentHistory;
	MkDataNode m_LastHistory;
	unsigned int m_UpdateWrittenTime;

	// state
	ePatchState m_MainState;
	eUpdateType m_UpdateType;
	MkArray<MkHashStr> m_PatchFilePathList[eMaxUpdateType];

	unsigned int m_TotalPatchFileCount;
	unsigned int m_DonePatchFileCount;

public:

	// update file 확장자
	static const MkStr DownloadableFileExtension;

	// history data tag
	static const MkStr DataType_PatchHistory;

	// patch info tag
	static const MkStr DataType_PatchInfo;

	// patch root 경로 내 data 보관소
	static const MkPathName PatchDataDirName;

	// node key
	static const MkHashStr KEY_CurrWT;
	static const MkHashStr KEY_StructureNode;
	static const MkHashStr KEY_FileSystemNode;
	static const MkHashStr KEY_PurgeListNode;
	static const MkHashStr KEY_LauncherNode;

	// launcher info file name & key
	static const MkHashStr LauncherFileName;

	// run file key
	static const MkHashStr KEY_RunFilePath;
};

//------------------------------------------------------------------------------------------------//
