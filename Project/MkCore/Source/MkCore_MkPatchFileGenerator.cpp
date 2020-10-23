
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
		MK_DEV_PANEL.MsgToLog(L"  런쳐 파일명 설정");
		MK_DEV_PANEL.InsertEmptyLine();
	}
}

void MkPatchFileGenerator::SetRunFilePath(const MkPathName& filePath)
{
	m_RunFilePath = filePath;

	if (!m_RunFilePath.Empty())
	{
		MK_DEV_PANEL.MsgToLog(L"> " + m_RunFilePath);
		MK_DEV_PANEL.MsgToLog(L"  실행 파일명 설정");
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
		msg += L"history 폴더가 존재하지 않습니다.";
		msg += MkStr::LF;
		msg += MkStr::LF;
		msg += L"생성 후 초기화 하시겠습니까?";
		
		int rlt = ::MessageBox(m_hWnd, msg.GetPtr(), L"MkPatchFileGenerator", MB_YESNO);
		if (rlt == IDYES)
		{
			targetDir.MakeDirectoryPath(); // 생성
		}
		else if (rlt == IDNO)
		{
			targetDir.Clear();
			targetDir.GetDirectoryPathFromDialog(L"기존 history 폴더 선택.", m_hWnd, MkPathName::GetRootDirectory()); // 선택
		}
	}

	m_HistoryDirPath = targetDir;
	bool ok = !m_HistoryDirPath.Empty();
	if (ok)
	{
		m_LastHistory.Clear();
		
		MK_DEV_PANEL.MsgToLog(L"> " + m_HistoryDirPath);
		MK_DEV_PANEL.MsgToLog(L"  history 폴더 설정");
		MK_DEV_PANEL.InsertEmptyLine();

		// history file table 구성
		m_HistoryList.Clear();
		
		MkArray<MkPathName> filePathList, extensionFilter;
		extensionFilter.PushBack(MkDataNode::DefaultFileExtension);
		m_HistoryDirPath.GetWhiteFileList(filePathList, NULL, &extensionFilter, NULL, NULL, false, false); // 확장자로 필터링
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
			MK_DEV_PANEL.MsgToLog(L"> 기존 history 파일이 없습니다.");
		}
		else
		{
			m_HistoryList.SortInDescendingOrder();

			MK_DEV_PANEL.MsgToLog(L"> 기존 history 파일 : " + MkStr(m_HistoryList.GetSize()) + L" 개");
			MK_DEV_PANEL.MsgToLog(L"  마지막 history : " + ConvertWrittenTimeToStr(m_HistoryList[0]));
		}
		MK_DEV_PANEL.InsertEmptyLine();
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(L">  history 폴더 설정 실패!!! 확인이 필요합니다.");
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
		msg += L"patch root 폴더가 존재하지 않습니다.";
		msg += MkStr::LF;
		msg += MkStr::LF;
		msg += L"생성 후 초기화 하시겠습니까?";
		
		int rlt = ::MessageBox(m_hWnd, msg.GetPtr(), L"MkPatchFileGenerator", MB_YESNO);
		if (rlt == IDYES)
		{
			targetDir.MakeDirectoryPath(); // 생성
		}
		else if (rlt == IDNO)
		{
			targetDir.Clear();
			targetDir.GetDirectoryPathFromDialog(L"기존 patch root 폴더 선택.", m_hWnd, MkPathName::GetRootDirectory()); // 선택
		}
	}

	m_PatchRootPath = targetDir;
	bool ok = !m_PatchRootPath.Empty();
	if (ok)
	{
		MK_DEV_PANEL.MsgToLog(L"> " + m_PatchRootPath);
		MK_DEV_PANEL.MsgToLog(L"  patch root 폴더 설정.");
		MK_DEV_PANEL.InsertEmptyLine();

		m_PatchDataPath = m_PatchRootPath + PatchDataDirName;
		MK_DEV_PANEL.MsgToLog(L"> " + m_PatchDataPath);
		if (m_PatchDataPath.CheckAvailable())
		{
			MK_DEV_PANEL.MsgToLog(L"  patch data 폴더 설정.");
		}
		else
		{
			m_PatchDataPath.MakeDirectoryPath(); // 생성
			MK_DEV_PANEL.MsgToLog(L"  patch data 폴더가 없어 생성합니다.");
		}
		MK_DEV_PANEL.InsertEmptyLine();
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(L">  patch root 폴더 설정 실패!!! 확인이 필요합니다.");
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
		msg += L"updating root 폴더가 존재하지 않습니다.";
		msg += MkStr::LF;
		msg += MkStr::LF;
		msg += L"생성 후 초기화 하시겠습니까?";
		
		int rlt = ::MessageBox(m_hWnd, msg.GetPtr(), L"MkPatchFileGenerator", MB_YESNO);
		if (rlt == IDYES)
		{
			targetDir.MakeDirectoryPath(); // 생성
		}
		else if (rlt == IDNO)
		{
			targetDir.Clear();
			targetDir.GetDirectoryPathFromDialog(L"기존 updating root 폴더 선택.", m_hWnd, MkPathName::GetRootDirectory()); // 선택
		}
	}

	m_UpdatingRootPath = targetDir;
	bool ok = !m_UpdatingRootPath.Empty();
	if (ok)
	{
		MK_DEV_PANEL.MsgToLog(L"> " + m_UpdatingRootPath);
		MK_DEV_PANEL.MsgToLog(L"  updating root 폴더 설정.");
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
				MK_DEV_PANEL.MsgToLog(L"  updating data 폴더 설정.");
			}
			else
			{
				currPath.MakeDirectoryPath(); // 생성
				MK_DEV_PANEL.MsgToLog(L"  updating data 폴더가 없어 생성합니다.");
			}
			MK_DEV_PANEL.InsertEmptyLine();
		}
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(L">  updating root 폴더 설정 실패!!! 확인이 필요합니다.");
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
			MK_DEV_PANEL.MsgToLog(L"> 지정된 history 폴더가 없습니다.");
			break;
		}

		if (m_PatchRootPath.Empty())
		{
			MK_DEV_PANEL.MsgToLog(L"> 지정된 patch root 폴더가 없습니다.");
			break;
		}

		if (m_UpdatingRootPath.Empty())
		{
			MK_DEV_PANEL.MsgToLog(L"> 지정된 updating root 폴더가 없습니다.");
			break;
		}

		// 현 갱신 시간
		CTime ct = CTime::GetCurrentTime();
		m_UpdateWrittenTime = MkPathName::ConvertWrittenTime(ct.GetYear(), ct.GetMonth(), ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond());

		// 원본 확인
		if (!_LoadCurrentStructure(sourceDirPath))
			break;

		// 마지막 history 확인
		m_LastHistory.Clear();
		unsigned int targetHistoryIndex = 0;

		if (m_HistoryList.Empty())
		{
			MK_DEV_PANEL.MsgToLog(L"> 초기 상태이므로 원본 파일 정보를 그대로 사용합니다.");
			MK_DEV_PANEL.InsertEmptyLine();
		}
		else
		{
			targetHistoryIndex = m_HistoryList[0];
			if (targetHistoryIndex >= m_UpdateWrittenTime)
			{
				MK_DEV_PANEL.MsgToLog(L"> 현재 시간은 대상보다 최신이어야 합니다.");
				MK_DEV_PANEL.MsgToLog(L"  현재 시간 : " + ConvertWrittenTimeToStr(m_UpdateWrittenTime));
				MK_DEV_PANEL.MsgToLog(L"  대상 시간 : " + ConvertWrittenTimeToStr(targetHistoryIndex));
				MK_DEV_PANEL.MsgToLog(L"  갱신을 취소합니다.");
				break;
			}

			if (!_LoadTargetHistoryFile(targetHistoryIndex))
				break;
		}

		// 변경사항 노드 생성
		MkDataNode* updateNode = m_CurrentHistory.CreateChildNode(KEY_UpdateNode);
		if (updateNode == NULL)
		{
			MK_DEV_PANEL.MsgToLog(L"> " + KEY_UpdateNode.GetString() + L" 노드 생성 실패.");
			break;
		}

		// purge list 인계
		MkDataNode* lastUpdateNode = m_LastHistory.GetChildNode(KEY_UpdateNode);
		if (lastUpdateNode != NULL)
		{
			MkDataNode* purgeListNode = lastUpdateNode->GetChildNode(KEY_PurgeListNode);
			if ((purgeListNode != NULL) && (purgeListNode->GetChildNodeCount() > 0))
			{
				purgeListNode->DetachFromParentNode();
				updateNode->AttachChildNode(purgeListNode);
			}
			m_LastHistory.RemoveChildNode(KEY_UpdateNode); // 해당 노드는 더 이상 필요 없음
			lastUpdateNode = NULL;
		}

		// 실행파일 경로 생성
		if (!m_RunFilePath.Empty())
		{
			if (!m_CurrentHistory.CreateUnit(KEY_RunFilePath, m_RunFilePath))
			{
				MK_DEV_PANEL.MsgToLog(L"> " + KEY_RunFilePath.GetString() + L" unit 생성 실패.");
				break;
			}
		}

		// 변경사항(추가, 수정, 삭제) 탐색
		if (!_FindDifferenceBetweenTwoNode
			(MkStr::EMPTY,
			(targetHistoryIndex == 0) ? NULL : m_LastHistory.GetChildNode(KEY_StructureNode),
			m_CurrentHistory.GetChildNode(KEY_StructureNode),
			*updateNode, m_UpdateWrittenTime))
		{
			MK_DEV_PANEL.MsgToLog(L"> 변경사항 탐색 실패.");
			break;
		}

		m_LastHistory.Clear(); // 더 이상 필요 없음

		m_CurrentHistory.CreateUnit(KEY_CurrWT, m_UpdateWrittenTime);
		m_CurrentHistory.CreateUnit(KEY_LastWT, targetHistoryIndex);

		MK_DEV_PANEL.MsgToLog(L"> 갱신 성공 : " + ConvertWrittenTimeToStr(m_UpdateWrittenTime));
		
		ok = true; // 여기까지 오면 성공

		MkStr msg;
		msg.Reserve(512);
		msg += L"갱신 성공 : ";
		msg += ConvertWrittenTimeToStr(m_UpdateWrittenTime);
		
		bool changed = false;
		changed |= _CheckUpdateType(*updateNode, KEY_AddNode, L"  - 추가 : ", msg);
		changed |= _CheckUpdateType(*updateNode, KEY_ReplaceNode, L"  - 수정 : ", msg);
		changed |= _CheckUpdateType(*updateNode, KEY_DeleteNode, L"  - 삭제 : ", msg);
		msg += MkStr::LF;

		if (changed || m_HistoryList.Empty())
		{
			if (m_HistoryList.Empty())
			{
				msg += L"완전 초기 상태로 ";
			}

			msg += L"패치 파일을 만드시겠습니까?";
			if (::MessageBox(m_hWnd, msg.GetPtr(), L"MkPatchFileGenerator", MB_YESNO) == IDNO)
			{
				MK_DEV_PANEL.MsgToLog(L"  갱신을 취소합니다.");
				break;
			}
		}
		else
		{
			msg += L"변경 사항 없음. 출력 무시."; // 성공이지만 무의미
			::MessageBox(m_hWnd, msg.GetPtr(), L"MkPatchFileGenerator", MB_OK);

			MK_DEV_PANEL.MsgToLog(L"  변경 사항 없음. 출력 무시.");
			break;
		}
		MK_DEV_PANEL.InsertEmptyLine();

		// 패치 파일 갱신 시작
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

			MK_DEV_PANEL.MsgToLog(L"> working directory를 갱신하고 backup을 만듭니다.");
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
		
				// PopBack()을 위한 역순 실행
				MkPathName targetFilePath = currList[currList.GetSize() - 1].GetString(); 

				// updating용 파일은 서버에서 다운 가능해야 하기 때문에 전용 확장자를 사용
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

						// 파일을 열어 압축 시도
						MkByteArray srcData;
						MkInterfaceForFileReading frInterface;
						if (!frInterface.SetUp(srcFilePath))
						{
							MK_DEV_PANEL.MsgToLog(L"   원본 파일 없음. 갱신을 취소합니다.");
							break;
						}

						unsigned int srcSize = (m_TryCompress) ? frInterface.Read(srcData, MkArraySection(0)) : frInterface.GetFileSize();
						frInterface.Clear();

						MkByteArray compData; // 압축 후 데이터
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
									compData.Clear(); // 압축 후 크기가 지정된 비율보다 높으면(압축률이 낮으면) 원본 파일 그대로 복사
								}
							}
						}

						if (compData.Empty())
						{
							// 원본 파일 그대로 복사
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
								MK_DEV_PANEL.MsgToLog(L"   복사 중 실패. 갱신을 취소합니다.");
								break;
							}
						}
						else
						{
							// 압축 데이터를 기록
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
								MK_DEV_PANEL.MsgToLog(L"   압축파일 기록 중 실패. 갱신을 취소합니다.");
								break;
							}
							
							// 파일 크기 정보 수정
							MkDataNode* fileInfo = _GetSourceFileInfoNode(targetFilePath);
							if (fileInfo == NULL)
							{
								MK_DEV_PANEL.MsgToLog(L"   압축파일 정보 기록을 위한 노드가 없음. 갱신을 취소합니다.");
								break;
							}

							MkArray<unsigned int> sizeBuf(2);
							sizeBuf.PushBack(srcSize);
							sizeBuf.PushBack(compData.GetSize());
							if (!fileInfo->SetData(MkPathName::KeyFileSize, sizeBuf))
							{
								MK_DEV_PANEL.MsgToLog(L"   압축파일 저장 후 정보 수정 실패. 갱신을 취소합니다.");
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
							if ((i == 0) && (!success)) // full patch data에서는 반드시 지워져야 함
							{
								error = true;
								break;
							}
						}
						if (error)
						{
							MK_DEV_PANEL.MsgToLog(L"   삭제 중 실패. 갱신을 취소합니다.");
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
				// structure node의 root에 m_LauncherFileName과 같은 파일 노드가 존재하면(패치되었으면) size, written time 복사
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
					MK_DEV_PANEL.MsgToLog(L"> " + LauncherFileName.GetString() + L" unit 생성 중 실패.");
					MK_DEV_PANEL.InsertEmptyLine();
					m_MainState = eReady;
				}
			}
			else
			{
				MK_DEV_PANEL.MsgToLog(L"> " + KEY_LauncherNode.GetString() + L" 노드 생성 중 실패.");
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
			MK_DEV_PANEL.MsgToLog(L"  경로에 history 파일 저장 완료.");
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

			// patch info file. m_CurrentHistory를 가공해 만듬
			// KEY_UpdateNode 안의 KEY_PurgeListNode 노드만 밖으로 꺼내고 나머지는 삭제
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

				// file system 설정 반영. root directory에 MkFileSystem::DefaultSettingFileName 파일 그대로 사용
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

				// updating용 파일은 서버에서 다운 가능해야 하기 때문에 전용 확장자를 사용
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
				MK_DEV_PANEL.MsgToLog(L"  경로에 파일 저장 실패. 갱신을 취소합니다.");
			}
			else
			{
				MK_DEV_PANEL.MsgToLog(L"  경로에 파일 저장 완료.");

				m_HistoryList.Insert(0, m_UpdateWrittenTime); // history list에 등록

				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"-------------------------------------");
				MK_DEV_PANEL.MsgToLog(L"[ 패치 파일 생성이 완료되었습니다. ]");
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
			if (enable[j]) // MkArray::IntersectionTest와는 달리 대소문자를 구별하지 않음
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
	for (unsigned int i=0; i<targetSize; ++i) // 일치되지 않은 나머지는 targetOnly에 등록
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
		MK_DEV_PANEL.MsgToLog(KEY_StructureNode.GetString() + L" 노드 생성 실패.");
		return false;
	}
	
	m_SourcePath.Clear();
	m_SourcePath.ConvertToRootBasisAbsolutePath(sourceDirPath);
	bool ok = (m_SourcePath.ExportSystemStructure(*structureNode) != 0);
	if (ok)
	{
		m_CurrentHistory.SetDataTypeTag(DataType_PatchHistory); // tag 삽입
		
		MK_DEV_PANEL.MsgToLog(L"> " + m_SourcePath);
		MK_DEV_PANEL.MsgToLog(L"  위치의 현 원본 파일 정보 구축 완료.");
	}
	else
	{
		if (m_SourcePath.IsDirectoryPath() && m_SourcePath.CheckAvailable())
		{
			// 디렉토리는 존재하지만 파일은 없음. 완전 초기 상태
			MkStr msg;
			msg.Reserve(512);
			msg += m_SourcePath;
			msg += MkStr::LF;
			msg += L"위치에 원본 파일이 없습니다.";
			msg += MkStr::LF;
			msg += MkStr::LF;
			msg += L"올바른 상태입니까?";
			
			int rlt = ::MessageBox(m_hWnd, msg.GetPtr(), L"MkPatchFileGenerator", MB_YESNO);
			if (rlt == IDYES)
			{
				m_CurrentHistory.SetDataTypeTag(DataType_PatchHistory); // tag 삽입

				ok = true;
			}
			else if (rlt == IDNO)
			{
				MK_DEV_PANEL.MsgToLog(L"> " + m_SourcePath);
				MK_DEV_PANEL.MsgToLog(L"  위치에 현 원본 파일이 없습니다. 확인이 필요합니다.");
			}
		}
		else
		{
			MK_DEV_PANEL.MsgToLog(L"> " + m_SourcePath);
			MK_DEV_PANEL.MsgToLog(L"  위치가 잘못되었습니다. 확인이 필요합니다.");
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
			MK_DEV_PANEL.MsgToLog(L"> " + MkStr(targetIndex) + L" 대상 history는 존재하지 않습니다.");
			break;
		}

		MkPathName targetFilePath = m_HistoryDirPath + MkStr(targetIndex) + L"." + MkDataNode::DefaultFileExtension;
		MK_DEV_PANEL.MsgToLog(L"> " + targetFilePath);

		if (!m_LastHistory.Load(targetFilePath))
		{
			MK_DEV_PANEL.MsgToLog(L"  대상 history 파일이 없거나 MkDataNode 형식이 아닙니다.");
			break;
		}

		if (m_LastHistory.GetDataTypeTag() != DataType_PatchHistory)
		{
			MK_DEV_PANEL.MsgToLog(L"  대상 history 파일은 올바른 형식의 파일이 아닙니다.");
			break;
		}

		if (!m_LastHistory.ChildExist(KEY_StructureNode))
		{
			MK_DEV_PANEL.MsgToLog(L"  대상 history 파일에 " + KEY_StructureNode.GetString() + L" 노드가 없습니다.");
			break;
		}

		if (!m_LastHistory.ChildExist(KEY_UpdateNode))
		{
			MK_DEV_PANEL.MsgToLog(L"  대상 history 파일에 " + KEY_UpdateNode.GetString() + L" 노드가 없습니다.");
			break;
		}
		
		MK_DEV_PANEL.MsgToLog(L"  대상 history 파일 로딩 완료 : " + ConvertWrittenTimeToStr(targetIndex));
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
		filePath.Tokenize(tokens, L"\\"); // 무조건 성공

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
			MkStr buffer = msgPrefix + MkStr(count) + L" 개 파일.";
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
			targetPath += dirName.GetString(); // targetPath(root로부터의 상대 경로)는 unique함이 보장 됨
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

	// 디렉토리 전체가 새로 등장 -> currDirNode 하위 모든 파일 add
	if ((!hasLastDir) && hasCurrDir)
	{
		if (!_SetAllDirsToUpdate(dirPath, *currDirNode, updateRoot, KEY_AddNode, true, -1, writtenTime))
			return false;
	}
	// 디렉토리 전체가 사라짐 -> lastDirNode 하위 모든 파일 delete
	else if (hasLastDir && (!hasCurrDir))
	{
		if (!_SetAllDirsToUpdate(dirPath, *lastDirNode, updateRoot, KEY_DeleteNode, false, 1, writtenTime)) // delete는 file size, written time 불필요
			return false;
	}
	// 동일 디렉토리 -> 하위 개별 비교
	else if (hasLastDir && hasCurrDir)
	{
		// [ 파일 비교 ]
		MkArray<MkHashStr> lastObjects, currObjects;
		MkPathName::__GetSubFiles(*lastDirNode, lastObjects);
		MkPathName::__GetSubFiles(*currDirNode, currObjects);

		// 교차 판정. 대소문자 구별하지 않음
		MkArray<MkHashStr> delList, addList;
		MkArray<unsigned int> lastIntersection, currIntersection;
		IntersectionTest(lastObjects, currObjects, delList, lastIntersection, currIntersection, addList);
		
		// delList, addList와는 달리 intersection은 크기와 수정시간을 비교해 갱신 결정
		MkArray<MkHashStr> repList(lastIntersection.GetSize());
		MK_INDEXING_LOOP(lastIntersection, i)
		{
			const MkHashStr& lastFileName = lastObjects[lastIntersection[i]];
			const MkHashStr& currFileName = currObjects[currIntersection[i]];
			if (MkPathName::__CheckFileDifference(*lastDirNode->GetChildNode(lastFileName), *currDirNode->GetChildNode(currFileName)))
			{
				repList.PushBack(currFileName); // 현재 파일명 기준
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

		// [ 디렉토리 비교 ]
		MkPathName::__GetSubDirectories(*lastDirNode, lastObjects);
		MkPathName::__GetSubDirectories(*currDirNode, currObjects);

		// 검증용 교차 판정. 대소문자 구별하지 않음
		IntersectionTest(lastObjects, currObjects, delList, lastIntersection, currIntersection, addList);
		delList.Clear();
		addList.Clear();

		// 교차 판정. 대소문자 구별 함!!!
		MkArray<MkHashStr> intersection;
		lastObjects.IntersectionTest(currObjects, delList, intersection, addList);

		// 대소문자 구분 한/구분하지 않은 교차 크기가 다르다면 동일 경로상에 대소문자가 다른 디렉토리명이 있다는 뜻
		if (intersection.GetSize() != lastIntersection.GetSize())
		{
			if (dirPath.Empty())
			{
				MK_DEV_PANEL.MsgToLog(L"> 원본 경로 루트에 이전과는 대소문자가 다른 디렉토리가 존재.");
			}
			else
			{
				MK_DEV_PANEL.MsgToLog(L"> " + dirPath);
				MK_DEV_PANEL.MsgToLog(L"  원본 경로 하위에 이전과는 대소문자가 다른 디렉토리가 존재.");
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
			::MessageBox(NULL, L"메모리 부족, update type node 생성 실패 : " + updateType.GetString(), L"Error", MB_OK);
			return false;
		}

		MK_INDEXING_LOOP(fileList, i)
		{
			MkStr targetPath = dirPath;
			targetPath += fileList[i].GetString(); // targetPath(root로부터의 상대 경로)는 유일성이 보장 됨
			MkDataNode* fileNode = updateTypeRoot->CreateChildNode(targetPath); // file node를 생성
			if (fileNode == NULL)
			{
				::MessageBox(NULL, L"메모리 부족, file node 생성 실패 :\n" + targetPath, L"Error", MB_OK);
				return false;
			}

			if (purgeState != 0)
			{
				MkDataNode* purgeTypeRoot = updateRoot.ChildExist(KEY_PurgeListNode) ? updateRoot.GetChildNode(KEY_PurgeListNode) : updateRoot.CreateChildNode(KEY_PurgeListNode);
				if (purgeTypeRoot == NULL)
				{
					::MessageBox(NULL, L"메모리 부족, update type node 생성 실패 : " + KEY_PurgeListNode.GetString(), L"Error", MB_OK);
					return false;
				}

				if (purgeState > 0) // delete
				{
					if (purgeTypeRoot->CreateChildNode(targetPath) == NULL)
					{
						::MessageBox(NULL, L"메모리 부족, purge node 생성 실패 :\n" + targetPath, L"Error", MB_OK);
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
				*fileNode = *targetDirNode.GetChildNode(fileList[i]); // deep copy로 file size, written time 정보를 복사

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
	MkPathName::__GetSubFiles(targetDirNode, subObjects); // 하위 모든 파일 대상
	if (!_SetAllFilesToUpdate(dirPath, targetDirNode, subObjects, updateRoot, updateType, copySrcData, purgeState, writtenTime))
		return false;

	subObjects.Clear();
	MkPathName::__GetSubDirectories(targetDirNode, subObjects); // 하위 모든 디렉토리 대상
	if (!subObjects.Empty())
	{
		MK_INDEXING_LOOP(subObjects, i)
		{
			MkStr targetPath = dirPath;
			targetPath += subObjects[i].GetString(); // targetPath(root로부터의 상대 경로)는 unique함이 보장 됨
			targetPath += L"\\";
			if (!_SetAllDirsToUpdate(targetPath, *targetDirNode.GetChildNode(subObjects[i]), updateRoot, updateType, copySrcData, purgeState, writtenTime)) // 재귀
				return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------//
