#pragma once

//------------------------------------------------------------------------------------------------//
// cdn���κ��� ��ġ ���� ���� ����
//   - ������ source�� ���� �����ð����� ����
//   - history�� cdn �������� �ʱ�ȭ ��(���� ��ġ�� �� ó�� �ν�)
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDataNode.h"


class MkFileDownInfo;

class MkPatchSrcRecovery
{
public:

	enum eRecoveryState
	{
		eReady = 0,

		eDownloadPatchInfoFile,
		eFindDownloadTargets,
		eRegisterTargetFiles,
		eDownloadTargetFiles,

		eShowSuccessResult,
		eShowFailedResult
	};

	bool RecoverPatchSource(const MkStr& url, const MkPathName& recoveryDirPath, const MkStr& historyDirName, const MkStr& patchDirName, const MkPathName& sourcePath);

	void Update(void);

	inline eRecoveryState GetMainState(void) const { return m_MainState; }
	inline void ResetMainState(void) { m_MainState = eReady; }

	inline const MkDataNode& GetHistoryNode(void) const { return m_PatchInfoNode; }

	MkPatchSrcRecovery();

protected:

	typedef struct __DownloadFileInfo
	{
		MkPathName filePath;
		unsigned int origSize;
		unsigned int compSize;
		unsigned int writtenTime;
	}
	_DownloadFileInfo;

	void _FindFilesToDownload(const MkDataNode& node, const MkPathName& pathOffset);

	static void _ConvertDataSizeToString(unsigned __int64 size, MkStr& buffer);

	//------------------------------------------------------------------------------------------------//

protected:

	eRecoveryState m_MainState;

	MkStr m_RootURL;
	MkStr m_DataRootURL;
	MkPathName m_RecoveryDirPath;
	MkPathName m_HistoryDirPath;
	MkPathName m_PatchDirPath;
	MkPathName m_PatchDataPath;
	MkPathName m_SourceDirPath;
	MkStr m_ErrorMsg;
	
	MkDataNode m_PatchInfoNode;
	MkDataNode* m_UpdateAddNode;
	unsigned int m_CurrentWrittenTime;
	MkArray<_DownloadFileInfo> m_DownloadFileInfoList;

	MkFileDownInfo* m_CurrentDownInfo;

	unsigned int m_MaxProgress;
	unsigned int m_CurrentProgress;
};

//------------------------------------------------------------------------------------------------//
