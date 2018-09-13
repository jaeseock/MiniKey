
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
				m_ErrorMsg = urlInfoFilePath + L"\n네트워크 오류로 다운이 실패했습니다.";
			}

			if (m_ErrorMsg.Empty())
			{
				MK_DEV_PANEL.MsgToLog(L"> " + urlInfoFilePath);
				MK_DEV_PANEL.MsgToLog(L"  설정 파일 다운 성공.");
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
						MK_DEV_PANEL.MsgToLog(L"  설정 파일 로딩 성공 : " + MkPatchFileGenerator::ConvertWrittenTimeToStr(lastWT));
						MK_DEV_PANEL.InsertEmptyLine();

						MkStr rfp;
						if (m_PatchInfoNode.GetData(MkPatchFileGenerator::KEY_RunFilePath, rfp, 0))
						{
							m_RunFilePath.ConvertToRootBasisAbsolutePath(rfp);

							MK_DEV_PANEL.MsgToLog(L"> " + m_RunFilePath);
							MK_DEV_PANEL.MsgToLog(L"  실행 파일 경로 설정");
							MK_DEV_PANEL.InsertEmptyLine();
						}
					}
					else
						m_ErrorMsg = urlInfoFilePath + L"\n위치의 파일은 정상적인 설정 파일이 아닙니다.";
				}
				else
					m_ErrorMsg = urlInfoFilePath + L"\n위치의 설정 파일 읽기 실패.";
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
					MK_DEV_PANEL.MsgToLog(L"> 파일 삭제 최대 " + MkStr(keys.GetSize()) + L" 개 파일");

					MK_INDEXING_LOOP(keys, i)
					{
						MkPathName targetFilePath = keys[i].GetString();

						// 실제 파일 있으면 삭제
						MkPathName absoluteFilePath;
						absoluteFilePath.ConvertToRootBasisAbsolutePath(targetFilePath);
						absoluteFilePath.DeleteCurrentFile();

						// file system상에 있으면 삭제
						MkFileManager::Instance().GetFileSystem().RemoveFile(targetFilePath);

						// 패치 잔여 정보에 있으면 삭제
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

				// purge node는 더 이상 필요 없음
				purgeNode->DetachFromParentNode();
				delete purgeNode;
			}

			m_MainState = eFindDownloadTargets;
		}
		break;

	case eFindDownloadTargets:
		{
			// client root에 file system 설정이 존재한다면 반영
			MkDataNode* fsSettingNode = m_PatchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_FileSystemNode);
			if (fsSettingNode != NULL)
			{
				MkFileManager::Instance().GetFileSystem().SetSystemSetting(*fsSettingNode);

				// 더 이상 필요 없음
				fsSettingNode->DetachFromParentNode();
				delete fsSettingNode;

				MK_DEV_PANEL.MsgToLog(L"> 파일 시스템 설정 적용");
				MK_DEV_PANEL.InsertEmptyLine();
			}

			// structure 기준으로 다운받을 파일 검색(structure node 존재여부는 위에서 검사)
			const MkDataNode& structureNode = *m_PatchInfoNode.GetChildNode(MkPatchFileGenerator::KEY_StructureNode);
			unsigned int totalFiles = MkPathName::__CountTotalFiles(structureNode);
			MK_DEV_PANEL.MsgToLog(L"> 누적 패치 파일 : " + MkStr(totalFiles) + L" 개");

			m_DownloadFileInfoList.Clear();
			m_DownloadFileInfoList.Reserve(totalFiles);

			// module file name
			wchar_t fullPath[MAX_PATH];
			::GetModuleFileName(NULL, fullPath, MAX_PATH);
			MkPathName tmpBuf = fullPath;
			MkPathName moduleFileName = tmpBuf.GetFileName();
			moduleFileName.ToLower();

			_FindFilesToDownload(structureNode, MkStr::EMPTY, moduleFileName); // 검색

			// 총량 계산
			MK_INDEXING_LOOP(m_DownloadFileInfoList, i)
			{
				const _DownloadFileInfo& fi = m_DownloadFileInfoList[i];
				m_TotalDownloadSize += static_cast<unsigned __int64>((fi.compSize > 0) ? fi.compSize : fi.origSize);
			}
			m_SuccessDownloadSize = 0;
			m_CurrentDownloadSize = 0;
			
			MK_DEV_PANEL.MsgToLog(L"> 다운 대상 파일 : " + MkStr(m_DownloadFileInfoList.GetSize()) + L" 개");

			MkStr sizeBuf;
			_ConvertDataSizeToString(m_TotalDownloadSize, sizeBuf);
			MK_DEV_PANEL.MsgToLog(L"> 다운 대상 크기 : " + sizeBuf);
			MK_DEV_PANEL.InsertEmptyLine();
			MK_DEV_PANEL.MsgToLog(L"> 파일 다운을 시작합니다.");

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
				MK_DEV_PANEL.MsgToLog(L"> 파일 다운이 완료되었습니다.");
				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"> 패치 파일을 적용합니다.");

				m_CurrentDownloadSize = m_SuccessDownloadSize = m_TotalDownloadSize;
				m_RemainCompleteTimeStr.Clear();
				m_CurrentProgress = 0;
				m_MainState = eUpdateFiles;
			}
			else
			{
				// 이미 다운받은 경우면 스킵
				const _DownloadFileInfo& fileInfo = m_DownloadFileInfoList[m_CurrentProgress];
				if (fileInfo.alreadyDowned)
				{
					MK_DEV_PANEL.MsgToLog(L"    " + fileInfo.filePath);
					++m_CurrentProgress;

					// 다운 성공 크기 갱신
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
					MK_DEV_PANEL.MsgToLog(L" ∇ " + relPathInServer);

					// local. 압축 여부에 따라 그대로 다운, 혹은 다운 후 압축해제
					MkPathName localFilePathForComp = m_TempDataPath + relPathInClient; // 압축. ex> D:\\Client\\__TempRep\\data\\AAA\\BBB\\abc.mmd.rp
					MkPathName localFilePathForOrig = m_TempDataPath + fileInfo.filePath; // 비압축. ex> D:\\Client\\__TempRep\\data\\AAA\\BBB\\abc.mmd
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

				::Sleep(1); // 대기
				break;
			}
			else if (downState == MkFileDownInfo::eDS_Complete) // 완료
			{
				_DownloadFileInfo& fileInfo = m_DownloadFileInfoList[m_CurrentProgress];

				MkPathName localFilePathForOrig = m_TempDataPath + fileInfo.filePath; // 비압축. ex> D:\\Client\\__TempRep\\data\\AAA\\BBB\\abc.mmd
				bool compressed = (fileInfo.compSize > 0);

				do
				{
					// uncompress!!! -> WT 변경!!!!
					if (compressed)
					{
						MkPathName relPathInClient;
						MkPatchFileGenerator::ConvertFilePathToDownloadable(fileInfo.filePath, relPathInClient); // ex> AAA\\BBB\\abc.mmd -> AAA\\BBB\\abc.mmd.rp
						MkPathName localFilePathForComp = m_TempDataPath + relPathInClient; // 압축. ex> D:\\Client\\__TempRep\\data\\AAA\\BBB\\abc.mmd.rp

						// 압축 파일 읽어들여
						MkByteArray compData;
						MkInterfaceForFileReading frInterface;
						if (!frInterface.SetUp(localFilePathForComp))
						{
							m_ErrorMsg = localFilePathForComp + L"\n파일 열기 실패.";
							break;
						}
						if (frInterface.Read(compData, MkArraySection(0)) != fileInfo.compSize)
						{
							m_ErrorMsg = localFilePathForComp + L"\n읽기 오류거나 다운된 압축 파일 크기가 다릅니다.";
							break;
						}
						frInterface.Clear();

						// 압축 해제
						MkByteArray origData;
						if (MkZipCompressor::Uncompress(compData.GetPtr(), compData.GetSize(), origData) != fileInfo.origSize)
						{
							m_ErrorMsg = localFilePathForComp + L"\n압축 해제 후 크기가 다릅니다";
							break;
						}
						compData.Clear();

						// 파일로 기록
						MkInterfaceForFileWriting fwInterface;
						if (!fwInterface.SetUp(localFilePathForOrig, true, true))
						{
							m_ErrorMsg = localFilePathForOrig + L"\n파일 생성 실패.";
							break;
						}
						if (fwInterface.Write(origData, MkArraySection(0)) != fileInfo.origSize)
						{
							m_ErrorMsg = localFilePathForOrig + L"\n쓰기 오류.";
							break;
						}
						fwInterface.Clear();
						origData.Clear();

						// 압축 파일 삭제
						localFilePathForComp.DeleteCurrentFile();

						// 다운 성공 크기 갱신
						m_SuccessDownloadSize += fileInfo.compSize;
					}
					else
					{
						// 크기만 검사
						if (localFilePathForOrig.GetFileSize() != fileInfo.origSize)
						{
							m_ErrorMsg = localFilePathForOrig + L"\n다운된 원본 파일 크기가 다릅니다. 맞는 크기 : " + MkStr(fileInfo.origSize);
							break;
						}

						// 다운 성공 크기 갱신
						m_SuccessDownloadSize += fileInfo.origSize;
					}

					// 파일 수정시간 변경 시도. 실패 가능성 존재
					localFilePathForOrig.SetWrittenTime(fileInfo.writtenTime);
				}
				while (false);

				_UpdateDownloadState();
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
				MK_DEV_PANEL.MsgToLog(L"  " + m_ErrorMsg);

				m_MainState = eShowFailedResult;
				m_MaxProgress = 0;
				m_CurrentProgress = 0;
			}

			// info 삭제
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
					MK_DEV_PANEL.MsgToLog(L"  패치 잔여 정보를 기록했습니다.");
					MK_DEV_PANEL.InsertEmptyLine();
				}

				MkPathName localTempDir = MkPathName::GetRootDirectory() + TemporaryRepositoryDirName; // ex> D:\\Client\\__TempRep
				localTempDir.DeleteCurrentDirectory(true);

				MK_DEV_PANEL.MsgToLog(L"> 파일 패치가 완료되었습니다.");
				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"> 파일 시스템 최적화를 실행합니다.");

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
						// 파일 수정시간 기록이 실패했을 경우 재패치를 피하기 위해 별도의 기록을 패치 잔여 정보에 남김
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
						m_ErrorMsg = fileInfo.filePath + L"\n복사 오류.";
					}
				}
				else
				{
					if (!MkFileManager::Instance().GetFileSystem().UpdateFromOriginalFile(m_TempDataPath, fileInfo.filePath, fileInfo.writtenTime))
					{
						m_ErrorMsg = fileInfo.filePath + L"\n갱신 오류.";
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
				MK_DEV_PANEL.MsgToLog(L"> 파일 시스템 최적화가 완료되었습니다.");
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
					MK_DEV_PANEL.MsgToLog(MkStr(targetChunk) + L" 번 청크 최적화를 완료했습니다.");
					++m_CurrentProgress;
				}
				else
				{
					m_ErrorMsg = MkStr(targetChunk) + L" 번 청크 최적화를 실패했습니다.";

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
	// 파일 검사
	MkArray<MkHashStr> subObjs;
	MkPathName::__GetSubFiles(node, subObjs);
	MK_INDEXING_LOOP(subObjs, i)
	{
		// module file의 경우는 이미 MkPatchStarter에서 패치되었을거라 간주하고 skip
		// 다시 패치받아도 보통은 문제 없지만 process에 올라간 파일이 바인드되어 있을 경우 error 발생 할 수 있으므로
		// (ex> 외장 메모리에서 실행될 경우 등) 깔끔하게 skip 하도록 함
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

		// 실제 파일이 존재하면 실제 파일과 다름 비교, 없으면 파일 시스템과 비교. 양쪽 다 없으면 다운 대상
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
			downTheFile = (realPath.GetFileSize() != patchOrigSize); // 크기 비교
			if (!downTheFile)
			{
				unsigned int writtenTimeOfRealFile = realPath.GetWrittenTime();
				downTheFile = (writtenTimeOfRealFile != patchWrittenTime); // 수정시간 비교
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

		// 다운 대상이면 이미 받아 놓은 임시 파일이 있는지 검사해 없거나 다르면 다운
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
				// 파일 시스템이 없으면 실제 파일 대상
				updateToRealFile = (MkFileManager::Instance().GetFileSystem().GetTotalChunkCount() == 0);
				if (!updateToRealFile)
				{
					// 실제 파일이 존재하면 대상
					updateToRealFile = realFileExist;

					// 파일 시스템 소속 여부 판단. 파일 시스템에도 소속되지 않는다면 실제 파일 대상
					if (!updateToRealFile)
					{
						updateToRealFile = posOnRealPath;
					}
				}
			}
			info.updateToRealFile = updateToRealFile;
		}
	}
	
	// 하위 디렉토리 검사
	subObjs.Clear();
	MkPathName::__GetSubDirectories(node, subObjs);
	MK_INDEXING_LOOP(subObjs, i)
	{
		MkPathName newOffset = pathOffset + subObjs[i];
		newOffset.CheckAndAddBackslash();
		_FindFilesToDownload(*node.GetChildNode(subObjs[i]), newOffset, moduleFileName); // 재귀
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
