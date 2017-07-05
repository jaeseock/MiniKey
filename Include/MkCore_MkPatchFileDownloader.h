#pragma once

//------------------------------------------------------------------------------------------------//
// 패킹된 개별 파일 관리자
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDataNode.h"


class MkPatchFileDownloader
{
public:

	enum ePatchState
	{
		eReady = 0,

		eDownloadPatchInfoFile,
		ePurgeDeleteList,
		eFindDownloadTargets,
		eDownloadTargetFiles,
		eUpdateFiles,
		eOptimizeChunk,

		eShowResult
	};

	bool CheckAndDownloadPatchFile(const MkStr& url);

	void Update(void);

	inline ePatchState GetCurrentState(void) const { return m_MainState; }
	inline unsigned int GetMaxProgress(void) const { return m_MaxProgress; }
	inline unsigned int GetCurrentProgress(void) const { return m_CurrentProgress; }

	inline const MkPathName& GetRunFilePath(void) const { return m_RunFilePath; }

	MkPatchFileDownloader();

protected:

	typedef struct __DownloadFileInfo
	{
		MkPathName filePath;
		unsigned int origSize;
		unsigned int compSize;
		unsigned int writtenTime;
		bool alreadyDowned;
		bool updateToRealFile;
	}
	_DownloadFileInfo;

	void _FindFilesToDownload(const MkDataNode& node, const MkPathName& pathOffset, const MkPathName& moduleFileName);

	static unsigned int _CountTotalFiles(const MkDataNode& node);

	//------------------------------------------------------------------------------------------------//

protected:

	ePatchState m_MainState;

	MkStr m_RootURL;
	MkStr m_DataRootURL;
	MkPathName m_TempDataPath;
	MkStr m_ErrorMsg;
	MkPathName m_RunFilePath;

	MkDataNode m_PatchInfoNode;
	MkArray<_DownloadFileInfo> m_DownloadFileInfoList;
	
	unsigned int m_MaxProgress;
	unsigned int m_CurrentProgress;
	MkArray<unsigned int> m_OptimizingTarget;

public:

	// 임시 다운로드 저장소 디렉토리명
	static const MkStr TemporaryRepositoryDirName;
};

//------------------------------------------------------------------------------------------------//
