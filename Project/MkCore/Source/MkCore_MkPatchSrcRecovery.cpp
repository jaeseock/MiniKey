
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkFileSystem.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkZipCompressor.h"
#include "MkCore_MkFileDownloader.h"
#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkPatchSrcRecovery.h"


//------------------------------------------------------------------------------------------------//

bool MkPatchSrcRecovery::RecoverPatchSource
(const MkStr& url, const MkPathName& recoveryDirPath, const MkStr& historyDirName, const MkStr& patchDirName, const MkPathName& sourcePath)
{
	bool ok = (m_MainState == eReady) || (m_MainState == eShowSuccessResult) || (m_MainState == eShowFailedResult);
	if (ok)
	{
		m_RootURL = url;
		if ((!m_RootURL.Empty()) && (!m_RootURL.CheckPostfix(L"/")))
		{
			m_RootURL += L"/"; // ex> http://210.207.252.151/Test -> http://210.207.252.151/Test/
		}

		m_DataRootURL = m_RootURL + MkPatchFileGenerator::PatchDataDirName;
		m_DataRootURL.BackSpace(1);
		m_DataRootURL += L"/"; // ex> http://210.207.252.151/Test/data/

		m_RecoveryDirPath = recoveryDirPath; // ex> D:\\Updater
		m_RecoveryDirPath.CheckAndAddBackslash();
		m_RecoveryDirPath.MakeDirectoryPath();
		// ex> D:\\Updater\\

		// ���� history, patch, source directory�� ��� ����. ���� �ʱ�ȭ
		m_HistoryDirPath = m_RecoveryDirPath + historyDirName;
		m_HistoryDirPath.CheckAndAddBackslash();
		m_HistoryDirPath.DeleteCurrentDirectory();
		m_HistoryDirPath.MakeDirectoryPath();
		// ex> History -> D:\\Updater\\History\\

		m_PatchDirPath = m_RecoveryDirPath + patchDirName;
		m_PatchDirPath.CheckAndAddBackslash();
		m_PatchDirPath.DeleteCurrentDirectory();
		m_PatchDirPath.MakeDirectoryPath();
		// ex> Patch -> D:\\Updater\\Patch\\

		m_SourceDirPath = sourcePath; // ex> D:\\Src
		m_SourceDirPath.CheckAndAddBackslash();
		m_SourceDirPath.DeleteCurrentDirectory();
		m_SourceDirPath.MakeDirectoryPath();
		// ex> D:\\Src\\

		m_ErrorMsg.Clear();
		m_MainState = eDownloadPatchInfoFile;
		
		m_MaxProgress = 0;
		m_CurrentProgress = 0;
	}
	return ok;
}

void MkPatchSrcRecovery::Update(void)
{
	switch (m_MainState)
	{
	case eReady: break;

	case eDownloadPatchInfoFile:
		{
			do
			{
				// down launcher file
				MkStr launcherFileName = MkPatchFileGenerator::LauncherFileName.GetString() + L"." + MkDataNode::DefaultFileExtension; // ex> LauncherFile.mmd
				MkPathName launcherFileNameDown;
				MkPatchFileGenerator::ConvertFilePathToDownloadable(launcherFileName, launcherFileNameDown); // ex> LauncherFile.mmd.rp

				MkStr urlLauncherFilePath = m_RootURL + launcherFileNameDown; // ex> http://210.207.252.151/Test/LauncherFile.mmd.rp
				MkPathName localLauncherFilePath = m_PatchDirPath + launcherFileNameDown; // ex> D:\\Updater\\Patch\\LauncherFile.mmd.rp

				if (MkFileDownloader::DownloadFileRightNow(urlLauncherFilePath, localLauncherFilePath))
				{
					MK_DEV_PANEL.MsgToLog(L"> " + urlLauncherFilePath);
					MK_DEV_PANEL.MsgToLog(L"  ���� ���� �ٿ� ����.");
					MK_DEV_PANEL.InsertEmptyLine();
				}
				else
				{
					m_ErrorMsg = urlLauncherFilePath + L"\n��Ʈ��ũ ������ �ٿ��� �����߽��ϴ�.";
					break;
				}

				// down info file
				MkStr infoFileName = MkPatchFileGenerator::DataType_PatchInfo + L"." + MkDataNode::DefaultFileExtension; // ex> PatchInfo.mmd
				MkPathName infoFileNameDown;
				MkPatchFileGenerator::ConvertFilePathToDownloadable(infoFileName, infoFileNameDown); // ex> PatchInfo.mmd.rp

				MkStr urlInfoFilePath = m_RootURL + infoFileNameDown; // ex> http://210.207.252.151/Test/PatchInfo.mmd.rp
				MkPathName localInfoFilePath = m_PatchDirPath + infoFileNameDown; // ex> D:\\Updater\\Patch\\PatchInfo.mmd.rp

				if (MkFileDownloader::DownloadFileRightNow(urlInfoFilePath, localInfoFilePath))
				{
					MK_DEV_PANEL.MsgToLog(L"> " + urlInfoFilePath);
					MK_DEV_PANEL.MsgToLog(L"  ���� ���� �ٿ� ����.");
					MK_DEV_PANEL.InsertEmptyLine();
				}
				else
				{
					m_ErrorMsg = urlInfoFilePath + L"\n��Ʈ��ũ ������ �ٿ��� �����߽��ϴ�.";
					break;
				}

				// load info file
				m_PatchInfoNode.Clear();
				if (!m_PatchInfoNode.Load(localInfoFilePath))
				{
					m_ErrorMsg = urlInfoFilePath + L"\n��ġ�� ���� ���� �б� ����.";
					break;
				}

				m_CurrentWrittenTime = 0;
				if ((m_PatchInfoNode.GetDataTypeTag() != MkPatchFileGenerator::DataType_PatchInfo) ||
					(!m_PatchInfoNode.ChildExist(MkPatchFileGenerator::KEY_StructureNode)) ||
					(!m_PatchInfoNode.GetData(MkPatchFileGenerator::KEY_CurrWT, m_CurrentWrittenTime, 0)))
				{
					m_ErrorMsg = urlInfoFilePath + L"\n��ġ�� ������ �������� ���� ������ �ƴմϴ�.";
					break;
				}

				m_PatchInfoNode.SetData(MkPatchFileGenerator::KEY_LastWT, static_cast<unsigned int>(0), 0);

				MK_DEV_PANEL.MsgToLog(L"> " + localInfoFilePath);
				MK_DEV_PANEL.MsgToLog(L"  ���� ���� �ε� ���� : " + MkPatchFileGenerator::ConvertWrittenTimeToStr(m_CurrentWrittenTime));
				MK_DEV_PANEL.InsertEmptyLine();

				// file system node ������ ����
				MkDataNode* fsSettingNode = m_PatchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_FileSystemNode);
				if (fsSettingNode != NULL)
				{
					fsSettingNode->SaveToText(m_RecoveryDirPath + MkFileSystem::DefaultSettingFileName);

					// �� �̻� �ʿ� ����
					fsSettingNode->DetachFromParentNode();
					delete fsSettingNode;

					MK_DEV_PANEL.MsgToLog(L"> ���� �ý��� ���� ����");
					MK_DEV_PANEL.InsertEmptyLine();
				}

				// info node�� history node�� ��ȯ
				m_PatchInfoNode.SetDataTypeTag(MkPatchFileGenerator::DataType_PatchHistory);

				// launcher ���� ����
				MkDataNode* launcherNode = m_PatchInfoNode.CreateChildNode(MkPatchFileGenerator::KEY_LauncherNode);
				if ((launcherNode == NULL) || (!launcherNode->Load(localLauncherFilePath)))
				{
					m_ErrorMsg = localLauncherFilePath + L"\n��ġ�� ���� ���� �б� ����";
					break;
				}

				// update node ����
				MkDataNode* updateNode = m_PatchInfoNode.CreateChildNode(MkPatchFileGenerator::KEY_UpdateNode);
				if (updateNode == NULL)
				{
					m_ErrorMsg = MkPatchFileGenerator::KEY_UpdateNode.GetString() + L" ��� ���� ����";
					break;
				}

				// purge list�� ������ update node ������ ����
				MkDataNode* purgeNode = m_PatchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_PurgeListNode);
				if (purgeNode != NULL)
				{
					updateNode->AttachChildNode(purgeNode);
				}
			}
			while (false);

			if (m_ErrorMsg.Empty())
			{
				m_PatchDataPath = m_PatchDirPath + MkPatchFileGenerator::PatchDataDirName; // ex> D:\\Updater\\Patch\\data\\

				m_MainState = eFindDownloadTargets;
			}
			else
			{
				MK_DEV_PANEL.MsgToLog(m_ErrorMsg);
				MK_DEV_PANEL.InsertEmptyLine();

				m_MainState = eShowFailedResult;
			}
		}
		break;

	case eFindDownloadTargets:
		{
			// structure �������� �ٿ���� ���� �˻�(structure node ���翩�δ� ������ �˻�)
			const MkDataNode& structureNode = *m_PatchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_StructureNode);
			unsigned int totalFiles = MkPathName::__CountTotalFiles(structureNode);
			MK_DEV_PANEL.MsgToLog(L"> ���� ��ġ ���� : " + MkStr(totalFiles) + L" ��");

			m_DownloadFileInfoList.Clear();
			m_DownloadFileInfoList.Reserve(totalFiles);

			_FindFilesToDownload(structureNode, MkStr::EMPTY);

			if (totalFiles > 0)
			{
				MkDataNode* updateNode = m_PatchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_UpdateNode);
				m_UpdateAddNode = updateNode->CreateChildNode(MkPatchFileGenerator::KEY_AddNode);
			}

			// �ѷ� ���
			unsigned __int64 totalDownloadSize = 0;
			MK_INDEXING_LOOP(m_DownloadFileInfoList, i)
			{
				const _DownloadFileInfo& fi = m_DownloadFileInfoList[i];
				totalDownloadSize += static_cast<unsigned __int64>((fi.compSize > 0) ? fi.compSize : fi.origSize);
			}
			
			MK_DEV_PANEL.MsgToLog(L"> �ٿ� ��� ���� : " + MkStr(m_DownloadFileInfoList.GetSize()) + L" ��");

			MkStr sizeBuf;
			_ConvertDataSizeToString(totalDownloadSize, sizeBuf);
			MK_DEV_PANEL.MsgToLog(L"> �ٿ� ��� ũ�� : " + sizeBuf);
			MK_DEV_PANEL.InsertEmptyLine();
			MK_DEV_PANEL.MsgToLog(L"> ���� �ٿ��� �����մϴ�.");

			m_MainState = eRegisterTargetFiles;
			m_MaxProgress = m_DownloadFileInfoList.GetSize();
			m_CurrentProgress = 0;
		}
		break;

	case eRegisterTargetFiles:
		{
			if (m_CurrentProgress == m_MaxProgress)
			{
				m_CurrentProgress = 0;

				MK_DEV_PANEL.MsgToLog(L"> ���� �ٿ�� ����ŷ �Ϸ�Ǿ����ϴ�.");
				MK_DEV_PANEL.InsertEmptyLine();

				MkPathName historyFilePath = m_HistoryDirPath + MkStr(m_CurrentWrittenTime) + L"." + MkDataNode::DefaultFileExtension;
				if (m_PatchInfoNode.SaveToBinary(historyFilePath))
				{
					MK_DEV_PANEL.MsgToLog(historyFilePath + L"\n���� �Ϸ�Ǿ����ϴ�.");
					MK_DEV_PANEL.InsertEmptyLine();
					MK_DEV_PANEL.MsgToLog(L"---------------------------------------------");
					MK_DEV_PANEL.MsgToLog(L"> ������ �Ϸ�Ǿ����ϴ�.");
					MK_DEV_PANEL.MsgToLog(L"---------------------------------------------");

					m_MainState = eShowSuccessResult;
				}
				else
				{
					MK_DEV_PANEL.MsgToLog(historyFilePath + L"\n������ �����Ͽ����ϴ�.");

					m_MainState = eShowFailedResult;
				}
			}
			else
			{
				// �̹� �ٿ���� ���� ��ŵ
				const _DownloadFileInfo& fileInfo = m_DownloadFileInfoList[m_CurrentProgress];
				
				// url
				MkPathName relPathInServer, relPathInClient;
				MkPatchFileGenerator::ConvertFilePathToDownloadable(fileInfo.filePath, relPathInServer); // ex> AAA\\BBB\\abc.mmd -> AAA\\BBB\\abc.mmd.rp
				relPathInClient = relPathInServer;
				relPathInServer.ReplaceKeyword(L"\\", L"/"); // ex> AAA/BBB/abc.mmd.rp
				MkStr urlPath = m_DataRootURL + relPathInServer; // ex> http://210.207.252.151/Test/data/AAA/BBB/abc.mmd.rp
				MK_DEV_PANEL.MsgToLog(L" �� " + relPathInServer);

				// local. ���࿩�� ������ �ʰ� �״�� �ٿ�
				m_CurrentDownInfo = MK_FILE_DOWNLOADER.DownloadFile(urlPath, m_PatchDataPath + relPathInClient); // ex> D:\\Updater\\Patch\\data\\AAA\\BBB\\abc.mmd.rp
				if (m_CurrentDownInfo == NULL)
				{
					m_MainState = eShowFailedResult;
					m_MaxProgress = 0;
					m_CurrentProgress = 0;
				}
				else
				{
					m_MainState = eDownloadTargetFiles;
				}
			}
		}
		break;

	case eDownloadTargetFiles:
		{
			MkFileDownInfo::eDownState downState = m_CurrentDownInfo->GetDownState();
			if ((downState == MkFileDownInfo::eDS_Wait) || (downState == MkFileDownInfo::eDS_Downloading))
			{
				::Sleep(1); // ���
				break;
			}
			else if (downState == MkFileDownInfo::eDS_Complete) // �Ϸ�
			{
				const _DownloadFileInfo& fileInfo = m_DownloadFileInfoList[m_CurrentProgress];

				MkPathName relPathInClient;
				MkPatchFileGenerator::ConvertFilePathToDownloadable(fileInfo.filePath, relPathInClient); // ex> AAA\\BBB\\abc.mmd -> AAA\\BBB\\abc.mmd.rp
				MkPathName localDownedFilePath = m_PatchDataPath + relPathInClient; // ��ġ ����. ex> D:\\Updater\\Patch\\data\\AAA\\BBB\\abc.mmd.rp

				MkPathName localSrcFilePath = m_SourceDirPath + fileInfo.filePath; // �ҽ� ����. ex> D:\\Src\\AAA\\BBB\\abc.mmd
				bool compressed = (fileInfo.compSize > 0);

				do
				{
					if (compressed)
					{
						// ���� ���� �о�鿩
						MkByteArray compData;
						MkInterfaceForFileReading frInterface;
						if (!frInterface.SetUp(localDownedFilePath))
						{
							m_ErrorMsg = L"�ٿ�� ���� ���� ����.";
							break;
						}
						if (frInterface.Read(compData, MkArraySection(0)) != fileInfo.compSize)
						{
							m_ErrorMsg = L"�ٿ�� ���� �б� �����ų� �ٿ�� ���� ���� ũ�Ⱑ �ٸ��ϴ�. �´� ũ�� : " + MkStr(fileInfo.compSize);
							break;
						}
						frInterface.Clear();

						// ���� ����
						MkByteArray origData;
						if (MkZipCompressor::Uncompress(compData.GetPtr(), compData.GetSize(), origData) != fileInfo.origSize)
						{
							m_ErrorMsg = L"�ٿ�� ���� ���� ���� �� ũ�Ⱑ �ٸ��ϴ�. �´� ũ�� : " + MkStr(fileInfo.origSize);
							break;
						}
						compData.Clear();

						// �ҽ� ���Ϸ� ���
						MkInterfaceForFileWriting fwInterface;
						if (!fwInterface.SetUp(localSrcFilePath, true, true))
						{
							m_ErrorMsg = L"�ҽ� ���� ���� ����.";
							break;
						}
						if (fwInterface.Write(origData, MkArraySection(0)) != fileInfo.origSize)
						{
							m_ErrorMsg = L"�ҽ� ���� ���� ����.";
							break;
						}
						fwInterface.Clear();
						origData.Clear();
					}
					else
					{
						// ũ�⸸ �˻� �� �ҽ� ���Ϸ� ����
						if (localDownedFilePath.GetFileSize() != fileInfo.origSize)
						{
							m_ErrorMsg = L"�ٿ�� ���� ���� ũ�Ⱑ �ٸ��ϴ�. �´� ũ�� : " + MkStr(fileInfo.origSize);
							break;
						}

						if (!localDownedFilePath.CopyCurrentFile(localSrcFilePath, false)) // overwrite
						{
							m_ErrorMsg = L"�ҽ� ���Ϸ� ���� ����.";
							break;
						}
					}

					// ���� �����ð� ����
					if (!localSrcFilePath.SetWrittenTime(fileInfo.writtenTime))
					{
						m_ErrorMsg = L"�ҽ� ���� �����ð� ���� ����";
						break;
					}

					// history�� update-add node�� �߰�
					MkDataNode* fileAddNode = m_UpdateAddNode->CreateChildNode(fileInfo.filePath);
					if ((fileAddNode == NULL) ||
						(!fileAddNode->CreateUnit(MkPathName::KeyFileSize, fileInfo.origSize)) ||
						(!fileAddNode->CreateUnit(MkPathName::KeyWrittenTime, fileInfo.writtenTime)))
					{
						m_ErrorMsg = L"�ҽ� ������ add node ���� ���� ����";
						break;
					}
					
				}
				while (false);
			}
			else // �ٿ�ε� �ߴ�/����
			{
				MkStr fileURL;
				MkPathName destLocalPath;
				m_CurrentDownInfo->__GetTargetInfo(fileURL, destLocalPath);

				switch (downState)
				{
				case MkFileDownInfo::eDS_Abort: m_ErrorMsg = fileURL + L"\n����ڿ� ���� �ߵ�����"; break;
				case MkFileDownInfo::eDS_InvalidURL: m_ErrorMsg = fileURL + L"\n�ش� ������ ��Ʈ��ũ�� �������� ����"; break;
				case MkFileDownInfo::eDS_OutOfMemory: m_ErrorMsg = fileURL + L"\n�޸� ����"; break;
				case MkFileDownInfo::eDS_Failed: m_ErrorMsg = fileURL + L"\n�ٿ�ε� ����"; break;
				}
			}

			if (m_ErrorMsg.Empty())
			{
				MK_DEV_PANEL.MsgToLog(L"  + " + m_DownloadFileInfoList[m_CurrentProgress].filePath);

				m_MainState = eRegisterTargetFiles;
				++m_CurrentProgress;
			}
			else
			{
				m_MainState = eShowFailedResult;
				m_MaxProgress = 0;
				m_CurrentProgress = 0;
			}

			// info ����
			delete m_CurrentDownInfo;
			m_CurrentDownInfo = NULL;
		}
		break;
		
	case eShowSuccessResult:
		break;

	case eShowFailedResult:
		{
			if (!m_ErrorMsg.Empty())
			{
				MK_DEV_PANEL.MsgToLog(m_ErrorMsg);
				MK_DEV_PANEL.InsertEmptyLine();

				::MessageBox(NULL, m_ErrorMsg.GetPtr(), L"ERROR!!!", MB_OK);
				m_MainState = eReady;
			}
		}
		break;
	}
}

MkPatchSrcRecovery::MkPatchSrcRecovery()
{
	m_MainState = eReady;
	m_MaxProgress = 0;
	m_CurrentProgress = 0;
	m_CurrentDownInfo = NULL;
	m_UpdateAddNode = NULL;
}

//------------------------------------------------------------------------------------------------//

void MkPatchSrcRecovery::_FindFilesToDownload(const MkDataNode& node, const MkPathName& pathOffset)
{
	// ���� �˻�
	MkArray<MkHashStr> subObjs;
	MkPathName::__GetSubFiles(node, subObjs);
	MK_INDEXING_LOOP(subObjs, i)
	{
		// patch file info
		unsigned int patchOrigSize = 0, patchCompSize = 0, patchWrittenTime = 0;
		const MkDataNode& fileNode = *node.GetChildNode(subObjs[i]);
		fileNode.GetData(MkPathName::KeyFileSize, patchOrigSize, 0);
		fileNode.GetData(MkPathName::KeyFileSize, patchCompSize, 1);
		fileNode.GetData(MkPathName::KeyWrittenTime, patchWrittenTime, 0);

		// down info
		_DownloadFileInfo& info = m_DownloadFileInfoList.PushBack();

		info.filePath = pathOffset + subObjs[i];
		info.origSize = patchOrigSize;
		info.compSize = ((patchOrigSize > 0) && (patchCompSize > 0)) ? patchCompSize : 0;
		info.writtenTime = patchWrittenTime;
	}
	
	// ���� ���丮 �˻�
	subObjs.Clear();
	MkPathName::__GetSubDirectories(node, subObjs);
	MK_INDEXING_LOOP(subObjs, i)
	{
		MkPathName newOffset = pathOffset + subObjs[i];
		newOffset.CheckAndAddBackslash();
		_FindFilesToDownload(*node.GetChildNode(subObjs[i]), newOffset); // ���
	}
}

void MkPatchSrcRecovery::_ConvertDataSizeToString(unsigned __int64 size, MkStr& buffer)
{
	unsigned __int64 totalKB = size / 1024;
	unsigned __int64 gb = 0, mb = 0, kb = 0;

	// giga ����? -> GB/MB
	if (totalKB > (1024 * 1024))
	{
		gb = totalKB / (1024 * 1024);
		mb = (totalKB - gb * (1024 * 1024)) / 1024;
	}
	// mega ����? -> MB/KB
	else if (totalKB > 1024)
	{
		mb = totalKB / 1024;
		kb = totalKB - mb * 1024;
	}
	// KB
	else
	{
		kb = totalKB;
	}

	buffer.Clear();
	buffer.Reserve(32);
	if (gb > 0)
	{
		buffer += static_cast<unsigned int>(gb);
		buffer += L"GB";
	}
	if (mb > 0)
	{
		if (!buffer.Empty())
		{
			buffer += L" ";
		}

		buffer += static_cast<unsigned int>(mb);
		buffer += L"MB";
	}
	if (kb > 0)
	{
		if (!buffer.Empty())
		{
			buffer += L" ";
		}
		buffer += static_cast<unsigned int>(kb);
		buffer += L"KB";
	}

	if (buffer.Empty())
	{
		buffer = L"0KB";
	}
}

//------------------------------------------------------------------------------------------------//
