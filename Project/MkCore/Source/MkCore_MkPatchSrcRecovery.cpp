
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

		// 기존 history, patch, source directory는 모두 삭제. 완전 초기화
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
					MK_DEV_PANEL.MsgToLog(L"  런쳐 파일 다운 성공.");
					MK_DEV_PANEL.InsertEmptyLine();
				}
				else
				{
					m_ErrorMsg = urlLauncherFilePath + L"\n네트워크 오류로 다운이 실패했습니다.";
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
					MK_DEV_PANEL.MsgToLog(L"  설정 파일 다운 성공.");
					MK_DEV_PANEL.InsertEmptyLine();
				}
				else
				{
					m_ErrorMsg = urlInfoFilePath + L"\n네트워크 오류로 다운이 실패했습니다.";
					break;
				}

				// load info file
				m_PatchInfoNode.Clear();
				if (!m_PatchInfoNode.Load(localInfoFilePath))
				{
					m_ErrorMsg = urlInfoFilePath + L"\n위치의 설정 파일 읽기 실패.";
					break;
				}

				m_CurrentWrittenTime = 0;
				if ((m_PatchInfoNode.GetDataTypeTag() != MkPatchFileGenerator::DataType_PatchInfo) ||
					(!m_PatchInfoNode.ChildExist(MkPatchFileGenerator::KEY_StructureNode)) ||
					(!m_PatchInfoNode.GetData(MkPatchFileGenerator::KEY_CurrWT, m_CurrentWrittenTime, 0)))
				{
					m_ErrorMsg = urlInfoFilePath + L"\n위치의 파일은 정상적인 설정 파일이 아닙니다.";
					break;
				}

				m_PatchInfoNode.SetData(MkPatchFileGenerator::KEY_LastWT, static_cast<unsigned int>(0), 0);

				MK_DEV_PANEL.MsgToLog(L"> " + localInfoFilePath);
				MK_DEV_PANEL.MsgToLog(L"  설정 파일 로딩 성공 : " + MkPatchFileGenerator::ConvertWrittenTimeToStr(m_CurrentWrittenTime));
				MK_DEV_PANEL.InsertEmptyLine();

				// file system node 추출해 저장
				MkDataNode* fsSettingNode = m_PatchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_FileSystemNode);
				if (fsSettingNode != NULL)
				{
					fsSettingNode->SaveToText(m_RecoveryDirPath + MkFileSystem::DefaultSettingFileName);

					// 더 이상 필요 없음
					fsSettingNode->DetachFromParentNode();
					delete fsSettingNode;

					MK_DEV_PANEL.MsgToLog(L"> 파일 시스템 설정 추출");
					MK_DEV_PANEL.InsertEmptyLine();
				}

				// info node를 history node로 변환
				m_PatchInfoNode.SetDataTypeTag(MkPatchFileGenerator::DataType_PatchHistory);

				// launcher 설정 삽입
				MkDataNode* launcherNode = m_PatchInfoNode.CreateChildNode(MkPatchFileGenerator::KEY_LauncherNode);
				if ((launcherNode == NULL) || (!launcherNode->Load(localLauncherFilePath)))
				{
					m_ErrorMsg = localLauncherFilePath + L"\n위치의 런쳐 파일 읽기 실패";
					break;
				}

				// update node 생성
				MkDataNode* updateNode = m_PatchInfoNode.CreateChildNode(MkPatchFileGenerator::KEY_UpdateNode);
				if (updateNode == NULL)
				{
					m_ErrorMsg = MkPatchFileGenerator::KEY_UpdateNode.GetString() + L" 노드 생성 실패";
					break;
				}

				// purge list가 있으면 update node 하위로 돌림
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
			// structure 기준으로 다운받을 파일 검색(structure node 존재여부는 위에서 검사)
			const MkDataNode& structureNode = *m_PatchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_StructureNode);
			unsigned int totalFiles = MkPathName::__CountTotalFiles(structureNode);
			MK_DEV_PANEL.MsgToLog(L"> 누적 패치 파일 : " + MkStr(totalFiles) + L" 개");

			m_DownloadFileInfoList.Clear();
			m_DownloadFileInfoList.Reserve(totalFiles);

			_FindFilesToDownload(structureNode, MkStr::EMPTY);

			if (totalFiles > 0)
			{
				MkDataNode* updateNode = m_PatchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_UpdateNode);
				m_UpdateAddNode = updateNode->CreateChildNode(MkPatchFileGenerator::KEY_AddNode);
			}

			// 총량 계산
			unsigned __int64 totalDownloadSize = 0;
			MK_INDEXING_LOOP(m_DownloadFileInfoList, i)
			{
				const _DownloadFileInfo& fi = m_DownloadFileInfoList[i];
				totalDownloadSize += static_cast<unsigned __int64>((fi.compSize > 0) ? fi.compSize : fi.origSize);
			}
			
			MK_DEV_PANEL.MsgToLog(L"> 다운 대상 파일 : " + MkStr(m_DownloadFileInfoList.GetSize()) + L" 개");

			MkStr sizeBuf;
			_ConvertDataSizeToString(totalDownloadSize, sizeBuf);
			MK_DEV_PANEL.MsgToLog(L"> 다운 대상 크기 : " + sizeBuf);
			MK_DEV_PANEL.InsertEmptyLine();
			MK_DEV_PANEL.MsgToLog(L"> 파일 다운을 시작합니다.");

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

				MK_DEV_PANEL.MsgToLog(L"> 파일 다운및 언패킹 완료되었습니다.");
				MK_DEV_PANEL.InsertEmptyLine();

				MkPathName historyFilePath = m_HistoryDirPath + MkStr(m_CurrentWrittenTime) + L"." + MkDataNode::DefaultFileExtension;
				if (m_PatchInfoNode.SaveToBinary(historyFilePath))
				{
					MK_DEV_PANEL.MsgToLog(historyFilePath + L"\n저장 완료되었습니다.");
					MK_DEV_PANEL.InsertEmptyLine();
					MK_DEV_PANEL.MsgToLog(L"---------------------------------------------");
					MK_DEV_PANEL.MsgToLog(L"> 복구가 완료되었습니다.");
					MK_DEV_PANEL.MsgToLog(L"---------------------------------------------");

					m_MainState = eShowSuccessResult;
				}
				else
				{
					MK_DEV_PANEL.MsgToLog(historyFilePath + L"\n저장이 실패하였습니다.");

					m_MainState = eShowFailedResult;
				}
			}
			else
			{
				// 이미 다운받은 경우면 스킵
				const _DownloadFileInfo& fileInfo = m_DownloadFileInfoList[m_CurrentProgress];
				
				// url
				MkPathName relPathInServer, relPathInClient;
				MkPatchFileGenerator::ConvertFilePathToDownloadable(fileInfo.filePath, relPathInServer); // ex> AAA\\BBB\\abc.mmd -> AAA\\BBB\\abc.mmd.rp
				relPathInClient = relPathInServer;
				relPathInServer.ReplaceKeyword(L"\\", L"/"); // ex> AAA/BBB/abc.mmd.rp
				MkStr urlPath = m_DataRootURL + relPathInServer; // ex> http://210.207.252.151/Test/data/AAA/BBB/abc.mmd.rp
				MK_DEV_PANEL.MsgToLog(L" ∇ " + relPathInServer);

				// local. 압축여부 따지지 않고 그대로 다운
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
				::Sleep(1); // 대기
				break;
			}
			else if (downState == MkFileDownInfo::eDS_Complete) // 완료
			{
				const _DownloadFileInfo& fileInfo = m_DownloadFileInfoList[m_CurrentProgress];

				MkPathName relPathInClient;
				MkPatchFileGenerator::ConvertFilePathToDownloadable(fileInfo.filePath, relPathInClient); // ex> AAA\\BBB\\abc.mmd -> AAA\\BBB\\abc.mmd.rp
				MkPathName localDownedFilePath = m_PatchDataPath + relPathInClient; // 패치 파일. ex> D:\\Updater\\Patch\\data\\AAA\\BBB\\abc.mmd.rp

				MkPathName localSrcFilePath = m_SourceDirPath + fileInfo.filePath; // 소스 파일. ex> D:\\Src\\AAA\\BBB\\abc.mmd
				bool compressed = (fileInfo.compSize > 0);

				do
				{
					if (compressed)
					{
						// 압축 파일 읽어들여
						MkByteArray compData;
						MkInterfaceForFileReading frInterface;
						if (!frInterface.SetUp(localDownedFilePath))
						{
							m_ErrorMsg = L"다운된 파일 열기 실패.";
							break;
						}
						if (frInterface.Read(compData, MkArraySection(0)) != fileInfo.compSize)
						{
							m_ErrorMsg = L"다운된 파일 읽기 오류거나 다운된 압축 파일 크기가 다릅니다. 맞는 크기 : " + MkStr(fileInfo.compSize);
							break;
						}
						frInterface.Clear();

						// 압축 해제
						MkByteArray origData;
						if (MkZipCompressor::Uncompress(compData.GetPtr(), compData.GetSize(), origData) != fileInfo.origSize)
						{
							m_ErrorMsg = L"다운된 파일 압축 해제 후 크기가 다릅니다. 맞는 크기 : " + MkStr(fileInfo.origSize);
							break;
						}
						compData.Clear();

						// 소스 파일로 기록
						MkInterfaceForFileWriting fwInterface;
						if (!fwInterface.SetUp(localSrcFilePath, true, true))
						{
							m_ErrorMsg = L"소스 파일 생성 실패.";
							break;
						}
						if (fwInterface.Write(origData, MkArraySection(0)) != fileInfo.origSize)
						{
							m_ErrorMsg = L"소스 파일 쓰기 오류.";
							break;
						}
						fwInterface.Clear();
						origData.Clear();
					}
					else
					{
						// 크기만 검사 후 소스 파일로 복사
						if (localDownedFilePath.GetFileSize() != fileInfo.origSize)
						{
							m_ErrorMsg = L"다운된 원본 파일 크기가 다릅니다. 맞는 크기 : " + MkStr(fileInfo.origSize);
							break;
						}

						if (!localDownedFilePath.CopyCurrentFile(localSrcFilePath, false)) // overwrite
						{
							m_ErrorMsg = L"소스 파일로 복사 오류.";
							break;
						}
					}

					// 파일 수정시간 변경
					if (!localSrcFilePath.SetWrittenTime(fileInfo.writtenTime))
					{
						m_ErrorMsg = L"소스 파일 수정시간 변경 실패";
						break;
					}

					// history의 update-add node에 추가
					MkDataNode* fileAddNode = m_UpdateAddNode->CreateChildNode(fileInfo.filePath);
					if ((fileAddNode == NULL) ||
						(!fileAddNode->CreateUnit(MkPathName::KeyFileSize, fileInfo.origSize)) ||
						(!fileAddNode->CreateUnit(MkPathName::KeyWrittenTime, fileInfo.writtenTime)))
					{
						m_ErrorMsg = L"소스 파일의 add node 정보 생성 실패";
						break;
					}
					
				}
				while (false);
			}
			else // 다운로드 중단/실패
			{
				MkStr fileURL;
				MkPathName destLocalPath;
				m_CurrentDownInfo->__GetTargetInfo(fileURL, destLocalPath);

				switch (downState)
				{
				case MkFileDownInfo::eDS_Abort: m_ErrorMsg = fileURL + L"\n사용자에 의한 중도포기"; break;
				case MkFileDownInfo::eDS_InvalidURL: m_ErrorMsg = fileURL + L"\n해당 파일이 네트워크상 존재하지 않음"; break;
				case MkFileDownInfo::eDS_OutOfMemory: m_ErrorMsg = fileURL + L"\n메모리 부족"; break;
				case MkFileDownInfo::eDS_Failed: m_ErrorMsg = fileURL + L"\n다운로드 실패"; break;
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

			// info 삭제
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
	// 파일 검사
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
	
	// 하위 디렉토리 검사
	subObjs.Clear();
	MkPathName::__GetSubDirectories(node, subObjs);
	MK_INDEXING_LOOP(subObjs, i)
	{
		MkPathName newOffset = pathOffset + subObjs[i];
		newOffset.CheckAndAddBackslash();
		_FindFilesToDownload(*node.GetChildNode(subObjs[i]), newOffset); // 재귀
	}
}

void MkPatchSrcRecovery::_ConvertDataSizeToString(unsigned __int64 size, MkStr& buffer)
{
	unsigned __int64 totalKB = size / 1024;
	unsigned __int64 gb = 0, mb = 0, kb = 0;

	// giga 단위? -> GB/MB
	if (totalKB > (1024 * 1024))
	{
		gb = totalKB / (1024 * 1024);
		mb = (totalKB - gb * (1024 * 1024)) / 1024;
	}
	// mega 단위? -> MB/KB
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
