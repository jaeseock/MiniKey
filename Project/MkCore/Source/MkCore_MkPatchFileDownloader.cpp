
#include <Windows.h>
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkTimeManager.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkZipCompressor.h"
#include "MkCore_MkFileDownloader.h"
#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkPatchFileDownloader.h"


const MkStr MkPatchFileDownloader::TemporaryRepositoryDirName(L"__TempRep\\");
const MkStr MkPatchFileDownloader::PatchLegacyFileName(L"MkPatchLegacy");

const static MkHashStr MKDEF_PL_FWT_NODE_NAME = L"[FileWrittenTime]";


//------------------------------------------------------------------------------------------------//

bool MkPatchFileDownloader::CheckAndDownloadPatchFile(const MkStr& url, bool useFileSystem)
{
	bool ok = (m_MainState == eReady);
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

		m_UseFileSystem = useFileSystem;

		m_PatchLegacyNode.Clear();
		m_PatchLegacyFilePath = MkPathName::GetRootDirectory() + MkPatchFileDownloader::PatchLegacyFileName + L"." + MkDataNode::DefaultFileExtension; // ex> D:\\Client\\MkPatchLegacy.mmd
		if (m_PatchLegacyFilePath.CheckAvailable())
		{
			m_PatchLegacyNode.Load(m_PatchLegacyFilePath);
		}

		m_ErrorMsg.Clear();
		m_MainState = eDownloadPatchInfoFile;
		m_TotalDownloadSize = 0;
		m_SuccessDownloadSize = 0;
		m_CurrentDownloadSize = 0;
		m_MaxProgress = 0;
		m_CurrentProgress = 0;
		m_LastDownloadSizeOnTick = 0;
	}
	return ok;
}

void MkPatchFileDownloader::Update(void)
{
	switch (m_MainState)
	{
	case eReady: break;

	case eDownloadPatchInfoFile:
		{
			MkStr infoFileName = MkPatchFileGenerator::DataType_PatchInfo + L"." + MkDataNode::DefaultFileExtension; // ex> PatchInfo.mmd
			MkPathName infoFileNameDown;
			MkPatchFileGenerator::ConvertFilePathToDownloadable(infoFileName, infoFileNameDown); // ex> PatchInfo.mmd.rp

			// local temp dir
			MkPathName localTempDir = MkPathName::GetRootDirectory() + TemporaryRepositoryDirName; // ex> D:\\Client\\__TempRep\\

			// down info file
			MkStr urlInfoFilePath = m_RootURL + infoFileNameDown; // ex> http://210.207.252.151/Test/PatchInfo.mmd.rp
			MkPathName localInfoFilePath = localTempDir + infoFileName; // D:\\Client\\__TempRep\\PatchInfo.mmd

			if (!MkFileDownloader::DownloadFileRightNow(urlInfoFilePath, localInfoFilePath))
			{
				m_ErrorMsg = urlInfoFilePath + L"\n��Ʈ��ũ ������ �ٿ��� �����߽��ϴ�.";
			}

			if (m_ErrorMsg.Empty())
			{
				MK_DEV_PANEL.MsgToLog(L"> " + urlInfoFilePath);
				MK_DEV_PANEL.MsgToLog(L"  ���� ���� �ٿ� ����.");
				MK_DEV_PANEL.InsertEmptyLine();

				// load info file
				m_PatchInfoNode.Clear();
				if (m_PatchInfoNode.Load(localInfoFilePath))
				{
					unsigned int lastWT = 0;
					if ((m_PatchInfoNode.GetDataTypeTag() == MkPatchFileGenerator::DataType_PatchInfo) &&
						m_PatchInfoNode.ChildExist(MkPatchFileGenerator::KEY_StructureNode) &&
						m_PatchInfoNode.GetData(MkPatchFileGenerator::KEY_CurrWT, lastWT, 0))
					{
						MK_DEV_PANEL.MsgToLog(L"> " + localInfoFilePath);
						MK_DEV_PANEL.MsgToLog(L"  ���� ���� �ε� ���� : " + MkPatchFileGenerator::ConvertWrittenTimeToStr(lastWT));
						MK_DEV_PANEL.InsertEmptyLine();

						MkStr rfp;
						if (m_PatchInfoNode.GetData(MkPatchFileGenerator::KEY_RunFilePath, rfp, 0))
						{
							m_RunFilePath.ConvertToRootBasisAbsolutePath(rfp);

							MK_DEV_PANEL.MsgToLog(L"> " + m_RunFilePath);
							MK_DEV_PANEL.MsgToLog(L"  ���� ���� ��� ����");
							MK_DEV_PANEL.InsertEmptyLine();
						}
					}
					else
						m_ErrorMsg = urlInfoFilePath + L"\n��ġ�� ������ �������� ���� ������ �ƴմϴ�.";
				}
				else
					m_ErrorMsg = urlInfoFilePath + L"\n��ġ�� ���� ���� �б� ����.";
			}

			if (m_ErrorMsg.Empty())
			{
				m_TempDataPath = localTempDir + MkPatchFileGenerator::PatchDataDirName; // ex> D:\\Client\\__TempRep\\data\\

				m_MainState = ePurgeDeleteList;
			}
			else
			{
				MK_DEV_PANEL.MsgToLog(m_ErrorMsg);
				MK_DEV_PANEL.InsertEmptyLine();

				m_MainState = eShowFailedResult;
			}
		}
		break;

	case ePurgeDeleteList:
		{
			MkDataNode* purgeNode = m_PatchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_PurgeListNode);
			if (purgeNode != NULL)
			{
				MkArray<MkHashStr> keys;
				purgeNode->GetChildNodeList(keys);

				if (!keys.Empty())
				{
					MK_DEV_PANEL.MsgToLog(L"> ���� ���� �ִ� " + MkStr(keys.GetSize()) + L" �� ����");

					MK_INDEXING_LOOP(keys, i)
					{
						MkPathName targetFilePath = keys[i].GetString();

						// ���� ���� ������ ����
						MkPathName absoluteFilePath;
						absoluteFilePath.ConvertToRootBasisAbsolutePath(targetFilePath);
						absoluteFilePath.DeleteCurrentFile();

						// file system�� ������ ����
						MkFileManager::Instance().GetFileSystem().RemoveFile(targetFilePath);

						// ��ġ �ܿ� ������ ������ ����
						MkDataNode* fwtNode = m_PatchLegacyNode.GetChildNode(MKDEF_PL_FWT_NODE_NAME);
						if (fwtNode != NULL)
						{
							MkStr lowerKey = targetFilePath;
							lowerKey.ToLower();
							fwtNode->RemoveChildNode(MkHashStr(lowerKey));
						}
					}

					MkDataNode* fwtNode = m_PatchLegacyNode.GetChildNode(MKDEF_PL_FWT_NODE_NAME);
					if ((fwtNode != NULL) && (fwtNode->GetChildNodeCount() == 0))
					{
						m_PatchLegacyNode.RemoveChildNode(MKDEF_PL_FWT_NODE_NAME);
					}

					MK_DEV_PANEL.InsertEmptyLine();
				}

				// purge node�� �� �̻� �ʿ� ����
				purgeNode->DetachFromParentNode();
				delete purgeNode;
			}

			m_MainState = eFindDownloadTargets;
		}
		break;

	case eFindDownloadTargets:
		{
			// client root�� file system ������ �����Ѵٸ� �ݿ�
			MkDataNode* fsSettingNode = m_PatchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_FileSystemNode);
			if (fsSettingNode != NULL)
			{
				MkFileManager::Instance().GetFileSystem().SetSystemSetting(*fsSettingNode);

				// �� �̻� �ʿ� ����
				fsSettingNode->DetachFromParentNode();
				delete fsSettingNode;

				MK_DEV_PANEL.MsgToLog(L"> ���� �ý��� ���� ����");
				MK_DEV_PANEL.InsertEmptyLine();
			}

			// structure �������� �ٿ���� ���� �˻�(structure node ���翩�δ� ������ �˻�)
			const MkDataNode& structureNode = *m_PatchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_StructureNode);
			unsigned int totalFiles = MkPathName::__CountTotalFiles(structureNode);
			MK_DEV_PANEL.MsgToLog(L"> ���� ��ġ ���� : " + MkStr(totalFiles) + L" ��");

			m_DownloadFileInfoList.Clear();
			m_DownloadFileInfoList.Reserve(totalFiles);

			// module file name
			wchar_t fullPath[MAX_PATH];
			::GetModuleFileName(NULL, fullPath, MAX_PATH);
			MkPathName tmpBuf = fullPath;
			MkPathName moduleFileName = tmpBuf.GetFileName();
			moduleFileName.ToLower();

			_FindFilesToDownload(structureNode, MkStr::EMPTY, moduleFileName); // �˻�

			// �ѷ� ���
			MK_INDEXING_LOOP(m_DownloadFileInfoList, i)
			{
				const _DownloadFileInfo& fi = m_DownloadFileInfoList[i];
				m_TotalDownloadSize += static_cast<unsigned __int64>((fi.compSize > 0) ? fi.compSize : fi.origSize);
			}
			m_SuccessDownloadSize = 0;
			m_CurrentDownloadSize = 0;
			
			MK_DEV_PANEL.MsgToLog(L"> �ٿ� ��� ���� : " + MkStr(m_DownloadFileInfoList.GetSize()) + L" ��");

			MkStr sizeBuf;
			_ConvertDataSizeToString(m_TotalDownloadSize, sizeBuf);
			MK_DEV_PANEL.MsgToLog(L"> �ٿ� ��� ũ�� : " + sizeBuf);
			MK_DEV_PANEL.InsertEmptyLine();
			MK_DEV_PANEL.MsgToLog(L"> ���� �ٿ��� �����մϴ�.");

			m_MainState = eRegisterTargetFiles;
			m_MaxProgress = m_DownloadFileInfoList.GetSize();
			m_CurrentProgress = 0;

			MkTimeState timeState;
			MK_TIME_MGR.GetCurrentTimeState(timeState);
			m_DownloadTickCounter.SetUp(timeState, 3.);
			m_LastDownloadSizeOnTick = 0;
			m_RemainCompleteTimeStr = L"--";
		}
		break;

	case eRegisterTargetFiles:
		{
			if (m_CurrentProgress == m_MaxProgress)
			{
				MK_DEV_PANEL.MsgToLog(L"> ���� �ٿ��� �Ϸ�Ǿ����ϴ�.");
				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"> ��ġ ������ �����մϴ�.");

				m_CurrentDownloadSize = m_SuccessDownloadSize = m_TotalDownloadSize;
				m_RemainCompleteTimeStr.Clear();
				m_CurrentProgress = 0;
				m_MainState = eUpdateFiles;
			}
			else
			{
				// �̹� �ٿ���� ���� ��ŵ
				const _DownloadFileInfo& fileInfo = m_DownloadFileInfoList[m_CurrentProgress];
				if (fileInfo.alreadyDowned)
				{
					MK_DEV_PANEL.MsgToLog(L"    " + fileInfo.filePath);
					++m_CurrentProgress;

					// �ٿ� ���� ũ�� ����
					m_SuccessDownloadSize += static_cast<unsigned __int64>((fileInfo.compSize > 0) ? fileInfo.compSize : fileInfo.origSize);
				}
				else
				{
					// url
					MkPathName relPathInServer, relPathInClient;
					MkPatchFileGenerator::ConvertFilePathToDownloadable(fileInfo.filePath, relPathInServer); // ex> AAA\\BBB\\abc.mmd -> AAA\\BBB\\abc.mmd.rp
					relPathInClient = relPathInServer;
					relPathInServer.ReplaceKeyword(L"\\", L"/"); // ex> AAA/BBB/abc.mmd.rp
					MkStr urlPath = m_DataRootURL + relPathInServer; // ex> http://210.207.252.151/Test/data/AAA/BBB/abc.mmd.rp
					MK_DEV_PANEL.MsgToLog(L" �� " + relPathInServer);

					// local. ���� ���ο� ���� �״�� �ٿ�, Ȥ�� �ٿ� �� ��������
					MkPathName localFilePathForComp = m_TempDataPath + relPathInClient; // ����. ex> D:\\Client\\__TempRep\\data\\AAA\\BBB\\abc.mmd.rp
					MkPathName localFilePathForOrig = m_TempDataPath + fileInfo.filePath; // �����. ex> D:\\Client\\__TempRep\\data\\AAA\\BBB\\abc.mmd
					bool compressed = (fileInfo.compSize > 0);

					// down
					m_CurrentDownInfo = MK_FILE_DOWNLOADER.DownloadFile(urlPath, (compressed) ? localFilePathForComp : localFilePathForOrig);
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

			_UpdateDownloadState();
		}
		break;

	case eDownloadTargetFiles:
		{
			MkFileDownInfo::eDownState downState = m_CurrentDownInfo->GetDownState();
			if ((downState == MkFileDownInfo::eDS_Wait) || (downState == MkFileDownInfo::eDS_Downloading))
			{
				_UpdateDownloadState();

				::Sleep(1); // ���
				break;
			}
			else if (downState == MkFileDownInfo::eDS_Complete) // �Ϸ�
			{
				_DownloadFileInfo& fileInfo = m_DownloadFileInfoList[m_CurrentProgress];

				MkPathName localFilePathForOrig = m_TempDataPath + fileInfo.filePath; // �����. ex> D:\\Client\\__TempRep\\data\\AAA\\BBB\\abc.mmd
				bool compressed = (fileInfo.compSize > 0);

				do
				{
					// uncompress!!! -> WT ����!!!!
					if (compressed)
					{
						MkPathName relPathInClient;
						MkPatchFileGenerator::ConvertFilePathToDownloadable(fileInfo.filePath, relPathInClient); // ex> AAA\\BBB\\abc.mmd -> AAA\\BBB\\abc.mmd.rp
						MkPathName localFilePathForComp = m_TempDataPath + relPathInClient; // ����. ex> D:\\Client\\__TempRep\\data\\AAA\\BBB\\abc.mmd.rp

						// ���� ���� �о�鿩
						MkByteArray compData;
						MkInterfaceForFileReading frInterface;
						if (!frInterface.SetUp(localFilePathForComp))
						{
							m_ErrorMsg = localFilePathForComp + L"\n���� ���� ����.";
							break;
						}
						if (frInterface.Read(compData, MkArraySection(0)) != fileInfo.compSize)
						{
							m_ErrorMsg = localFilePathForComp + L"\n�б� �����ų� �ٿ�� ���� ���� ũ�Ⱑ �ٸ��ϴ�.";
							break;
						}
						frInterface.Clear();

						// ���� ����
						MkByteArray origData;
						if (MkZipCompressor::Uncompress(compData.GetPtr(), compData.GetSize(), origData) != fileInfo.origSize)
						{
							m_ErrorMsg = localFilePathForComp + L"\n���� ���� �� ũ�Ⱑ �ٸ��ϴ�";
							break;
						}
						compData.Clear();

						// ���Ϸ� ���
						MkInterfaceForFileWriting fwInterface;
						if (!fwInterface.SetUp(localFilePathForOrig, true, true))
						{
							m_ErrorMsg = localFilePathForOrig + L"\n���� ���� ����.";
							break;
						}
						if (fwInterface.Write(origData, MkArraySection(0)) != fileInfo.origSize)
						{
							m_ErrorMsg = localFilePathForOrig + L"\n���� ����.";
							break;
						}
						fwInterface.Clear();
						origData.Clear();

						// ���� ���� ����
						localFilePathForComp.DeleteCurrentFile();

						// �ٿ� ���� ũ�� ����
						m_SuccessDownloadSize += fileInfo.compSize;
					}
					else
					{
						// ũ�⸸ �˻�
						if (localFilePathForOrig.GetFileSize() != fileInfo.origSize)
						{
							m_ErrorMsg = localFilePathForOrig + L"\n�ٿ�� ���� ���� ũ�Ⱑ �ٸ��ϴ�. �´� ũ�� : " + MkStr(fileInfo.origSize);
							break;
						}

						// �ٿ� ���� ũ�� ����
						m_SuccessDownloadSize += fileInfo.origSize;
					}

					// ���� �����ð� ���� �õ�. ���� ���ɼ� ����
					localFilePathForOrig.SetWrittenTime(fileInfo.writtenTime);
				}
				while (false);

				_UpdateDownloadState();
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
				MK_DEV_PANEL.MsgToLog(L"  " + m_ErrorMsg);

				m_MainState = eShowFailedResult;
				m_MaxProgress = 0;
				m_CurrentProgress = 0;
			}

			// info ����
			delete m_CurrentDownInfo;
			m_CurrentDownInfo = NULL;
		}
		break;

	case eUpdateFiles:
		{
			if (m_CurrentProgress == m_MaxProgress)
			{
				if (m_PatchLegacyNode.GetChildNodeCount() > 0)
				{
					m_PatchLegacyNode.SaveToBinary(m_PatchLegacyFilePath);

					MK_DEV_PANEL.MsgToLog(L"> " + m_PatchLegacyFilePath);
					MK_DEV_PANEL.MsgToLog(L"  ��ġ �ܿ� ������ ����߽��ϴ�.");
					MK_DEV_PANEL.InsertEmptyLine();
				}

				MkPathName localTempDir = MkPathName::GetRootDirectory() + TemporaryRepositoryDirName; // ex> D:\\Client\\__TempRep
				localTempDir.DeleteCurrentDirectory(true);

				MK_DEV_PANEL.MsgToLog(L"> ���� ��ġ�� �Ϸ�Ǿ����ϴ�.");
				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"> ���� �ý��� ����ȭ�� �����մϴ�.");

				m_OptimizingTarget.Clear();
				MkFileManager::Instance().GetFileSystem().GetBlankChunks(40, m_OptimizingTarget);
				m_MaxProgress = m_OptimizingTarget.GetSize();
				m_CurrentProgress = 0;
				m_MainState = eOptimizeChunk;
			}
			else
			{
				_DownloadFileInfo& fileInfo = m_DownloadFileInfoList[m_CurrentProgress];

				MkPathName tempFilePath = m_TempDataPath + fileInfo.filePath; // ex> D:\\Client\\__TempRep\\data\\AAA\\BBB\\abc.mmd
				if (fileInfo.updateToRealFile)
				{
					MkPathName realFilePath;
					realFilePath.ConvertToRootBasisAbsolutePath(fileInfo.filePath); // ex> D:\\Client\\AAA\\BBB\\abc.mmd
					if (tempFilePath.CopyCurrentFile(realFilePath, false)) // overwrite
					{
						// ���� �����ð� ����� �������� ��� ����ġ�� ���ϱ� ���� ������ ����� ��ġ �ܿ� ������ ����
						if (realFilePath.GetWrittenTime() != fileInfo.writtenTime)
						{
							// ex>
							//	Node "[FileWrittenTime]"
							//	{
							//		Node "aaa\\bbb\\abc.mmd"
							//		{
							//			uint #WT = 1234567;
							//		}
							//	}
							MkDataNode* fwtNode = m_PatchLegacyNode.ChildExist(MKDEF_PL_FWT_NODE_NAME) ?
								m_PatchLegacyNode.GetChildNode(MKDEF_PL_FWT_NODE_NAME) : m_PatchLegacyNode.CreateChildNode(MKDEF_PL_FWT_NODE_NAME);

							if (fwtNode != NULL)
							{
								MkStr lowerKey = fileInfo.filePath;
								lowerKey.ToLower();
								MkHashStr fileKey = lowerKey;

								MkDataNode* fileNode = fwtNode->ChildExist(fileKey) ? fwtNode->GetChildNode(fileKey) : fwtNode->CreateChildNode(fileKey);
								if (fileNode != NULL)
								{
									if (fileNode->IsValidKey(MkPathName::KeyWrittenTime))
									{
										fileNode->SetData(MkPathName::KeyWrittenTime, fileInfo.writtenTime, 0);
									}
									else
									{
										fileNode->CreateUnit(MkPathName::KeyWrittenTime, fileInfo.writtenTime);
									}
								}
							}
						}
					}
					else
					{
						m_ErrorMsg = fileInfo.filePath + L"\n���� ����.";
					}
				}
				else
				{
					if (!MkFileManager::Instance().GetFileSystem().UpdateFromOriginalFile(m_TempDataPath, fileInfo.filePath, fileInfo.writtenTime))
					{
						m_ErrorMsg = fileInfo.filePath + L"\n���� ����.";
					}
				}

				if (m_ErrorMsg.Empty())
				{
					tempFilePath.DeleteCurrentFile();

					MK_DEV_PANEL.MsgToLog(MkStr((fileInfo.updateToRealFile) ? L"  R> " : L"  F> ") + fileInfo.filePath);
					++m_CurrentProgress;
				}
				else
				{
					m_MainState = eShowFailedResult;
					m_MaxProgress = 0;
					m_CurrentProgress = 0;
				}
			}
		}
		break;

	case eOptimizeChunk:
		{
			if (m_CurrentProgress == m_MaxProgress)
			{
				MK_DEV_PANEL.MsgToLog(L"> ���� �ý��� ����ȭ�� �Ϸ�Ǿ����ϴ�.");
				MK_DEV_PANEL.InsertEmptyLine();

				m_MainState = eShowSuccessResult;
				m_MaxProgress = 0;
				m_CurrentProgress = 0;
			}
			else
			{
				unsigned int targetChunk = m_OptimizingTarget[m_CurrentProgress];
				if (MkFileManager::Instance().GetFileSystem().OptimizeChunk(targetChunk))
				{
					MK_DEV_PANEL.MsgToLog(MkStr(targetChunk) + L" �� ûũ ����ȭ�� �Ϸ��߽��ϴ�.");
					++m_CurrentProgress;
				}
				else
				{
					m_ErrorMsg = MkStr(targetChunk) + L" �� ûũ ����ȭ�� �����߽��ϴ�.";

					m_MainState = eShowFailedResult;
					m_MaxProgress = 0;
					m_CurrentProgress = 0;
				}
			}
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

float MkPatchFileDownloader::GetDownloadProgress(void)
{
	return (m_CurrentDownInfo == NULL) ? 0.f : m_CurrentDownInfo->GetProgress();
}

void MkPatchFileDownloader::GetCurrentDownloadState(MkStr& buffer) const
{
	MkStr currSize;
	_ConvertDataSizeToString(m_CurrentDownloadSize, currSize);

	MkStr totalSize;
	_ConvertDataSizeToString(m_TotalDownloadSize, totalSize);

	buffer.Clear();
	buffer.Reserve(128);
	buffer += currSize;
	buffer += L" / ";
	buffer += totalSize;
	buffer += L" (";
	buffer += static_cast<unsigned int>(ConvertToPercentage<unsigned __int64, unsigned __int64>(m_CurrentDownloadSize, m_TotalDownloadSize));
	buffer += L"%)... ";
	buffer += m_RemainCompleteTimeStr;
	buffer += L" left";
}

MkPatchFileDownloader::MkPatchFileDownloader()
{
	m_MainState = eReady;
	m_UseFileSystem = true;
	m_TotalDownloadSize = 0;
	m_SuccessDownloadSize = 0;
	m_CurrentDownloadSize = 0;
	m_MaxProgress = 0;
	m_CurrentProgress = 0;
	m_CurrentDownInfo = NULL;
	m_LastDownloadSizeOnTick = 0;
}

//------------------------------------------------------------------------------------------------//

void MkPatchFileDownloader::_FindFilesToDownload(const MkDataNode& node, const MkPathName& pathOffset, const MkPathName& moduleFileName)
{
	// ���� �˻�
	MkArray<MkHashStr> subObjs;
	MkPathName::__GetSubFiles(node, subObjs);
	MK_INDEXING_LOOP(subObjs, i)
	{
		// module file�� ���� �̹� MkPatchStarter���� ��ġ�Ǿ����Ŷ� �����ϰ� skip
		// �ٽ� ��ġ�޾Ƶ� ������ ���� ������ process�� �ö� ������ ���ε�Ǿ� ���� ��� error �߻� �� �� �����Ƿ�
		// (ex> ���� �޸𸮿��� ����� ��� ��) ����ϰ� skip �ϵ��� ��
		if (pathOffset.Empty())
		{
			MkPathName currFileName = subObjs[i];
			currFileName.ToLower();
			if (currFileName == moduleFileName)
				continue;
		}

		// patch file info
		unsigned int patchOrigSize = 0, patchCompSize = 0, patchWrittenTime = 0;
		const MkDataNode& fileNode = *node.GetChildNode(subObjs[i]);
		fileNode.GetData(MkPathName::KeyFileSize, patchOrigSize, 0);
		fileNode.GetData(MkPathName::KeyFileSize, patchCompSize, 1);
		fileNode.GetData(MkPathName::KeyWrittenTime, patchWrittenTime, 0);

		// ���� ������ �����ϸ� ���� ���ϰ� �ٸ� ��, ������ ���� �ý��۰� ��. ���� �� ������ �ٿ� ���
		MkPathName filePath = pathOffset + subObjs[i];
		MkPathName realPath;
		realPath.ConvertToRootBasisAbsolutePath(filePath);

		bool posOnRealPath = true;
		if (m_UseFileSystem)
		{
			const MkFileSystem& fs = MkFileManager::Instance().GetFileSystem();
			posOnRealPath = (!(fs.CheckFileFilter(pathOffset) && fs.CheckFileFilter(filePath)));
		}

		bool realFileExist = realPath.CheckAvailable();
		bool downTheFile = false;
		if (realFileExist)
		{
			downTheFile = (realPath.GetFileSize() != patchOrigSize); // ũ�� ��
			if (!downTheFile)
			{
				unsigned int writtenTimeOfRealFile = realPath.GetWrittenTime();
				downTheFile = (writtenTimeOfRealFile != patchWrittenTime); // �����ð� ��
				if (downTheFile)
				{
					MkDataNode* fwtNode = m_PatchLegacyNode.GetChildNode(MKDEF_PL_FWT_NODE_NAME);
					if (fwtNode != NULL)
					{
						MkStr lowerKey = filePath;
						lowerKey.ToLower();
						MkDataNode* fileNode = fwtNode->GetChildNode(MkHashStr(lowerKey));
						if (fileNode != NULL)
						{
							unsigned int writtenTimeInLegacy = 0;
							if (fileNode->GetData(MkPathName::KeyWrittenTime, writtenTimeInLegacy, 0))
							{
								downTheFile = (writtenTimeOfRealFile != writtenTimeInLegacy);
							}
						}
					}
				}
			}
		}
		else
		{
			downTheFile = (posOnRealPath || (MkFileManager::Instance().GetFileSystem().GetFileDifference(filePath, patchOrigSize, patchWrittenTime) != 0));
		}

		// �ٿ� ����̸� �̹� �޾� ���� �ӽ� ������ �ִ��� �˻��� ���ų� �ٸ��� �ٿ�
		if (downTheFile)
		{
			_DownloadFileInfo& info = m_DownloadFileInfoList.PushBack();

			info.filePath = filePath;
			info.origSize = patchOrigSize;
			info.compSize = ((patchOrigSize > 0) && (patchCompSize > 0)) ? patchCompSize : 0;
			info.writtenTime = patchWrittenTime;

			MkPathName tmpFilePath = m_TempDataPath + filePath;
			info.alreadyDowned = (tmpFilePath.CheckAvailable() && (tmpFilePath.GetFileSize() == patchOrigSize) && (tmpFilePath.GetWrittenTime() == patchWrittenTime));

			bool updateToRealFile = true;
			if (m_UseFileSystem)
			{
				// ���� �ý����� ������ ���� ���� ���
				updateToRealFile = (MkFileManager::Instance().GetFileSystem().GetTotalChunkCount() == 0);
				if (!updateToRealFile)
				{
					// ���� ������ �����ϸ� ���
					updateToRealFile = realFileExist;

					// ���� �ý��� �Ҽ� ���� �Ǵ�. ���� �ý��ۿ��� �Ҽӵ��� �ʴ´ٸ� ���� ���� ���
					if (!updateToRealFile)
					{
						updateToRealFile = posOnRealPath;
					}
				}
			}
			info.updateToRealFile = updateToRealFile;
		}
	}
	
	// ���� ���丮 �˻�
	subObjs.Clear();
	MkPathName::__GetSubDirectories(node, subObjs);
	MK_INDEXING_LOOP(subObjs, i)
	{
		MkPathName newOffset = pathOffset + subObjs[i];
		newOffset.CheckAndAddBackslash();
		_FindFilesToDownload(*node.GetChildNode(subObjs[i]), newOffset, moduleFileName); // ���
	}
}

void MkPatchFileDownloader::_UpdateDownloadState(void)
{
	m_CurrentDownloadSize = m_SuccessDownloadSize;
	if ((m_CurrentDownInfo != NULL) && (m_CurrentDownInfo->GetDownState() == MkFileDownInfo::eDS_Downloading))
	{
		m_CurrentDownloadSize += static_cast<unsigned __int64>(m_CurrentDownInfo->GetDownSize());
	}

	MkTimeState timeState;
	MK_TIME_MGR.GetCurrentTimeState(timeState);
	if (m_DownloadTickCounter.OnTick(timeState))
	{
		double downSizePerSec = static_cast<double>(m_CurrentDownloadSize - m_LastDownloadSizeOnTick) / m_DownloadTickCounter.GetTickCount();
		double remainSize = static_cast<double>(m_TotalDownloadSize - m_CurrentDownloadSize);
		unsigned int seconds = static_cast<unsigned int>(remainSize / downSizePerSec);
		unsigned int days = seconds / 86400;
		seconds = seconds % 86400;
		unsigned int hour = seconds / 3600;
		seconds = seconds % 3600;
		unsigned int minute = seconds / 60;
		seconds = seconds % 60;

		m_RemainCompleteTimeStr.Clear();
		m_RemainCompleteTimeStr.Reserve(32);
		if (days > 0)
		{
			m_RemainCompleteTimeStr += days;
			m_RemainCompleteTimeStr += L"d ";

			m_RemainCompleteTimeStr += hour;
			m_RemainCompleteTimeStr += L"h";
		}
		else if (hour > 0)
		{
			m_RemainCompleteTimeStr += hour;
			m_RemainCompleteTimeStr += L"h ";

			m_RemainCompleteTimeStr += minute;
			m_RemainCompleteTimeStr += L"m";
		}
		else if (minute > 0)
		{
			m_RemainCompleteTimeStr += minute;
			m_RemainCompleteTimeStr += L"m ";

			m_RemainCompleteTimeStr += seconds;
			m_RemainCompleteTimeStr += L"s";
		}
		else if (seconds > 0)
		{
			m_RemainCompleteTimeStr += seconds;
			m_RemainCompleteTimeStr += L"s";
		}
		else
		{
			m_RemainCompleteTimeStr += L"--";
		}

		m_LastDownloadSizeOnTick = m_CurrentDownloadSize;
		m_DownloadTickCounter.SetUp(timeState, 3.);
	}
}

void MkPatchFileDownloader::_ConvertDataSizeToString(unsigned __int64 size, MkStr& buffer)
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
