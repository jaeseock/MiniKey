#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkFtpInterface.h"
#include "MkCore_MkDataNode.h"


class MkPatchFileUploader
{
public:

	bool StartUploading(const MkPathName& targetDirPath, const MkStr& url, const MkStr& remotePath, const MkStr& userName, const MkStr& password, bool passiveMode);

	bool Update(void);

	MkPatchFileUploader();
	~MkPatchFileUploader() { _Clear(); }


protected:

	void _Clear(void);

	void _AddPurgeDirectory(const MkStr& filePath, MkDataNode& purgeDirNode);
	bool _PurgeEmptyDirectory(MkDataNode& purgeDirNode);

	void _BuildUploadCommand(const MkDataNode& structureNode, const MkPathName& currPath);

protected:

	enum eState
	{
		eReady = 0,

		ePurgeFiles,
		ePurgeDirs,
		eUploadingFiles,
		eClearTargetDir
	};

	enum eUploadCommandType
	{
		eMoveToChild = 0,
		eMoveToParent,
		eUploadFile
	};

	typedef struct __UploadCommand
	{
		eUploadCommandType command;
		MkPathName target;
	}
	_UploadCommand;

	eState m_State;

	MkArray<MkPathName> m_PurgeList;
	MkDataNode m_PurgeDirNode;
	
	MkPathName m_UploadDirectoryPath;
	MkArray<_UploadCommand> m_UploadCommand;
	unsigned int m_CurrCommandIndex;
	unsigned int m_CurrCommandCounter;

	MkFtpInterface m_FTP;
};

//------------------------------------------------------------------------------------------------//
