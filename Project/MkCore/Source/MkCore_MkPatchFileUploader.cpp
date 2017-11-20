
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkPatchFileUploader.h"

//------------------------------------------------------------------------------------------------//

bool MkPatchFileUploader::StartUploading
(const MkPathName& targetDirPath, const MkStr& url, const MkStr& remotePath, const MkStr& userName, const MkStr& password)
{
	if (m_State != eReady)
		return false;

	_Clear();
	
	// target dir 존재여부 검사
	m_UploadDirectoryPath.ConvertToRootBasisAbsolutePath(targetDirPath);
	if (!m_UploadDirectoryPath.CheckAvailable())
	{
		MK_DEV_PANEL.MsgToLog(L"> " + targetDirPath);
		MK_DEV_PANEL.MsgToLog(L"  대상 디렉토리가 존재하지 않습니다.");
		MK_DEV_PANEL.InsertEmptyLine();
		return false;
	}

	// file structure
	MkDataNode uploadNode;
	if (m_UploadDirectoryPath.ExportSystemStructure(uploadNode) == 0)
	{
		MK_DEV_PANEL.MsgToLog(L"> " + targetDirPath);
		MK_DEV_PANEL.MsgToLog(L"  대상 디렉토리에 파일이 없습니다.");
		MK_DEV_PANEL.InsertEmptyLine();
		return false;
	}

	// launcher info 설정파일 검사
	MkStr launcherInfoFileName = MkPatchFileGenerator::LauncherFileName.GetString() + L"." + MkDataNode::DefaultFileExtension; // ex> LauncherFile.mmd
	MkPathName launcherInfoFileDown;
	MkPatchFileGenerator::ConvertFilePathToDownloadable(launcherInfoFileName, launcherInfoFileDown); // ex> LauncherFile.mmd.rp
	if (!uploadNode.ChildExist(launcherInfoFileDown))
	{
		MK_DEV_PANEL.MsgToLog(L"> " + targetDirPath);
		MK_DEV_PANEL.MsgToLog(L"  디렉토리에 " + launcherInfoFileDown + L" 파일이 존재하지 않습니다.");
		MK_DEV_PANEL.InsertEmptyLine();
		return false;
	}
	
	// patch info 설정파일 검사
	MkStr patchInfoFileName = MkPatchFileGenerator::DataType_PatchInfo + L"." + MkDataNode::DefaultFileExtension; // ex> PatchInfo.mmd
	MkPathName patchInfoFileDown;
	MkPatchFileGenerator::ConvertFilePathToDownloadable(patchInfoFileName, patchInfoFileDown); // ex> PatchInfo.mmd.rp
	if (!uploadNode.ChildExist(patchInfoFileDown))
	{
		MK_DEV_PANEL.MsgToLog(L"> " + targetDirPath);
		MK_DEV_PANEL.MsgToLog(L"  디렉토리에 " + patchInfoFileDown + L" 파일이 존재하지 않습니다.");
		MK_DEV_PANEL.InsertEmptyLine();
		return false;
	}

	// purge 정보 구축
	MkDataNode patchInfoNode;
	if (!patchInfoNode.Load(m_UploadDirectoryPath + patchInfoFileDown))
		return false;

	MkDataNode* purgeNode = patchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_PurgeListNode);
	if (purgeNode != NULL)
	{
		MkDataNode purgeDirNode;

		MkArray<MkHashStr> purgeFiles;
		m_PurgeList.Reserve(purgeNode->GetChildNodeList(purgeFiles));

		MK_INDEXING_LOOP(purgeFiles, i)
		{
			MkPathName currPath;
			MkPatchFileGenerator::ConvertFilePathToDownloadable(purgeFiles[i].GetString(), currPath);
			
			// 삭제할 파일 경로 리스트
			m_PurgeList.PushBack(currPath);

			// 디렉토리가 비었으면 같이 삭제하기 위해 별도의 구조를 만듬
			_AddPurgeDirectory(currPath, m_PurgeDirNode);
		}
	}

	// upload command 구축
	m_UploadCommand.Reserve(400000); // 사십만개면 충분하지 않을까...
	_BuildUploadCommand(uploadNode, MkStr::EMPTY);

	unsigned int uploadDirCount = 1; // root
	unsigned int uploadFileCount = 0;
	MK_INDEXING_LOOP(m_UploadCommand, i)
	{
		switch (m_UploadCommand[i].command)
		{
		case eMoveToChild: ++uploadDirCount; break;
		case eUploadFile: ++uploadFileCount; break;
		}
	}
	MK_DEV_PANEL.MsgToLog(L"> " + MkStr(uploadDirCount) + L"개의 디렉토리에 " + MkStr(uploadFileCount) + L"개의 파일 업로드 존재.");
	MK_DEV_PANEL.InsertEmptyLine();

	MkStr msg;
	msg.Reserve(1024 * 1024 * 10);
	msg += m_UploadCommand.GetSize();
	msg += MkStr::LF;
	MK_INDEXING_LOOP(m_UploadCommand, i)
	{
		switch (m_UploadCommand[i].command)
		{
		case eMoveToChild:
			msg += L"MoveToChild : ";
			msg += m_UploadCommand[i].target;
			break;
		case eMoveToParent:
			msg += L"MoveToParent";
			break;
		case eUploadFile:
			msg += L"UploadFile : ";
			msg += m_UploadCommand[i].target;
			break;
		}

		msg += MkStr::LF;
	}
	
	// ftp 연결
	MK_DEV_PANEL.MsgToLog(L"> URL : " + url);
	MK_DEV_PANEL.MsgToLog(L"  RemotePath : " + remotePath);
	MK_DEV_PANEL.MsgToLog(L"  UserName : " + userName);
	MK_DEV_PANEL.MsgToLog(L"  Password : " + password);

	if (!m_FTP.Connect(url, remotePath, userName, password))
	{
		MK_DEV_PANEL.MsgToLog(L"  FTP 접속을 실패했습니다!");
		MK_DEV_PANEL.InsertEmptyLine();
		return false;
	}

	MK_DEV_PANEL.MsgToLog(L"  FTP 접속을 성공했습니다.");
	MK_DEV_PANEL.InsertEmptyLine();

	// 삭제 할 파일과 data directory가 존재하면 purge state
	if (!m_PurgeList.Empty())
	{
		MkStr dataDir = MkPatchFileGenerator::PatchDataDirName;
		if (dataDir.CheckPostfix(L"\\"))
		{
			dataDir.BackSpace(1);
		}

		if (m_FTP.MoveToChild(dataDir, false))
		{
			MK_DEV_PANEL.MsgToLog(L"> " + MkStr(m_PurgeList.GetSize()) + L"개의 파일을 삭제 시도합니다.");

			m_State = ePurgeFiles;
		}
	}

	// 아니면 바로 upload state
	if (m_State != ePurgeFiles)
	{
		MK_DEV_PANEL.MsgToLog(L"> 삭제(Purge) 할 파일이 없습니다. 바로 파일 업로드를 시작합니다.");
		MK_DEV_PANEL.InsertEmptyLine();

		m_State = eUploadingFiles;
	}

	return true;
}

bool MkPatchFileUploader::Update(void)
{
	switch (m_State)
	{
	case eReady:
		return false;

	case ePurgeFiles:
		{
			if (!m_PurgeList.Empty())
			{
				MkPathName currPath = m_PurgeList[m_PurgeList.GetSize() - 1]; // 마지막 element
				if (m_FTP.DeleteChildFile(currPath))
				{
					MK_DEV_PANEL.MsgToLog(L"  - " + currPath);
				}

				m_PurgeList.PopBack();
			}
			else
			{
				MK_DEV_PANEL.InsertEmptyLine();

				m_State = ePurgeDirs;
			}
		}
		break;

	case ePurgeDirs:
		{
			if (m_PurgeDirNode.GetChildNodeCount() > 0)
			{
				MK_DEV_PANEL.MsgToLog(L"> 디렉토리 최적화를 시도합니다.");

				_PurgeEmptyDirectory(m_PurgeDirNode);

				MK_DEV_PANEL.InsertEmptyLine();
			}

			if (m_FTP.MoveToParent())
			{
				MK_DEV_PANEL.MsgToLog(L"> 파일 업로드를 시작합니다.");

				m_State = eUploadingFiles;
			}
			else
			{
				MK_DEV_PANEL.MsgToLog(L"> 디렉토리 최적화 후 root directory로 이동이 실패했습니다.");
				m_State = eReady;
			}
		}
		break;

	case eUploadingFiles:
		{
			if (m_CurrCommandIndex < m_UploadCommand.GetSize())
			{
				bool success = false;
				MkStr msg;
				msg.Reserve(1024);

				// 재시도의 경우 잠깐 대기
				if (m_CurrCommandCounter > 0)
				{
					::Sleep(5000); // 5 secs
				}
				++m_CurrCommandCounter;

				// 커맨드 실행
				const _UploadCommand& currCommand = m_UploadCommand[m_CurrCommandIndex];
				switch (currCommand.command)
				{
				case eMoveToChild:
					{
						success = m_FTP.MoveToChild(currCommand.target, true);
						msg += (success) ? L"  -> " : L"  (실패) -> ";
						msg += currCommand.target;
					}
					break;

				case eMoveToParent:
					{
						success = m_FTP.MoveToParent();
						msg += (success) ? L"  <- " : L"  (실패) <- ";
					}
					break;

				case eUploadFile:
					{
						MkPathName currFilePath = m_UploadDirectoryPath + currCommand.target;
						success = m_FTP.UploadFile(currFilePath);
						msg += (success) ? L"  + " : L"  (실패) + ";
						msg += currCommand.target;
					}
					break;
				}

				MK_DEV_PANEL.MsgToLog(msg);

				// 성공했으면 다음 커맨드로
				if (success)
				{
					++m_CurrCommandIndex;
					m_CurrCommandCounter = 0;
				}
				// 실패했으면 재시도
				else if (m_CurrCommandCounter == 30)
				{
					MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
					MK_DEV_PANEL.MsgToLog(L"> " + MkStr(m_CurrCommandCounter) + L"회의 시도가 모두 실패해서 취소합니다.");
					MK_DEV_PANEL.MsgToLog(L"> 이대로 작업을 멈추고 클라이언트팀에 문의 바랍니다.");
					MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
					MK_DEV_PANEL.InsertEmptyLine();

					m_State = eReady;
					_Clear();
				}
			}
			else
			{
				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"> 대상 디렉토리를 정리합니다.");
				MK_DEV_PANEL.InsertEmptyLine();

				m_State = eClearTargetDir;
			}
		}
		break;

	case eClearTargetDir:
		{
			if (m_UploadDirectoryPath.DeleteCurrentDirectory())
			{
				m_UploadDirectoryPath.MakeDirectoryPath();
			}

			MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
			MK_DEV_PANEL.MsgToLog(L"> 업로드가 완료되었습니다.");
			MK_DEV_PANEL.MsgToLog(L"---------------------------------------------------------------");
			MK_DEV_PANEL.InsertEmptyLine();

			m_State = eReady;
			_Clear();
		}
		break;
	}

	return true;
}

void MkPatchFileUploader::_Clear(void)
{
	m_PurgeList.Clear();
	m_PurgeDirNode.Clear();
	m_UploadDirectoryPath.Clear();
	m_UploadCommand.Clear();
	m_CurrCommandIndex = 0;
	m_CurrCommandCounter = 0;
	m_FTP.Close();
}

MkPatchFileUploader::MkPatchFileUploader()
{
	m_State = eReady;
	_Clear();
}

void MkPatchFileUploader::_AddPurgeDirectory(const MkStr& filePath, MkDataNode& purgeDirNode)
{
	MkArray<MkStr> tokens;
	unsigned int tokenSize = filePath.Tokenize(tokens, L"\\");
	if (tokenSize > 0)
	{
		// root부터 시작
		MkDataNode* currNode = &purgeDirNode;

		// directory만 저장
		unsigned int lastIndex = tokenSize - 1;
		for (unsigned int i=0; i<lastIndex; ++i)
		{
			MkHashStr currKey = tokens[i];
			currNode = currNode->ChildExist(currKey) ? currNode->GetChildNode(currKey) : currNode->CreateChildNode(currKey);
		}
	}
}

bool MkPatchFileUploader::_PurgeEmptyDirectory(const MkDataNode& purgeDirNode)
{
	MkArray<MkHashStr> dirs;
	purgeDirNode.GetChildNodeList(dirs);
	MK_INDEXING_LOOP(dirs, i)
	{
		const MkHashStr& dirName = dirs[i];

		// ftp에서 해당 dir로 이동 가능하면 네트워크상에서 존재한다는 의미. 삭제 시도
		if (m_FTP.MoveToChild(dirName.GetString(), false))
		{
			const MkDataNode& childNode = *purgeDirNode.GetChildNode(dirName);

			// 해당 노드에 자식이 존재하면 depth를 높여 재귀
			if (childNode.GetChildNodeCount() > 0)
			{
				if (!_PurgeEmptyDirectory(childNode))
					return false;
			}

			// 부모로 돌아와
			if (!m_FTP.MoveToParent())
				return false;

			// 해당 디렉토리 삭제 시도. 성공여부는 중요하지 않음
			if (m_FTP.DeleteChildDirectory(dirName.GetString()))
			{
				MkStr currPath;
				m_FTP.GetCurrentPath(currPath, true);
				currPath.ReplaceKeyword(L"/", L"\\");
				currPath += L"\\";
				currPath += dirName.GetString();
				currPath.PopFront(MkPatchFileGenerator::PatchDataDirName.GetSize() + 1);

				MK_DEV_PANEL.MsgToLog(L"  - " + currPath);
			}
		}
	}
	return true;
}

void MkPatchFileUploader::_BuildUploadCommand(const MkDataNode& structureNode, const MkPathName& currPath)
{
	MkArray<MkHashStr> objects;
	MkPathName::__GetSubFiles(structureNode, objects);

	MK_INDEXING_LOOP(objects, i)
	{
		_UploadCommand& cmd = m_UploadCommand.PushBack();
		cmd.command = eUploadFile;
		if (!currPath.Empty())
		{
			cmd.target = currPath;
			cmd.target += L"\\";
		}
		cmd.target += objects[i].GetString();
	}
	objects.Clear();

	MkPathName::__GetSubDirectories(structureNode, objects);
	MK_INDEXING_LOOP(objects, i)
	{
		// push
		_UploadCommand& cmd = m_UploadCommand.PushBack();
		cmd.command = eMoveToChild;
		cmd.target = objects[i].GetString();

		// 재귀
		MkPathName childPath;
		if (!currPath.Empty())
		{
			childPath = currPath;
			childPath += L"\\";
		}
		childPath += objects[i].GetString();

		_BuildUploadCommand(*structureNode.GetChildNode(objects[i]), childPath);

		// pop
		m_UploadCommand.PushBack().command = eMoveToParent;
	}
}

//------------------------------------------------------------------------------------------------//
