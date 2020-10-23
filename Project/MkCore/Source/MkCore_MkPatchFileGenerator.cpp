
#include <atltime.h>
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkFileSystem.h"
#include "MkCore_MkZipCompressor.h"
#include "MkCore_MkPatchFileGenerator.h"

const MkStr MkPatchFileGenerator::DownloadableFileExtension(L"rp");
const MkStr MkPatchFileGenerator::DataType_PatchHistory(L"PatchHistory");
const MkStr MkPatchFileGenerator::DataType_PatchInfo(L"PatchInfo");
const MkPathName MkPatchFileGenerator::PatchDataDirName(L"data\\");
const MkHashStr MkPatchFileGenerator::KEY_CurrWT(L"#CurrWT");
const MkHashStr MkPatchFileGenerator::KEY_LastWT(L"#LastWT");
const MkHashStr MkPatchFileGenerator::KEY_StructureNode(L"#Structure");
const MkHashStr MkPatchFileGenerator::KEY_FileSystemNode(L"#FileSystem");
const MkHashStr MkPatchFileGenerator::KEY_PurgeListNode(L"PurgeList");
const MkHashStr MkPatchFileGenerator::KEY_LauncherNode(L"#Launcher");
const MkHashStr MkPatchFileGenerator::KEY_UpdateNode(L"#Update");
const MkHashStr MkPatchFileGenerator::KEY_AddNode(L"#Add");
const MkHashStr MkPatchFileGenerator::KEY_ReplaceNode(L"#Replace");
const MkHashStr MkPatchFileGenerator::KEY_DeleteNode(L"#Delete");
const MkHashStr MkPatchFileGenerator::LauncherFileName(L"LauncherFile");
const MkHashStr MkPatchFileGenerator::KEY_RunFilePath(L"#RunFilePath");


//------------------------------------------------------------------------------------------------//

void MkPatchFileGenerator::SetLauncherFileName(const MkPathName& fileName)
{
	m_LauncherFileName = fileName;

	if (!m_LauncherFileName.Empty())
	{
		MK_DEV_PANEL.MsgToLog(L"> " + m_LauncherFileName);
		MK_DEV_PANEL.MsgToLog(L"  ���� ���ϸ� ����");
		MK_DEV_PANEL.InsertEmptyLine();
	}
}

void MkPatchFileGenerator::SetRunFilePath(const MkPathName& filePath)
{
	m_RunFilePath = filePath;

	if (!m_RunFilePath.Empty())
	{
		MK_DEV_PANEL.MsgToLog(L"> " + m_RunFilePath);
		MK_DEV_PANEL.MsgToLog(L"  ���� ���ϸ� ����");
		MK_DEV_PANEL.InsertEmptyLine();
	}
}

bool MkPatchFileGenerator::SetHistoryDirectoryPath(const MkPathName& historyDirPath)
{
	MkPathName targetDir;
	targetDir.ConvertToRootBasisAbsolutePath(historyDirPath);
	if (!targetDir.CheckAvailable())
	{
		MkStr msg;
		msg.Reserve(512);
		msg += targetDir;
		msg += MkStr::LF;
		msg += L"history ������ �������� �ʽ��ϴ�.";
		msg += MkStr::LF;
		msg += MkStr::LF;
		msg += L"���� �� �ʱ�ȭ �Ͻðڽ��ϱ�?";
		
		int rlt = ::MessageBox(m_hWnd, msg.GetPtr(), L"MkPatchFileGenerator", MB_YESNO);
		if (rlt == IDYES)
		{
			targetDir.MakeDirectoryPath(); // ����
		}
		else if (rlt == IDNO)
		{
			targetDir.Clear();
			targetDir.GetDirectoryPathFromDialog(L"���� history ���� ����.", m_hWnd, MkPathName::GetRootDirectory()); // ����
		}
	}

	m_HistoryDirPath = targetDir;
	bool ok = !m_HistoryDirPath.Empty();
	if (ok)
	{
		m_LastHistory.Clear();
		
		MK_DEV_PANEL.MsgToLog(L"> " + m_HistoryDirPath);
		MK_DEV_PANEL.MsgToLog(L"  history ���� ����");
		MK_DEV_PANEL.InsertEmptyLine();

		// history file table ����
		m_HistoryList.Clear();
		
		MkArray<MkPathName> filePathList, extensionFilter;
		extensionFilter.PushBack(MkDataNode::DefaultFileExtension);
		m_HistoryDirPath.GetWhiteFileList(filePathList, NULL, &extensionFilter, NULL, NULL, false, false); // Ȯ���ڷ� ���͸�
		m_HistoryList.Reserve(filePathList.GetSize());

		MK_INDEXING_LOOP(filePathList, i)
		{
			MkPathName currName = filePathList[i].GetFileName(false);
			if (currName.IsDigit())
			{
				m_HistoryList.PushBack(currName.ToUnsignedInteger());
			}
		}
		
		if (m_HistoryList.Empty())
		{
			MK_DEV_PANEL.MsgToLog(L"> ���� history ������ �����ϴ�.");
		}
		else
		{
			m_HistoryList.SortInDescendingOrder();

			MK_DEV_PANEL.MsgToLog(L"> ���� history ���� : " + MkStr(m_HistoryList.GetSize()) + L" ��");
			MK_DEV_PANEL.MsgToLog(L"  ������ history : " + ConvertWrittenTimeToStr(m_HistoryList[0]));
		}
		MK_DEV_PANEL.InsertEmptyLine();
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(L">  history ���� ���� ����!!! Ȯ���� �ʿ��մϴ�.");
		MK_DEV_PANEL.InsertEmptyLine();
	}
	return ok;
}

bool MkPatchFileGenerator::SetPatchRootDirectoryPath(const MkPathName& patchDirPath)
{
	MkPathName targetDir;
	targetDir.ConvertToRootBasisAbsolutePath(patchDirPath);
	if (!targetDir.CheckAvailable())
	{
		MkStr msg;
		msg.Reserve(512);
		msg += targetDir;
		msg += MkStr::LF;
		msg += L"patch root ������ �������� �ʽ��ϴ�.";
		msg += MkStr::LF;
		msg += MkStr::LF;
		msg += L"���� �� �ʱ�ȭ �Ͻðڽ��ϱ�?";
		
		int rlt = ::MessageBox(m_hWnd, msg.GetPtr(), L"MkPatchFileGenerator", MB_YESNO);
		if (rlt == IDYES)
		{
			targetDir.MakeDirectoryPath(); // ����
		}
		else if (rlt == IDNO)
		{
			targetDir.Clear();
			targetDir.GetDirectoryPathFromDialog(L"���� patch root ���� ����.", m_hWnd, MkPathName::GetRootDirectory()); // ����
		}
	}

	m_PatchRootPath = targetDir;
	bool ok = !m_PatchRootPath.Empty();
	if (ok)
	{
		MK_DEV_PANEL.MsgToLog(L"> " + m_PatchRootPath);
		MK_DEV_PANEL.MsgToLog(L"  patch root ���� ����.");
		MK_DEV_PANEL.InsertEmptyLine();

		m_PatchDataPath = m_PatchRootPath + PatchDataDirName;
		MK_DEV_PANEL.MsgToLog(L"> " + m_PatchDataPath);
		if (m_PatchDataPath.CheckAvailable())
		{
			MK_DEV_PANEL.MsgToLog(L"  patch data ���� ����.");
		}
		else
		{
			m_PatchDataPath.MakeDirectoryPath(); // ����
			MK_DEV_PANEL.MsgToLog(L"  patch data ������ ���� �����մϴ�.");
		}
		MK_DEV_PANEL.InsertEmptyLine();
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(L">  patch root ���� ���� ����!!! Ȯ���� �ʿ��մϴ�.");
		MK_DEV_PANEL.InsertEmptyLine();
	}
	return ok;
}

bool MkPatchFileGenerator::SetUpdatingRootDirectoryPath(const MkPathName& updatingDirPath, const MkArray<MkStr>& destList)
{
	MkPathName targetDir;
	targetDir.ConvertToRootBasisAbsolutePath(updatingDirPath);
	if (!targetDir.CheckAvailable())
	{
		MkStr msg;
		msg.Reserve(512);
		msg += targetDir;
		msg += MkStr::LF;
		msg += L"updating root ������ �������� �ʽ��ϴ�.";
		msg += MkStr::LF;
		msg += MkStr::LF;
		msg += L"���� �� �ʱ�ȭ �Ͻðڽ��ϱ�?";
		
		int rlt = ::MessageBox(m_hWnd, msg.GetPtr(), L"MkPatchFileGenerator", MB_YESNO);
		if (rlt == IDYES)
		{
			targetDir.MakeDirectoryPath(); // ����
		}
		else if (rlt == IDNO)
		{
			targetDir.Clear();
			targetDir.GetDirectoryPathFromDialog(L"���� updating root ���� ����.", m_hWnd, MkPathName::GetRootDirectory()); // ����
		}
	}

	m_UpdatingRootPath = targetDir;
	bool ok = !m_UpdatingRootPath.Empty();
	if (ok)
	{
		MK_DEV_PANEL.MsgToLog(L"> " + m_UpdatingRootPath);
		MK_DEV_PANEL.MsgToLog(L"  updating root ���� ����.");
		MK_DEV_PANEL.InsertEmptyLine();

		m_UpdatingDestName = destList;
		if (m_UpdatingDestName.Empty())
		{
			m_UpdatingDestName.Reserve(2);
			m_UpdatingDestName.PushBack(L"Test");
			m_UpdatingDestName.PushBack(L"Service");
		}

		m_UpdatingDestPath.Clear();
		m_UpdatingDestPath.Reserve(m_UpdatingDestName.GetSize());
		MK_INDEXING_LOOP(m_UpdatingDestName, i)
		{
			MkPathName currPath = m_UpdatingRootPath + m_UpdatingDestName[i];
			currPath.CheckAndAddBackslash();
			m_UpdatingDestPath.PushBack(currPath);
		}

		m_UpdatingDataPath.Clear();
		m_UpdatingDataPath.Reserve(m_UpdatingDestPath.GetSize());
		MK_INDEXING_LOOP(m_UpdatingDestPath, i)
		{
			MkPathName currPath = m_UpdatingDestPath[i] + PatchDataDirName;
			m_UpdatingDataPath.PushBack(currPath);

			MK_DEV_PANEL.MsgToLog(L"> " + currPath);

			if (currPath.CheckAvailable())
			{
				MK_DEV_PANEL.MsgToLog(L"  updating data ���� ����.");
			}
			else
			{
				currPath.MakeDirectoryPath(); // ����
				MK_DEV_PANEL.MsgToLog(L"  updating data ������ ���� �����մϴ�.");
			}
			MK_DEV_PANEL.InsertEmptyLine();
		}
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(L">  updating root ���� ���� ����!!! Ȯ���� �ʿ��մϴ�.");
		MK_DEV_PANEL.InsertEmptyLine();
	}
	return ok;
}

bool MkPatchFileGenerator::GeneratePatchFiles(const MkPathName& sourceDirPath, bool tryCompress, const MkArray<MkStr>& doNotCompressExts)
{
	if (m_MainState != eReady)
		return false;

	m_TryCompress = tryCompress;

	m_DoNotCompressExts = doNotCompressExts;
	MK_INDEXING_LOOP(m_DoNotCompressExts, i)
	{
		m_DoNotCompressExts[i].ToLower();
	}

	bool ok = false;
	do
	{
		if (m_HistoryDirPath.Empty())
		{
			MK_DEV_PANEL.MsgToLog(L"> ������ history ������ �����ϴ�.");
			break;
		}

		if (m_PatchRootPath.Empty())
		{
			MK_DEV_PANEL.MsgToLog(L"> ������ patch root ������ �����ϴ�.");
			break;
		}

		if (m_UpdatingRootPath.Empty())
		{
			MK_DEV_PANEL.MsgToLog(L"> ������ updating root ������ �����ϴ�.");
			break;
		}

		// �� ���� �ð�
		CTime ct = CTime::GetCurrentTime();
		m_UpdateWrittenTime = MkPathName::ConvertWrittenTime(ct.GetYear(), ct.GetMonth(), ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond());

		// ���� Ȯ��
		if (!_LoadCurrentStructure(sourceDirPath))
			break;

		// ������ history Ȯ��
		m_LastHistory.Clear();
		unsigned int targetHistoryIndex = 0;

		if (m_HistoryList.Empty())
		{
			MK_DEV_PANEL.MsgToLog(L"> �ʱ� �����̹Ƿ� ���� ���� ������ �״�� ����մϴ�.");
			MK_DEV_PANEL.InsertEmptyLine();
		}
		else
		{
			targetHistoryIndex = m_HistoryList[0];
			if (targetHistoryIndex >= m_UpdateWrittenTime)
			{
				MK_DEV_PANEL.MsgToLog(L"> ���� �ð��� ��󺸴� �ֽ��̾�� �մϴ�.");
				MK_DEV_PANEL.MsgToLog(L"  ���� �ð� : " + ConvertWrittenTimeToStr(m_UpdateWrittenTime));
				MK_DEV_PANEL.MsgToLog(L"  ��� �ð� : " + ConvertWrittenTimeToStr(targetHistoryIndex));
				MK_DEV_PANEL.MsgToLog(L"  ������ ����մϴ�.");
				break;
			}

			if (!_LoadTargetHistoryFile(targetHistoryIndex))
				break;
		}

		// ������� ��� ����
		MkDataNode* updateNode = m_CurrentHistory.CreateChildNode(KEY_UpdateNode);
		if (updateNode == NULL)
		{
			MK_DEV_PANEL.MsgToLog(L"> " + KEY_UpdateNode.GetString() + L" ��� ���� ����.");
			break;
		}

		// purge list �ΰ�
		MkDataNode* lastUpdateNode = m_LastHistory.GetChildNode(KEY_UpdateNode);
		if (lastUpdateNode != NULL)
		{
			MkDataNode* purgeListNode = lastUpdateNode->GetChildNode(KEY_PurgeListNode);
			if ((purgeListNode != NULL) && (purgeListNode->GetChildNodeCount() > 0))
			{
				purgeListNode->DetachFromParentNode();
				updateNode->AttachChildNode(purgeListNode);
			}
			m_LastHistory.RemoveChildNode(KEY_UpdateNode); // �ش� ���� �� �̻� �ʿ� ����
			lastUpdateNode = NULL;
		}

		// �������� ��� ����
		if (!m_RunFilePath.Empty())
		{
			if (!m_CurrentHistory.CreateUnit(KEY_RunFilePath, m_RunFilePath))
			{
				MK_DEV_PANEL.MsgToLog(L"> " + KEY_RunFilePath.GetString() + L" unit ���� ����.");
				break;
			}
		}

		// �������(�߰�, ����, ����) Ž��
		if (!_FindDifferenceBetweenTwoNode
			(MkStr::EMPTY,
			(targetHistoryIndex == 0) ? NULL : m_LastHistory.GetChildNode(KEY_StructureNode),
			m_CurrentHistory.GetChildNode(KEY_StructureNode),
			*updateNode, m_UpdateWrittenTime))
		{
			MK_DEV_PANEL.MsgToLog(L"> ������� Ž�� ����.");
			break;
		}

		m_LastHistory.Clear(); // �� �̻� �ʿ� ����

		m_CurrentHistory.CreateUnit(KEY_CurrWT, m_UpdateWrittenTime);
		m_CurrentHistory.CreateUnit(KEY_LastWT, targetHistoryIndex);

		MK_DEV_PANEL.MsgToLog(L"> ���� ���� : " + ConvertWrittenTimeToStr(m_UpdateWrittenTime));
		
		ok = true; // ������� ���� ����

		MkStr msg;
		msg.Reserve(512);
		msg += L"���� ���� : ";
		msg += ConvertWrittenTimeToStr(m_UpdateWrittenTime);
		
		bool changed = false;
		changed |= _CheckUpdateType(*updateNode, KEY_AddNode, L"  - �߰� : ", msg);
		changed |= _CheckUpdateType(*updateNode, KEY_ReplaceNode, L"  - ���� : ", msg);
		changed |= _CheckUpdateType(*updateNode, KEY_DeleteNode, L"  - ���� : ", msg);
		msg += MkStr::LF;

		if (changed || m_HistoryList.Empty())
		{
			if (m_HistoryList.Empty())
			{
				msg += L"���� �ʱ� ���·� ";
			}

			msg += L"��ġ ������ ����ðڽ��ϱ�?";
			if (::MessageBox(m_hWnd, msg.GetPtr(), L"MkPatchFileGenerator", MB_YESNO) == IDNO)
			{
				MK_DEV_PANEL.MsgToLog(L"  ������ ����մϴ�.");
				break;
			}
		}
		else
		{
			msg += L"���� ���� ����. ��� ����."; // ���������� ���ǹ�
			::MessageBox(m_hWnd, msg.GetPtr(), L"MkPatchFileGenerator", MB_OK);

			MK_DEV_PANEL.MsgToLog(L"  ���� ���� ����. ��� ����.");
			break;
		}
		MK_DEV_PANEL.InsertEmptyLine();

		// ��ġ ���� ���� ����
		m_MainState = eStartPatchWorks;
		return ok;
	}
	while (false);

	m_CurrentHistory.Clear();
	m_LastHistory.Clear();
	m_UpdateWrittenTime = 0;

	MK_DEV_PANEL.InsertEmptyLine();
	return ok;
}

bool MkPatchFileGenerator::Update(void)
{
	bool onWorking = true;

	switch (m_MainState)
	{
	case eReady:
		onWorking = false;
		break;

	case eStartPatchWorks:
		{
			m_MainState = eMakePatchFile;
			m_UpdateType = eUpdateTypeAdd;

			m_TotalPatchFileCount = 0;
			m_DonePatchFileCount = 0;

			m_TotalPatchFileCount += _GetPatchFileListByType(KEY_AddNode, m_PatchFilePathList[eUpdateTypeAdd]);
			m_TotalPatchFileCount += _GetPatchFileListByType(KEY_ReplaceNode, m_PatchFilePathList[eUpdateTypeRep]);
			m_TotalPatchFileCount += _GetPatchFileListByType(KEY_DeleteNode, m_PatchFilePathList[eUpdateTypeDel]);

			MK_DEV_PANEL.MsgToLog(L"> working directory�� �����ϰ� backup�� ����ϴ�.");
		}
		break;

	case eMakePatchFile:
		{
			MkArray<MkHashStr>& currList = m_PatchFilePathList[m_UpdateType];
			if (currList.Empty())
			{
				switch (m_UpdateType)
				{
				case eUpdateTypeAdd: m_UpdateType = eUpdateTypeRep; break;
				case eUpdateTypeRep: m_UpdateType = eUpdateTypeDel; break;
				case eUpdateTypeDel: m_MainState = eUpdateLauncherInfo; break;
				}
			}
			else
			{
				MkStr typePrefix;
				bool copyOrDel;
				switch (m_UpdateType)
				{
				case eUpdateTypeAdd: typePrefix = L"  + "; copyOrDel = true; break;
				case eUpdateTypeRep: typePrefix = L"  * "; copyOrDel = true; break;
				case eUpdateTypeDel: typePrefix = L"  - "; copyOrDel = false; break;
				}
		
				// PopBack()�� ���� ���� ����
				MkPathName targetFilePath = currList[currList.GetSize() - 1].GetString(); 

				// updating�� ������ �������� �ٿ� �����ؾ� �ϱ� ������ ���� Ȯ���ڸ� ���
				MkPathName patchFilePath;
				ConvertFilePathToDownloadable(targetFilePath, patchFilePath);

				MkArray<MkPathName> destFilePath(1 + m_UpdatingDataPath.GetSize());
				destFilePath.PushBack(m_PatchDataPath + patchFilePath); // full patch data
				MK_INDEXING_LOOP(m_UpdatingDataPath, i)
				{
					destFilePath.PushBack(m_UpdatingDataPath[i] + patchFilePath); // updating patch data
				}

				MK_DEV_PANEL.MsgToLog(typePrefix + targetFilePath);

				bool ok = false;
				do
				{
					if (copyOrDel)
					{
						MkPathName srcFilePath = m_SourcePath + targetFilePath;

						// ������ ���� ���� �õ�
						MkByteArray srcData;
						MkInterfaceForFileReading frInterface;
						if (!frInterface.SetUp(srcFilePath))
						{
							MK_DEV_PANEL.MsgToLog(L"   ���� ���� ����. ������ ����մϴ�.");
							break;
						}

						unsigned int srcSize = (m_TryCompress) ? frInterface.Read(srcData, MkArraySection(0)) : frInterface.GetFileSize();
						frInterface.Clear();

						MkByteArray compData; // ���� �� ������
						if (m_TryCompress && (srcSize > 0))
						{
							bool extPass = true;
							if (!m_DoNotCompressExts.Empty())
							{
								MkStr ext = targetFilePath.GetFileExtension();
								ext.ToLower();
								extPass = !m_DoNotCompressExts.Exist(MkArraySection(0), ext);
							}

							if (extPass)
							{
								MkZipCompressor::Compress(srcData.GetPtr(), srcData.GetSize(), compData);
								srcData.Clear();

								if (ConvertToPercentage<unsigned int, unsigned int>(compData.GetSize(), srcSize) > 90)
								{
									compData.Clear(); // ���� �� ũ�Ⱑ ������ �������� ������(������� ������) ���� ���� �״�� ����
								}
							}
						}

						if (compData.Empty())
						{
							// ���� ���� �״�� ����
							bool error = false;
							MK_INDEXING_LOOP(destFilePath, i)
							{
								if (!srcFilePath.CopyCurrentFile(destFilePath[i], false))
								{
									error = true;
									break;
								}
							}
							if (error)
							{
								MK_DEV_PANEL.MsgToLog(L"   ���� �� ����. ������ ����մϴ�.");
								break;
							}
						}
						else
						{
							// ���� �����͸� ���
							bool error = false;
							MK_INDEXING_LOOP(destFilePath, i)
							{
								MkInterfaceForFileWriting fwInterface;
								if ((!fwInterface.SetUp(destFilePath[i], true, true)) ||
									(fwInterface.Write(compData, MkArraySection(0)) != compData.GetSize()))
								{
									error = true;
									break;
								}
							}
							if (error)
							{
								MK_DEV_PANEL.MsgToLog(L"   �������� ��� �� ����. ������ ����մϴ�.");
								break;
							}
							
							// ���� ũ�� ���� ����
							MkDataNode* fileInfo = _GetSourceFileInfoNode(targetFilePath);
							if (fileInfo == NULL)
							{
								MK_DEV_PANEL.MsgToLog(L"   �������� ���� ����� ���� ��尡 ����. ������ ����մϴ�.");
								break;
							}

							MkArray<unsigned int> sizeBuf(2);
							sizeBuf.PushBack(srcSize);
							sizeBuf.PushBack(compData.GetSize());
							if (!fileInfo->SetData(MkPathName::KeyFileSize, sizeBuf))
							{
								MK_DEV_PANEL.MsgToLog(L"   �������� ���� �� ���� ���� ����. ������ ����մϴ�.");
								break;
							}
						}
					}
					else
					{
						bool error = false;
						MK_INDEXING_LOOP(destFilePath, i)
						{
							bool success = destFilePath[i].DeleteCurrentFile();
							if ((i == 0) && (!success)) // full patch data������ �ݵ�� �������� ��
							{
								error = true;
								break;
							}
						}
						if (error)
						{
							MK_DEV_PANEL.MsgToLog(L"   ���� �� ����. ������ ����մϴ�.");
							break;
						}
					}

					currList.PopBack();
					++m_DonePatchFileCount;
					ok = true;
				}
				while (false);

				if (!ok)
				{
					MK_DEV_PANEL.InsertEmptyLine();
					m_MainState = eReady;
				}
			}
		}
		break;

	case eUpdateLauncherInfo:
		{
			MkDataNode* launcherNode = m_CurrentHistory.CreateChildNode(KEY_LauncherNode);
			if (launcherNode != NULL)
			{
				// structure node�� root�� m_LauncherFileName�� ���� ���� ��尡 �����ϸ�(��ġ�Ǿ�����) size, written time ����
				MkDataNode* structureNode = m_CurrentHistory.GetChildNode(KEY_StructureNode);
				if (structureNode != NULL)
				{
					MkDataNode* fileNode = structureNode->GetChildNode(m_LauncherFileName);
					if ((fileNode != NULL) && fileNode->IsValidKey(MkPathName::KeyFileSize) && fileNode->IsValidKey(MkPathName::KeyWrittenTime))
					{
						// deep copy
						*launcherNode = *fileNode;
					}
				}

				if (launcherNode->CreateUnit(LauncherFileName, m_LauncherFileName))
				{
					m_MainState = eReportPatchResult;
				}
				else
				{
					MK_DEV_PANEL.MsgToLog(L"> " + LauncherFileName.GetString() + L" unit ���� �� ����.");
					MK_DEV_PANEL.InsertEmptyLine();
					m_MainState = eReady;
				}
			}
			else
			{
				MK_DEV_PANEL.MsgToLog(L"> " + KEY_LauncherNode.GetString() + L" ��� ���� �� ����.");
				MK_DEV_PANEL.InsertEmptyLine();
				m_MainState = eReady;
			}
		}
		break;

	case eReportPatchResult:
		{
			// patch history file
			MkPathName historyFilePath = m_HistoryDirPath + MkStr(m_UpdateWrittenTime) + L"." + MkDataNode::DefaultFileExtension;
			m_CurrentHistory.SaveToBinary(historyFilePath);

			MK_DEV_PANEL.InsertEmptyLine();
			MK_DEV_PANEL.MsgToLog(L"> " + historyFilePath);
			MK_DEV_PANEL.MsgToLog(L"  ��ο� history ���� ���� �Ϸ�.");
			MK_DEV_PANEL.InsertEmptyLine();

			bool error = false;

			// launcher info file
			MkDataNode* launcherNode = m_CurrentHistory.GetChildNode(KEY_LauncherNode);
			if (launcherNode != NULL)
			{
				MkPathName launcherFileName;
				ConvertFilePathToDownloadable(LauncherFileName.GetString() + L"." + MkDataNode::DefaultFileExtension, launcherFileName);
				MkPathName launcherFilePath = m_PatchRootPath + launcherFileName;

				launcherNode->SaveToBinary(launcherFilePath);
				MK_DEV_PANEL.MsgToLog(L"> " + launcherFilePath);

				MK_INDEXING_LOOP(m_UpdatingDestPath, i)
				{
					MkPathName updatingDestPath = m_UpdatingDestPath[i] + launcherFileName;
					if (!launcherFilePath.CopyCurrentFile(updatingDestPath, false))
					{
						error = true;
						break;
					}
					MK_DEV_PANEL.MsgToLog(L"> " + updatingDestPath);
				}
				m_CurrentHistory.RemoveChildNode(KEY_LauncherNode);
			}

			// patch info file. m_CurrentHistory�� ������ ����
			// KEY_UpdateNode ���� KEY_PurgeListNode ��常 ������ ������ �������� ����
			if (!error)
			{
				m_CurrentHistory.SetDataTypeTag(DataType_PatchInfo);

				MkDataNode* updateNode = m_CurrentHistory.GetChildNode(KEY_UpdateNode);
				if (updateNode != NULL)
				{
					MkDataNode* purgeListNode = updateNode->GetChildNode(KEY_PurgeListNode);
					if ((purgeListNode != NULL) && (purgeListNode->GetChildNodeCount() > 0))
					{
						purgeListNode->DetachFromParentNode();
						m_CurrentHistory.AttachChildNode(purgeListNode);
					}
					m_CurrentHistory.RemoveChildNode(KEY_UpdateNode);
				}

				// file system ���� �ݿ�. root directory�� MkFileSystem::DefaultSettingFileName ���� �״�� ���
				MkPathName fsSettingFilePath;
				fsSettingFilePath.ConvertToRootBasisAbsolutePath(MkFileSystem::DefaultSettingFileName);
				if (fsSettingFilePath.CheckAvailable())
				{
					MkDataNode* fsSettingNode = m_CurrentHistory.CreateChildNode(KEY_FileSystemNode);
					if (fsSettingNode != NULL)
					{
						fsSettingNode->Load(fsSettingFilePath);
					}
				}

				// updating�� ������ �������� �ٿ� �����ؾ� �ϱ� ������ ���� Ȯ���ڸ� ���
				MkPathName infoFileName;
				ConvertFilePathToDownloadable(DataType_PatchInfo + L"." + MkDataNode::DefaultFileExtension, infoFileName);
				MkPathName infoFilePath = m_PatchRootPath + infoFileName;

				m_CurrentHistory.SaveToBinary(infoFilePath);
				MK_DEV_PANEL.MsgToLog(L"> " + infoFilePath);

				MK_INDEXING_LOOP(m_UpdatingDestPath, i)
				{
					MkPathName updatingDestPath = m_UpdatingDestPath[i] + infoFileName;
					if (!infoFilePath.CopyCurrentFile(updatingDestPath, false))
					{
						error = true;
						break;
					}
					MK_DEV_PANEL.MsgToLog(L"> " + updatingDestPath);
				}
			}

			if (error)
			{
				MK_DEV_PANEL.MsgToLog(L"  ��ο� ���� ���� ����. ������ ����մϴ�.");
			}
			else
			{
				MK_DEV_PANEL.MsgToLog(L"  ��ο� ���� ���� �Ϸ�.");

				m_HistoryList.Insert(0, m_UpdateWrittenTime); // history list�� ���

				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"-------------------------------------");
				MK_DEV_PANEL.MsgToLog(L"[ ��ġ ���� ������ �Ϸ�Ǿ����ϴ�. ]");
				MK_DEV_PANEL.MsgToLog(L"-------------------------------------");
			}
			MK_DEV_PANEL.InsertEmptyLine();

			m_PatchFilePathList[eUpdateTypeAdd].Clear();
			m_PatchFilePathList[eUpdateTypeRep].Clear();
			m_PatchFilePathList[eUpdateTypeDel].Clear();

			m_MainState = eReady;
		}
		break;
	}

	return onWorking;
}

MkStr MkPatchFileGenerator::ConvertWrittenTimeToStr(unsigned int writtenTime)
{
	int year, month, day, hour, min, sec;
	MkPathName::ConvertWrittenTime(writtenTime, year, month, day, hour, min, sec);

	MkStr msg;
	msg.Reserve(32);
	msg += writtenTime;
	msg += L" ("; msg += year; msg += L"."; msg += month; msg += L"."; msg += day; // (yyyy.mm.dd - hh:mm:ss)
	msg += L" - ";
	msg += hour; msg += L":"; msg += min; msg += L":"; msg += sec; msg += L")";
	return msg;
}

void MkPatchFileGenerator::ConvertFilePathToDownloadable(const MkPathName& filePath, MkPathName& buffer)
{
	buffer = filePath;
	buffer += L".";
	buffer += DownloadableFileExtension;
}

void MkPatchFileGenerator::IntersectionTest(const MkArray<MkHashStr>& sources, const MkArray<MkHashStr>& targets,
 MkArray<MkHashStr>& sourceOnly, MkArray<unsigned int>& sourceIntersection, MkArray<unsigned int>& targetIntersection, MkArray<MkHashStr>& targetOnly)
{
	unsigned int sourceSize = sources.GetSize();
	unsigned int targetSize = targets.GetSize();
	if ((sourceSize == 0) && (targetSize > 0))
	{
		targetOnly = targets;
		return;
	}
	else if ((sourceSize > 0) && (targetSize == 0))
	{
		sourceOnly = sources;
		return;
	}

	MkArray<bool> enable;
	enable.Fill(targetSize, true);

	sourceOnly.Reserve(sourceSize);
	unsigned int minSize = GetMin<unsigned int>(sourceSize, targetSize);
	sourceIntersection.Reserve(minSize);
	targetIntersection.Reserve(minSize);
	
	for (unsigned int i=0; i<sourceSize; ++i)
	{
		bool notFound = true;
		const MkHashStr& currSrc = sources[i];
		for (unsigned int j=0; j<targetSize; ++j)
		{
			if (enable[j]) // MkArray::IntersectionTest�ʹ� �޸� ��ҹ��ڸ� �������� ����
			{
				bool equals = true;

				const MkHashStr& currTgt = targets[j];
				if (currSrc != currTgt)
				{
					MkStr lowSrc = currSrc.GetString();
					lowSrc.ToLower();

					MkStr lowTgt = currTgt.GetString();
					lowTgt.ToLower();

					equals = (lowSrc == lowTgt);
				}

				if (equals)
				{
					sourceIntersection.PushBack(i);
					targetIntersection.PushBack(j);
					enable[j] = false;
					notFound = false;
					break;
				}
			}
		}
		if (notFound)
		{
			sourceOnly.PushBack(currSrc);
		}
	}

	targetOnly.Reserve(targetSize - sourceIntersection.GetSize());
	for (unsigned int i=0; i<targetSize; ++i) // ��ġ���� ���� �������� targetOnly�� ���
	{
		if (enable[i])
		{
			targetOnly.PushBack(targets[i]);
		}
	}
}

MkPatchFileGenerator::MkPatchFileGenerator()
{
	m_hWnd = NULL;
	m_MainState = eReady;
	m_UpdateWrittenTime = 0;
	m_TotalPatchFileCount = 0;
	m_DonePatchFileCount = 0;
	m_TryCompress = true;
}

//------------------------------------------------------------------------------------------------//

bool MkPatchFileGenerator::_LoadCurrentStructure(const MkPathName& sourceDirPath)
{
	m_CurrentHistory.Clear();
	MkDataNode* structureNode = m_CurrentHistory.CreateChildNode(KEY_StructureNode);
	if (structureNode == NULL)
	{
		MK_DEV_PANEL.MsgToLog(KEY_StructureNode.GetString() + L" ��� ���� ����.");
		return false;
	}
	
	m_SourcePath.Clear();
	m_SourcePath.ConvertToRootBasisAbsolutePath(sourceDirPath);
	bool ok = (m_SourcePath.ExportSystemStructure(*structureNode) != 0);
	if (ok)
	{
		m_CurrentHistory.SetDataTypeTag(DataType_PatchHistory); // tag ����
		
		MK_DEV_PANEL.MsgToLog(L"> " + m_SourcePath);
		MK_DEV_PANEL.MsgToLog(L"  ��ġ�� �� ���� ���� ���� ���� �Ϸ�.");
	}
	else
	{
		if (m_SourcePath.IsDirectoryPath() && m_SourcePath.CheckAvailable())
		{
			// ���丮�� ���������� ������ ����. ���� �ʱ� ����
			MkStr msg;
			msg.Reserve(512);
			msg += m_SourcePath;
			msg += MkStr::LF;
			msg += L"��ġ�� ���� ������ �����ϴ�.";
			msg += MkStr::LF;
			msg += MkStr::LF;
			msg += L"�ùٸ� �����Դϱ�?";
			
			int rlt = ::MessageBox(m_hWnd, msg.GetPtr(), L"MkPatchFileGenerator", MB_YESNO);
			if (rlt == IDYES)
			{
				m_CurrentHistory.SetDataTypeTag(DataType_PatchHistory); // tag ����

				ok = true;
			}
			else if (rlt == IDNO)
			{
				MK_DEV_PANEL.MsgToLog(L"> " + m_SourcePath);
				MK_DEV_PANEL.MsgToLog(L"  ��ġ�� �� ���� ������ �����ϴ�. Ȯ���� �ʿ��մϴ�.");
			}
		}
		else
		{
			MK_DEV_PANEL.MsgToLog(L"> " + m_SourcePath);
			MK_DEV_PANEL.MsgToLog(L"  ��ġ�� �߸��Ǿ����ϴ�. Ȯ���� �ʿ��մϴ�.");
		}
	}
	MK_DEV_PANEL.InsertEmptyLine();
	return ok;
}

bool MkPatchFileGenerator::_LoadTargetHistoryFile(unsigned int targetIndex)
{
	do
	{
		if (!m_HistoryList.Exist(MkArraySection(0), targetIndex))
		{
			MK_DEV_PANEL.MsgToLog(L"> " + MkStr(targetIndex) + L" ��� history�� �������� �ʽ��ϴ�.");
			break;
		}

		MkPathName targetFilePath = m_HistoryDirPath + MkStr(targetIndex) + L"." + MkDataNode::DefaultFileExtension;
		MK_DEV_PANEL.MsgToLog(L"> " + targetFilePath);

		if (!m_LastHistory.Load(targetFilePath))
		{
			MK_DEV_PANEL.MsgToLog(L"  ��� history ������ ���ų� MkDataNode ������ �ƴմϴ�.");
			break;
		}

		if (m_LastHistory.GetDataTypeTag() != DataType_PatchHistory)
		{
			MK_DEV_PANEL.MsgToLog(L"  ��� history ������ �ùٸ� ������ ������ �ƴմϴ�.");
			break;
		}

		if (!m_LastHistory.ChildExist(KEY_StructureNode))
		{
			MK_DEV_PANEL.MsgToLog(L"  ��� history ���Ͽ� " + KEY_StructureNode.GetString() + L" ��尡 �����ϴ�.");
			break;
		}

		if (!m_LastHistory.ChildExist(KEY_UpdateNode))
		{
			MK_DEV_PANEL.MsgToLog(L"  ��� history ���Ͽ� " + KEY_UpdateNode.GetString() + L" ��尡 �����ϴ�.");
			break;
		}
		
		MK_DEV_PANEL.MsgToLog(L"  ��� history ���� �ε� �Ϸ� : " + ConvertWrittenTimeToStr(targetIndex));
		MK_DEV_PANEL.InsertEmptyLine();
		return true;
	}
	while (false);

	MK_DEV_PANEL.InsertEmptyLine();
	m_LastHistory.Clear();
	return false;
}

unsigned int MkPatchFileGenerator::_GetPatchFileListByType(const MkHashStr& key, MkArray<MkHashStr>& buffer)
{
	buffer.Clear();

	const MkDataNode* updateRootNode = m_CurrentHistory.GetChildNode(KEY_UpdateNode);
	if (updateRootNode != NULL)
	{
		const MkDataNode* updateTypeNode = updateRootNode->GetChildNode(key);
		if (updateTypeNode != NULL)
		{
			return updateTypeNode->GetChildNodeList(buffer);
		}
	}
	return 0;
}

MkDataNode* MkPatchFileGenerator::_GetSourceFileInfoNode(const MkPathName& filePath)
{
	MkDataNode* structureNode = m_CurrentHistory.GetChildNode(KEY_StructureNode);
	if (structureNode != NULL)
	{
		MkArray<MkStr> tokens;
		filePath.Tokenize(tokens, L"\\"); // ������ ����

		MkArray<MkHashStr> keys(tokens.GetSize());
		MK_INDEXING_LOOP(tokens, i)
		{
			keys.PushBack(tokens[i]);
		}
		return structureNode->GetChildNode(keys);
	}
	return NULL;
}

bool MkPatchFileGenerator::_CheckUpdateType(const MkDataNode& updateRoot, const MkHashStr& key, const MkStr& msgPrefix, MkStr& msg)
{
	const MkDataNode* typeNode = updateRoot.GetChildNode(key);
	if (typeNode != NULL)
	{
		unsigned int count = typeNode->GetChildNodeCount();
		bool changed = (count > 0);
		if (changed)
		{
			MkStr buffer = msgPrefix + MkStr(count) + L" �� ����.";
			MK_DEV_PANEL.MsgToLog(buffer);

			msg += MkStr::LF;
			msg += buffer;
		}
		return changed;
	}
	return false;
}

bool MkPatchFileGenerator::_FindDirectoryDifference
(const MkStr& dirPath, MkArray<MkHashStr>& dirList, const MkDataNode* lastDirNode, MkDataNode* currDirNode, MkDataNode& updateRoot, unsigned int& writtenTime)
{
	if (!dirList.Empty())
	{
		MK_INDEXING_LOOP(dirList, i)
		{
			const MkHashStr dirName = dirList[i];
			MkStr targetPath = dirPath;
			targetPath += dirName.GetString(); // targetPath(root�κ����� ��� ���)�� unique���� ���� ��
			targetPath += L"\\";

			if (!_FindDifferenceBetweenTwoNode
				(targetPath,
				(lastDirNode == NULL) ? NULL : lastDirNode->GetChildNode(dirName),
				(currDirNode == NULL) ? NULL : currDirNode->GetChildNode(dirName),
				updateRoot, writtenTime))
				return false;
		}
		dirList.Clear();
	}
	return true;
}

bool MkPatchFileGenerator::_FindDifferenceBetweenTwoNode
(const MkStr& dirPath, const MkDataNode* lastDirNode, MkDataNode* currDirNode, MkDataNode& updateRoot, unsigned int& writtenTime)
{
	bool hasLastDir = (lastDirNode != NULL);
	bool hasCurrDir = (currDirNode != NULL);

	// ���丮 ��ü�� ���� ���� -> currDirNode ���� ��� ���� add
	if ((!hasLastDir) && hasCurrDir)
	{
		if (!_SetAllDirsToUpdate(dirPath, *currDirNode, updateRoot, KEY_AddNode, true, -1, writtenTime))
			return false;
	}
	// ���丮 ��ü�� ����� -> lastDirNode ���� ��� ���� delete
	else if (hasLastDir && (!hasCurrDir))
	{
		if (!_SetAllDirsToUpdate(dirPath, *lastDirNode, updateRoot, KEY_DeleteNode, false, 1, writtenTime)) // delete�� file size, written time ���ʿ�
			return false;
	}
	// ���� ���丮 -> ���� ���� ��
	else if (hasLastDir && hasCurrDir)
	{
		// [ ���� �� ]
		MkArray<MkHashStr> lastObjects, currObjects;
		MkPathName::__GetSubFiles(*lastDirNode, lastObjects);
		MkPathName::__GetSubFiles(*currDirNode, currObjects);

		// ���� ����. ��ҹ��� �������� ����
		MkArray<MkHashStr> delList, addList;
		MkArray<unsigned int> lastIntersection, currIntersection;
		IntersectionTest(lastObjects, currObjects, delList, lastIntersection, currIntersection, addList);
		
		// delList, addList�ʹ� �޸� intersection�� ũ��� �����ð��� ���� ���� ����
		MkArray<MkHashStr> repList(lastIntersection.GetSize());
		MK_INDEXING_LOOP(lastIntersection, i)
		{
			const MkHashStr& lastFileName = lastObjects[lastIntersection[i]];
			const MkHashStr& currFileName = currObjects[currIntersection[i]];
			if (MkPathName::__CheckFileDifference(*lastDirNode->GetChildNode(lastFileName), *currDirNode->GetChildNode(currFileName)))
			{
				repList.PushBack(currFileName); // ���� ���ϸ� ����
			}
		}

		lastObjects.Clear();
		currObjects.Clear();
		lastIntersection.Clear();
		currIntersection.Clear();

		// delete
		if (!_SetAllFilesToUpdate(dirPath, *lastDirNode, delList, updateRoot, KEY_DeleteNode, false, 1, writtenTime))
			return false;
		delList.Clear();

		// replace
		if (!_SetAllFilesToUpdate(dirPath, *currDirNode, repList, updateRoot, KEY_ReplaceNode, true, 0, writtenTime))
			return false;
		repList.Clear();

		// add
		if (!_SetAllFilesToUpdate(dirPath, *currDirNode, addList, updateRoot, KEY_AddNode, true, -1, writtenTime))
			return false;
		addList.Clear();

		// [ ���丮 �� ]
		MkPathName::__GetSubDirectories(*lastDirNode, lastObjects);
		MkPathName::__GetSubDirectories(*currDirNode, currObjects);

		// ������ ���� ����. ��ҹ��� �������� ����
		IntersectionTest(lastObjects, currObjects, delList, lastIntersection, currIntersection, addList);
		delList.Clear();
		addList.Clear();

		// ���� ����. ��ҹ��� ���� ��!!!
		MkArray<MkHashStr> intersection;
		lastObjects.IntersectionTest(currObjects, delList, intersection, addList);

		// ��ҹ��� ���� ��/�������� ���� ���� ũ�Ⱑ �ٸ��ٸ� ���� ��λ� ��ҹ��ڰ� �ٸ� ���丮���� �ִٴ� ��
		if (intersection.GetSize() != lastIntersection.GetSize())
		{
			if (dirPath.Empty())
			{
				MK_DEV_PANEL.MsgToLog(L"> ���� ��� ��Ʈ�� �������� ��ҹ��ڰ� �ٸ� ���丮�� ����.");
			}
			else
			{
				MK_DEV_PANEL.MsgToLog(L"> " + dirPath);
				MK_DEV_PANEL.MsgToLog(L"  ���� ��� ������ �������� ��ҹ��ڰ� �ٸ� ���丮�� ����.");
			}
			return false;
		}

		lastIntersection.Clear();
		currIntersection.Clear();
		lastObjects.Clear();
		currObjects.Clear();

		// delete
		if (!_FindDirectoryDifference(dirPath, delList, lastDirNode, NULL, updateRoot, writtenTime))
			return false;

		// check difference
		if (!_FindDirectoryDifference(dirPath, intersection, lastDirNode, currDirNode, updateRoot, writtenTime))
			return false;

		// add
		if (!_FindDirectoryDifference(dirPath, addList, NULL, currDirNode, updateRoot, writtenTime))
			return false;
	}
	return true;
}

bool MkPatchFileGenerator::_SetAllFilesToUpdate
(const MkStr& dirPath, const MkDataNode& targetDirNode, const MkArray<MkHashStr>& fileList, MkDataNode& updateRoot,
 const MkHashStr& updateType, bool copySrcData, int purgeState, unsigned int& writtenTime)
{
	if (!fileList.Empty())
	{
		MkDataNode* updateTypeRoot = updateRoot.ChildExist(updateType) ? updateRoot.GetChildNode(updateType) : updateRoot.CreateChildNode(updateType);
		if (updateTypeRoot == NULL)
		{
			::MessageBox(NULL, L"�޸� ����, update type node ���� ���� : " + updateType.GetString(), L"Error", MB_OK);
			return false;
		}

		MK_INDEXING_LOOP(fileList, i)
		{
			MkStr targetPath = dirPath;
			targetPath += fileList[i].GetString(); // targetPath(root�κ����� ��� ���)�� ���ϼ��� ���� ��
			MkDataNode* fileNode = updateTypeRoot->CreateChildNode(targetPath); // file node�� ����
			if (fileNode == NULL)
			{
				::MessageBox(NULL, L"�޸� ����, file node ���� ���� :\n" + targetPath, L"Error", MB_OK);
				return false;
			}

			if (purgeState != 0)
			{
				MkDataNode* purgeTypeRoot = updateRoot.ChildExist(KEY_PurgeListNode) ? updateRoot.GetChildNode(KEY_PurgeListNode) : updateRoot.CreateChildNode(KEY_PurgeListNode);
				if (purgeTypeRoot == NULL)
				{
					::MessageBox(NULL, L"�޸� ����, update type node ���� ���� : " + KEY_PurgeListNode.GetString(), L"Error", MB_OK);
					return false;
				}

				if (purgeState > 0) // delete
				{
					if (purgeTypeRoot->CreateChildNode(targetPath) == NULL)
					{
						::MessageBox(NULL, L"�޸� ����, purge node ���� ���� :\n" + targetPath, L"Error", MB_OK);
						return false;
					}
				}
				else if (purgeState < 0) // add
				{
					purgeTypeRoot->RemoveChildNode(targetPath);
				}
			}

			if (copySrcData)
			{
				*fileNode = *targetDirNode.GetChildNode(fileList[i]); // deep copy�� file size, written time ������ ����

				unsigned int wt = 0;
				fileNode->GetData(MkPathName::KeyWrittenTime, wt, 0);
				writtenTime = GetMax<unsigned int>(writtenTime, wt);
			}
		}
	}
	return true;
}

bool MkPatchFileGenerator::_SetAllDirsToUpdate
(const MkStr& dirPath, const MkDataNode& targetDirNode, MkDataNode& updateRoot, const MkHashStr& updateType, bool copySrcData, int purgeState, unsigned int& writtenTime)
{
	MkArray<MkHashStr> subObjects;
	MkPathName::__GetSubFiles(targetDirNode, subObjects); // ���� ��� ���� ���
	if (!_SetAllFilesToUpdate(dirPath, targetDirNode, subObjects, updateRoot, updateType, copySrcData, purgeState, writtenTime))
		return false;

	subObjects.Clear();
	MkPathName::__GetSubDirectories(targetDirNode, subObjects); // ���� ��� ���丮 ���
	if (!subObjects.Empty())
	{
		MK_INDEXING_LOOP(subObjects, i)
		{
			MkStr targetPath = dirPath;
			targetPath += subObjects[i].GetString(); // targetPath(root�κ����� ��� ���)�� unique���� ���� ��
			targetPath += L"\\";
			if (!_SetAllDirsToUpdate(targetPath, *targetDirNode.GetChildNode(subObjects[i]), updateRoot, updateType, copySrcData, purgeState, writtenTime)) // ���
				return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------//
