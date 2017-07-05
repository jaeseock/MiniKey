
#include <Windows.h>
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkZipCompressor.h"
#include "MkCore_MkFileDownloader.h"
#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkPatchFileDownloader.h"


const MkStr MkPatchFileDownloader::TemporaryRepositoryDirName(L"__TempRep\\");


//------------------------------------------------------------------------------------------------//

bool MkPatchFileDownloader::CheckAndDownloadPatchFile(const MkStr& url)
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

		m_ErrorMsg.Clear();
		m_MainState = eDownloadPatchInfoFile;
		m_MaxProgress = 0;
		m_CurrentProgress = 0;
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

			switch (MkFileDownloader::Start(urlInfoFilePath, localInfoFilePath))
			{
			case MkFileDownloader::eInvalidURL: m_ErrorMsg = urlInfoFilePath + L"\n��ο� ������ �������� �ʽ��ϴ�."; break;
			case MkFileDownloader::eNetworkError: m_ErrorMsg = urlInfoFilePath + L"\n��Ʈ��ũ ������ �ٿ��� �����߽��ϴ�."; break;
			case MkFileDownloader::eOutOfMemory: m_ErrorMsg = urlInfoFilePath + L"\n�޸� �������� �ٿ��� �����߽��ϴ�."; break;
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

				m_MainState = eShowResult;
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
			unsigned int totalFiles = _CountTotalFiles(structureNode);
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

			MK_DEV_PANEL.MsgToLog(L"> �ٿ� ��� ���� : " + MkStr(m_DownloadFileInfoList.GetSize()) + L" ��");
			MK_DEV_PANEL.InsertEmptyLine();
			MK_DEV_PANEL.MsgToLog(L"> ���� �ٿ��� �����մϴ�.");

			m_MainState = eDownloadTargetFiles;
			m_MaxProgress = m_DownloadFileInfoList.GetSize();
			m_CurrentProgress = 0;
		}
		break;

	case eDownloadTargetFiles:
		{
			if (m_CurrentProgress == m_MaxProgress)
			{
				MK_DEV_PANEL.MsgToLog(L"> ���� �ٿ��� �Ϸ�Ǿ����ϴ�.");
				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"> ��ġ ������ �����մϴ�.");

				m_CurrentProgress = 0;
				m_MainState = eUpdateFiles;
			}
			else
			{
				// �̹� �ٿ���� ���� ��ŵ
				const _DownloadFileInfo& fileInfo = m_DownloadFileInfoList[m_CurrentProgress];
				if (!fileInfo.alreadyDowned)
				{
					// url
					MkPathName relPathInServer, relPathInClient;
					MkPatchFileGenerator::ConvertFilePathToDownloadable(fileInfo.filePath, relPathInServer); // ex> AAA\\BBB\\abc.mmd -> AAA\\BBB\\abc.mmd.rp
					relPathInClient = relPathInServer;
					relPathInServer.ReplaceKeyword(L"\\", L"/"); // ex> AAA/BBB/abc.mmd.rp
					MkStr urlPath = m_DataRootURL + relPathInServer; // ex> http://210.207.252.151/Test/data/AAA/BBB/abc.mmd.rp

					// local. ���� ���ο� ���� �״�� �ٿ�, Ȥ�� �ٿ� �� ��������
					MkPathName localFilePathForComp = m_TempDataPath + relPathInClient; // ����. ex> D:\\Client\\__TempRep\\data\\AAA\\BBB\\abc.mmd.rp
					MkPathName localFilePathForOrig = m_TempDataPath + fileInfo.filePath; // �����. ex> D:\\Client\\__TempRep\\data\\AAA\\BBB\\abc.mmd
					bool compressed = (fileInfo.compSize > 0);

					// down
					switch (MkFileDownloader::Start(urlPath, (compressed) ? localFilePathForComp : localFilePathForOrig))
					{
					case MkFileDownloader::eInvalidURL: m_ErrorMsg = urlPath + L"\n��ο� ������ �������� �ʽ��ϴ�."; break;
					case MkFileDownloader::eNetworkError: m_ErrorMsg = urlPath + L"\n��Ʈ��ũ ������ �ٿ��� �����߽��ϴ�."; break;
					case MkFileDownloader::eOutOfMemory: m_ErrorMsg = urlPath + L"\n�޸� �������� �ٿ��� �����߽��ϴ�."; break;
					}

					if (m_ErrorMsg.Empty())
					{
						do
						{
							// uncompress!!! -> WT ����!!!!
							if (compressed)
							{
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
							}
							else
							{
								// ũ�⸸ �˻�
								if (localFilePathForOrig.GetFileSize() != fileInfo.origSize)
								{
									m_ErrorMsg = localFilePathForOrig + L"\n�ٿ�� ���� ���� ũ�Ⱑ �ٸ��ϴ�. �´� ũ�� : " + MkStr(fileInfo.origSize);
									break;
								}
							}

							// ���� �����ð� ����
							if (!localFilePathForOrig.SetWrittenTime(fileInfo.writtenTime))
							{
								m_ErrorMsg = localFilePathForOrig + L"\n���� �����ð� ���� ����";
							}
						}
						while (false);
					}
				}

				if (m_ErrorMsg.Empty())
				{
					MK_DEV_PANEL.MsgToLog(L"  + " + fileInfo.filePath);
					++m_CurrentProgress;
				}
				else
				{
					m_MainState = eShowResult;
					m_MaxProgress = 0;
					m_CurrentProgress = 0;
				}
			}
		}
		break;

	case eUpdateFiles:
		{
			if (m_CurrentProgress == m_MaxProgress)
			{
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
					if (!tempFilePath.CopyCurrentFile(realFilePath, false)) // overwrite
					{
						m_ErrorMsg = fileInfo.filePath + L"\n���� ����.";
					}
				}
				else
				{
					if (!MkFileManager::Instance().GetFileSystem().UpdateFromOriginalFile(m_TempDataPath, fileInfo.filePath))
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
					m_MainState = eShowResult;
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

				m_MainState = eShowResult;
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

					m_MainState = eShowResult;
					m_MaxProgress = 0;
					m_CurrentProgress = 0;
				}
			}
		}
		break;

	case eShowResult:
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

MkPatchFileDownloader::MkPatchFileDownloader()
{
	m_MainState = eReady;
	m_MaxProgress = 0;
	m_CurrentProgress = 0;
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

		bool realFileExist = realPath.CheckAvailable();
		bool downTheFile = (realFileExist) ?
			((realPath.GetFileSize() != patchOrigSize) || (realPath.GetWrittenTime() != patchWrittenTime)) :
			(MkFileManager::Instance().GetFileSystem().GetFileDifference(filePath, patchOrigSize, patchWrittenTime) != 0);

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

			// ���� �ý����� ������ ���� ���� ���
			const MkFileSystem& fs = MkFileManager::Instance().GetFileSystem();
			bool updateToRealFile = (fs.GetTotalChunkCount() == 0);
			if (!updateToRealFile)
			{
				// ���� ������ �����ϸ� ���
				updateToRealFile = realFileExist;

				// ���� �ý��� �Ҽ� ���� �Ǵ�. ���� �ý��ۿ��� �Ҽӵ��� �ʴ´ٸ� ���� ���� ���
				if (!updateToRealFile)
				{
					updateToRealFile = (!(fs.CheckFileFilter(pathOffset) && fs.CheckFileFilter(filePath)));
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

unsigned int MkPatchFileDownloader::_CountTotalFiles(const MkDataNode& node)
{
	unsigned int files = 0;
	node.GetData(MkPathName::KeyFileCount, files, 0);

	MkArray<MkHashStr> subDirs;
	MkPathName::__GetSubDirectories(node, subDirs);
	MK_INDEXING_LOOP(subDirs, i)
	{
		files += _CountTotalFiles(*node.GetChildNode(subDirs[i]));
	}
	return files;
}

//------------------------------------------------------------------------------------------------//
