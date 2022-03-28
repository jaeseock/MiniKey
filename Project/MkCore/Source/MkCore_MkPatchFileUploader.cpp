
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkPatchFileUploader.h"

//------------------------------------------------------------------------------------------------//

bool MkPatchFileUploader::StartUploading
(const MkPathName& targetDirPath, const MkStr& url, const MkStr& remotePath, const MkStr& userName, const MkStr& password, bool passiveMode, MkStr* outMsg)
{
	if (m_State != eReady)
		return false;

	_Clear();
	
	// target dir ���翩�� �˻�
	m_UploadDirectoryPath.ConvertToRootBasisAbsolutePath(targetDirPath);
	if (!m_UploadDirectoryPath.CheckAvailable())
	{
		_AddLogMessage(outMsg, L"> " + targetDirPath);
		_AddLogMessage(outMsg, L"  ��� ���丮�� �������� �ʽ��ϴ�.");
		_AddLogMessage(outMsg, MkStr::EMPTY);
		return false;
	}

	// file structure
	MkDataNode uploadNode;
	if (m_UploadDirectoryPath.ExportSystemStructure(uploadNode) == 0)
	{
		_AddLogMessage(outMsg, L"> " + targetDirPath);
		_AddLogMessage(outMsg, L"  ��� ���丮�� ������ �����ϴ�.");
		_AddLogMessage(outMsg, MkStr::EMPTY);
		return true; // ���� ������ ���� ��� ��ġ ��� ���� ���ε� ����� ���� ���� ����
	}

	// launcher info �������� �˻�
	MkStr launcherInfoFileName = MkPatchFileGenerator::LauncherFileName.GetString() + L"." + MkDataNode::DefaultFileExtension; // ex> LauncherFile.mmd
	MkPathName launcherInfoFileDown;
	MkPatchFileGenerator::ConvertFilePathToDownloadable(launcherInfoFileName, launcherInfoFileDown); // ex> LauncherFile.mmd.rp
	if (!uploadNode.ChildExist(launcherInfoFileDown))
	{
		_AddLogMessage(outMsg, L"> " + targetDirPath);
		_AddLogMessage(outMsg, L"  ���丮�� " + launcherInfoFileDown + L" ������ �������� �ʽ��ϴ�.");
		_AddLogMessage(outMsg, MkStr::EMPTY);
		return false;
	}
	
	// patch info �������� �˻�
	MkStr patchInfoFileName = MkPatchFileGenerator::DataType_PatchInfo + L"." + MkDataNode::DefaultFileExtension; // ex> PatchInfo.mmd
	MkPathName patchInfoFileDown;
	MkPatchFileGenerator::ConvertFilePathToDownloadable(patchInfoFileName, patchInfoFileDown); // ex> PatchInfo.mmd.rp
	if (!uploadNode.ChildExist(patchInfoFileDown))
	{
		_AddLogMessage(outMsg, L"> " + targetDirPath);
		_AddLogMessage(outMsg, L"  ���丮�� " + patchInfoFileDown + L" ������ �������� �ʽ��ϴ�.");
		_AddLogMessage(outMsg, MkStr::EMPTY);
		return false;
	}

	// purge ���� ����
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
			
			// ������ ���� ��� ����Ʈ
			m_PurgeList.PushBack(currPath);

			// ���丮�� ������� ���� �����ϱ� ���� ������ ������ ����
			_AddPurgeDirectory(currPath, m_PurgeDirNode);
		}
	}

	// upload command ����
	m_UploadCommand.Reserve(400000); // ��ʸ����� ������� ������...
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

	_AddLogMessage(outMsg, L"> " + MkStr(uploadDirCount) + L"���� ���丮�� " + MkStr(uploadFileCount) + L"���� ���� ���ε� ����.");
	_AddLogMessage(outMsg, MkStr::EMPTY);
	
	// ftp ����
	_AddLogMessage(outMsg, L"> URL : " + url);
	_AddLogMessage(outMsg, L"  RemotePath : " + remotePath);
	_AddLogMessage(outMsg, L"  UserName : " + userName);
	_AddLogMessage(outMsg, L"  Password : " + password);
	_AddLogMessage(outMsg, (passiveMode) ? L"  Passive mode" : L"  Active mode");

	if (!m_FTP.Connect(url, remotePath, userName, password, passiveMode))
	{
		_AddLogMessage(outMsg, L"  FTP ������ �����߽��ϴ�!");
		_AddLogMessage(outMsg, MkStr::EMPTY);
		return false;
	}

	_AddLogMessage(outMsg, L"  FTP ������ �����߽��ϴ�.");
	_AddLogMessage(outMsg, MkStr::EMPTY);

	// ���� �� ���ϰ� data directory�� �����ϸ� purge state
	if (!m_PurgeList.Empty())
	{
		MkStr dataDir = MkPatchFileGenerator::PatchDataDirName;
		if (dataDir.CheckPostfix(L"\\"))
		{
			dataDir.BackSpace(1);
		}

		if (m_FTP.MoveToChild(dataDir, false))
		{
			_AddLogMessage(outMsg, L"> " + MkStr(m_PurgeList.GetSize()) + L"���� ������ ���� �õ��մϴ�.");

			m_State = ePurgeFiles;
		}
	}

	// �ƴϸ� �ٷ� upload state
	if (m_State != ePurgeFiles)
	{
		_AddLogMessage(outMsg, L"> ����(Purge) �� ������ �����ϴ�. �ٷ� ���� ���ε带 �����մϴ�.");
		_AddLogMessage(outMsg, MkStr::EMPTY);

		m_State = eUploadingFiles;
	}

	return true;
}

bool MkPatchFileUploader::Update(MkStr* outMsg)
{
	switch (m_State)
	{
	case eReady:
		return false;

	case ePurgeFiles:
		{
			if (!m_PurgeList.Empty())
			{
				MkPathName currPath = m_PurgeList[m_PurgeList.GetSize() - 1]; // ������ element
				if (m_FTP.DeleteChildFile(currPath))
				{
					_AddLogMessage(outMsg, L"  - " + currPath);
				}

				m_PurgeList.PopBack();
			}
			else
			{
				_AddLogMessage(outMsg, MkStr::EMPTY);

				m_State = ePurgeDirs;
			}
		}
		break;

	case ePurgeDirs:
		{
			if (m_PurgeDirNode.GetChildNodeCount() > 0)
			{
				_AddLogMessage(outMsg, L"> ���丮 ����ȭ�� �õ��մϴ�.");

				_PurgeEmptyDirectory(m_PurgeDirNode, outMsg);

				_AddLogMessage(outMsg, MkStr::EMPTY);
			}

			if (m_FTP.MoveToParent())
			{
				_AddLogMessage(outMsg, L"> ���� ���ε带 �����մϴ�.");

				m_State = eUploadingFiles;
			}
			else
			{
				_AddLogMessage(outMsg, L"> ���丮 ����ȭ �� root directory�� �̵��� �����߽��ϴ�.");

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

				// ��õ��� ��� ��� ���
				if (m_CurrCommandCounter > 0)
				{
					::Sleep(5000); // 5 secs
				}
				++m_CurrCommandCounter;

				// Ŀ�ǵ� ����
				const _UploadCommand& currCommand = m_UploadCommand[m_CurrCommandIndex];
				switch (currCommand.command)
				{
				case eMoveToChild:
					{
						success = m_FTP.MoveToChild(currCommand.target, true);
						msg += (success) ? L"  -> " : L"  (����) -> ";
						msg += currCommand.target;
					}
					break;

				case eMoveToParent:
					{
						success = m_FTP.MoveToParent();
						msg += (success) ? L"  <- " : L"  (����) <- ";
					}
					break;

				case eUploadFile:
					{
						MkPathName currFilePath = m_UploadDirectoryPath + currCommand.target;
						success = m_FTP.UploadFile(currFilePath);
						msg += (success) ? L"  + " : L"  (����) + ";
						msg += currCommand.target;
					}
					break;
				}

				_AddLogMessage(outMsg, msg);

				// ���������� ���� Ŀ�ǵ��
				if (success)
				{
					++m_CurrCommandIndex;
					m_CurrCommandCounter = 0;
				}
				// ���������� ��õ�
				else if (m_CurrCommandCounter == 30)
				{
					_AddLogMessage(outMsg, L"---------------------------------------------------------------");
					_AddLogMessage(outMsg, L"> " + MkStr(m_CurrCommandCounter) + L"ȸ�� �õ��� ��� �����ؼ� ����մϴ�.");
					_AddLogMessage(outMsg, L"> �̴�� �۾��� ���߰� Ŭ���̾�Ʈ���� ���� �ٶ��ϴ�.");
					_AddLogMessage(outMsg, L"---------------------------------------------------------------");
					_AddLogMessage(outMsg, MkStr::EMPTY);

					m_State = eReady;
					_Clear();
				}
			}
			else
			{
				_AddLogMessage(outMsg, MkStr::EMPTY);
				_AddLogMessage(outMsg, L"> ��� ���丮�� �����մϴ�.");
				_AddLogMessage(outMsg, MkStr::EMPTY);

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

			_AddLogMessage(outMsg, L"---------------------------------------------------------------");
			_AddLogMessage(outMsg, L"> ���ε尡 �Ϸ�Ǿ����ϴ�.");
			_AddLogMessage(outMsg, L"---------------------------------------------------------------");
			_AddLogMessage(outMsg, MkStr::EMPTY);

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
		// root���� ����
		MkDataNode* currNode = &purgeDirNode;

		// directory�� ����
		unsigned int lastIndex = tokenSize - 1;
		for (unsigned int i=0; i<lastIndex; ++i)
		{
			MkHashStr currKey = tokens[i];
			currNode = currNode->ChildExist(currKey) ? currNode->GetChildNode(currKey) : currNode->CreateChildNode(currKey);
		}
	}
}

bool MkPatchFileUploader::_PurgeEmptyDirectory(MkDataNode& purgeDirNode, MkStr* outMsg)
{
	MkArray<MkHashStr> dirs;
	purgeDirNode.GetChildNodeList(dirs);
	MK_INDEXING_LOOP(dirs, i)
	{
		const MkHashStr& dirName = dirs[i];

		// ftp���� �ش� dir�� �̵� �����ϸ� ��Ʈ��ũ�󿡼� �����Ѵٴ� �ǹ�. ���� �õ�
		if (m_FTP.MoveToChild(dirName.GetString(), false))
		{
			MkDataNode& childNode = *purgeDirNode.GetChildNode(dirName);

			// �ش� ��忡 �ڽ��� �����ϸ� depth�� ���� ���
			if (childNode.GetChildNodeCount() > 0)
			{
				if (!_PurgeEmptyDirectory(childNode, outMsg))
					return false;
			}

			// �θ�� ���ƿ�
			if (!m_FTP.MoveToParent())
				return false;

			// �ڽ��� ������ �ش� ���丮 ���� �õ�
			if ((childNode.GetChildNodeCount() == 0) && m_FTP.DeleteChildDirectory(dirName.GetString()))
			{
				purgeDirNode.RemoveChildNode(dirName); // �ش� ��� ����

				MkStr currPath;
				m_FTP.GetCurrentPath(currPath, true);
				currPath.ReplaceKeyword(L"/", L"\\");
				currPath += L"\\";
				currPath += dirName.GetString();
				currPath.PopFront(MkPatchFileGenerator::PatchDataDirName.GetSize() + 1);
				_AddLogMessage(outMsg, L"  - " + currPath);
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

		// ���
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

void MkPatchFileUploader::_AddLogMessage(MkStr* outMsg, const MkStr& msg)
{
	MK_DEV_PANEL.MsgToLog(msg);

	if (outMsg != NULL)
	{
		*outMsg += msg;
		*outMsg += MkStr::LF;
	}
}

//------------------------------------------------------------------------------------------------//
