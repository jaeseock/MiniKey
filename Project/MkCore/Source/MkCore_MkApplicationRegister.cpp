
#include "MkCore_MkRegistryOp.h"
#include "MkCore_MkShortcutOp.h"
#include "MkCore_MkApplicationRegister.h"


static const MkHashStr NKEY_RegisterProjectInfo = L"[RegisterProjectInfo]";  // RPI(RegisterProjectInfo)
static const MkHashStr NKEY_RegisterCustomURLScheme = L"[RegisterCustomURLScheme]"; // RCS(RegisterCustomURLScheme)
static const MkHashStr NKEY_DesktopDirectoryShortcut = L"[DesktopDirectoryShortcut]"; // SC(ShortCut)
static const MkHashStr NKEY_ProgramsShortcut = L"[ProgramsShortcut]"; // SC(ShortCut)
static const MkHashStr NKEY_ClientDeleteList = L"[ClientDeleteList]"; // CDL(ClientDeleteList)

static const MkHashStr UKEY_ClientPath = L"#ClientPath"; // GenerateServiceData only

static const MkHashStr UKEY_UninstallList = L"#UninstallList"; // all

static const MkHashStr UKEY_PublisherName = L"#PublisherName"; // RPI
static const MkHashStr UKEY_IconFile = L"#IconFile"; // RPI, SC
static const MkHashStr UKEY_HomePage = L"#HomePage"; // RPI
static const MkHashStr UKEY_UninstallApp = L"#UninstallApp"; // RPI

static const MkHashStr UKEY_TargetApp = L"#TargetApp"; // RCS, SC
static const MkHashStr UKEY_AcceptArg = L"#AcceptArg"; // RCS

static const MkHashStr UKEY_TargetURL = L"#TargetURL"; // SC
static const MkHashStr UKEY_Desc = L"#Desc"; // SC

static const MkHashStr UKEY_Files = L"#Files"; // CDL
static const MkHashStr UKEY_Dirs = L"#Dirs"; // CDL
static const MkHashStr UKEY_ChunkPrefix = L"#ChunkPrefix"; // CDL
static const MkHashStr UKEY_ChunkExtension = L"#ChunkExtension"; // CDL


//------------------------------------------------------------------------------------------------//

bool MkApplicationRegister::GenerateServiceData::LoadServices(const MkPathName& servicesFilePath, MkArray<MkHashStr>& services)
{
	// 설정 파일 체크
	MkPathName fullPath;
	fullPath.ConvertToModuleBasisAbsolutePath(servicesFilePath);

	if (fullPath.CheckAvailable())
	{
		// 설정 파일 로드
		if (!m_Services.Load(fullPath))
		{
			::MessageBox(NULL, servicesFilePath + L" 파일 내부 구성 오류. 확인 요망.", L"MkApplicationRegister ERROR!", MB_OK);
			return false;
		}

		if (m_Services.GetChildNodeCount() == 0)
		{
			::MessageBox(NULL, servicesFilePath + L" 파일 내부 구성 오류. 확인 요망.", L"MkApplicationRegister ERROR!", MB_OK);
			return false;
		}
	}
	else
	{
		MkStr alertMsg = servicesFilePath + L" 파일이 없어 기본값으로 초기화";
		::MessageBox(NULL, alertMsg.GetPtr(), L"MkApplicationRegister ALERT!", MB_OK);
		
		// 없으면 기본 구성(한국-PW 기준)
		if (!_CreateDefaultService(m_Services))
		{
			::MessageBox(NULL, servicesFilePath + L" 파일 기본 구성 중 실패", L"MkApplicationRegister ERROR!", MB_OK);
			return false;
		}

		if (!m_Services.SaveToText(fullPath))
		{
			::MessageBox(NULL, servicesFilePath + L" 파일 저장 실패", L"MkApplicationRegister ERROR!", MB_OK);
			return false;
		}
	}

	services.Clear();
	m_Services.GetChildNodeList(services);
	return true;
}

bool MkApplicationRegister::GenerateServiceData::SaveService(const MkHashStr& targetService, const MkPathName& saveFileName) const
{
	if (!m_Services.ChildExist(targetService))
		return false;

	const MkDataNode& serviceNode = *m_Services.GetChildNode(targetService);

	// 검증
	MkStr errorMsg;
	if (!_CheckServiceEnable(serviceNode, errorMsg))
	{
		MkStr msg = targetService.GetString() + L" 서비스에서," + MkStr::LF + errorMsg;
		::MessageBox(NULL, errorMsg, L"MkApplicationRegister ERROR!", MB_OK);
		return false;
	}

	// client path 추출
	MkPathName clientPath;
	MkStr cBuffer;
	serviceNode.GetData(UKEY_ClientPath, cBuffer, 0);
	clientPath.ConvertToModuleBasisAbsolutePath(cBuffer);

	// chunk info 추출
	MkStr chunkPrefix, chunkExtension;
	serviceNode.GetData(UKEY_ChunkPrefix, chunkPrefix, 0);
	serviceNode.GetData(UKEY_ChunkExtension, chunkExtension, 0);
	chunkPrefix.ToLower();
	chunkExtension.ToLower();

	// service 노드 복사 후 UKEY_ClientPath, UKEY_ChunkPrefix, UKEY_ChunkExtension 제거
	MkDataNode targetNode = serviceNode; // deep copy
	targetNode.RemoveUnit(UKEY_ClientPath);
	targetNode.RemoveUnit(UKEY_ChunkPrefix);
	targetNode.RemoveUnit(UKEY_ChunkExtension);

	// client delete list
	MkDataNode* deleteNode = targetNode.CreateChildNode(NKEY_ClientDeleteList);
	if (deleteNode == NULL)
		return false;

	// uninstall시 삭제 할 스냅샷을 작성
	MkStr findPath = clientPath + L"*.*";

	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(findPath.GetPtr(), &fd); // 최초 핸들
	if (hFind == INVALID_HANDLE_VALUE)
		return false;

	MkArray<MkStr> files(256), dirs(16);
	do
	{
		// 히든 속성 제외
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0)
		{
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) // 파일
			{
				// chunk 여부 조사
				MkPathName fileName = fd.cFileName;
				fileName.ToLower();

				bool isChunk = false;
				if (!chunkPrefix.Empty())
				{
					isChunk = fileName.CheckPrefix(chunkPrefix);
				}

				if (isChunk)
				{
					if (!chunkExtension.Empty())
					{
						isChunk = (fileName.GetFileExtension() == chunkExtension);
					}
				}

				if (!isChunk) // chunk인 경우 자동 삭제되므로 제외
				{
					files.PushBack(MkStr(fd.cFileName));
				}
			}
			else // 디렉토리
			{
				MkStr dirName = fd.cFileName;
				if ((dirName != L".") && (dirName != L".."))
				{
					dirs.PushBack(dirName);
				}
			}
		}
	}
	while (::FindNextFile(hFind, &fd));
	::FindClose(hFind);

	if (!files.Empty())
	{
		if (!deleteNode->CreateUnit(UKEY_Files, files))
			return false;
	}

	if (!dirs.Empty())
	{
		if (!deleteNode->CreateUnit(UKEY_Dirs, dirs))
			return false;
	}

	if (!chunkPrefix.Empty())
	{
		if (!deleteNode->CreateUnit(UKEY_ChunkPrefix, chunkPrefix))
			return false;
	}

	if (!chunkExtension.Empty())
	{
		if (!deleteNode->CreateUnit(UKEY_ChunkExtension, chunkExtension))
			return false;
	}

	// 저장
	MkPathName savePath = clientPath + saveFileName;
	if (!targetNode.SaveToBinary(savePath))
		return false;
	
	return true;
}

bool MkApplicationRegister::GenerateServiceData::_CreateDefaultService(MkDataNode& dataNode)
{
	MkDataNode* serviceNode = dataNode.CreateChildNode(L"Korea - Playwith");
	if (serviceNode == NULL) return false;
	{
		// register program info
		MkDataNode* rpiNode = serviceNode->CreateChildNode(NKEY_RegisterProjectInfo);
		if (rpiNode == NULL) return false;
		{
			MkDataNode* projectNode = rpiNode->CreateChildNode(L"군타라 온라인 for 플레이위드");
			if (projectNode == NULL) return false;
			{
				if (!projectNode->CreateUnit(UKEY_PublisherName, MkStr(L"Playwith-GAMES Entertainment"))) return false;
				if (!projectNode->CreateUnit(UKEY_IconFile, MkStr(L"Kuntara.ico"))) return false;
				if (!projectNode->CreateUnit(UKEY_HomePage, MkStr(L"http://www.kuntara.co.kr"))) return false;
				if (!projectNode->CreateUnit(UKEY_UninstallApp, MkStr(L"Uninstall.exe"))) return false;
			}

			if (!rpiNode->CreateUnit(UKEY_UninstallList, projectNode->GetNodeName().GetString())) return false;
		}

		// register custom url scheme
		MkDataNode* rcsNode = serviceNode->CreateChildNode(NKEY_RegisterCustomURLScheme);
		if (rcsNode == NULL) return false;
		{
			MkDataNode* protocolNode = rcsNode->CreateChildNode(L"pwkopw");
			if (protocolNode == NULL) return false;
			{
				if (!protocolNode->CreateUnit(UKEY_TargetApp, MkStr(L"Starter.exe"))) return false;
				if (!protocolNode->CreateUnit(UKEY_AcceptArg, true)) return false;
			}

			if (!rcsNode->CreateUnit(UKEY_UninstallList, protocolNode->GetNodeName().GetString())) return false;
		}

		// shortcut - desktop directory
		MkDataNode* dsNode = serviceNode->CreateChildNode(NKEY_DesktopDirectoryShortcut);
		if (dsNode == NULL) return false;
		{
			MkDataNode* shortcutNode = dsNode->CreateChildNode(L"군타라 온라인 for 플레이위드");
			if (shortcutNode == NULL) return false;
			{
				if (!shortcutNode->CreateUnit(UKEY_TargetURL, MkStr(L"http://www.kuntara.co.kr"))) return false;
				if (!shortcutNode->CreateUnit(UKEY_IconFile, MkStr(L"Kuntara.ico"))) return false;
				if (!shortcutNode->CreateUnit(UKEY_Desc, MkStr(L"군타라 온라인을 실행합니다."))) return false;
			}

			if (!dsNode->CreateUnit(UKEY_UninstallList, shortcutNode->GetNodeName().GetString())) return false;
		}

		// shortcut - programs
		MkDataNode* psNode = serviceNode->CreateChildNode(NKEY_ProgramsShortcut);
		if (psNode == NULL) return false;
		{
			MkDataNode* dirNode = psNode->CreateChildNode(L"군타라 온라인 for 플레이위드");
			if (dirNode == NULL) return false;
			{
				MkDataNode* deleteShortcutNode = dirNode->CreateChildNode(L"군타라 온라인 삭제");
				if (deleteShortcutNode == NULL) return false;
				{
					if (!deleteShortcutNode->CreateUnit(UKEY_TargetApp, MkStr(L"Uninstall.exe"))) return false;
					if (!deleteShortcutNode->CreateUnit(UKEY_IconFile, MkStr(L"Uninstall.exe"))) return false;
					if (!deleteShortcutNode->CreateUnit(UKEY_Desc, MkStr(L"군타라 온라인을 삭제합니다."))) return false;
				}

				MkDataNode* runShortcutNode = dirNode->CreateChildNode(L"군타라 온라인");
				if (runShortcutNode == NULL) return false;
				{
					if (!runShortcutNode->CreateUnit(UKEY_TargetURL, MkStr(L"http://www.kuntara.co.kr"))) return false;
					if (!runShortcutNode->CreateUnit(UKEY_IconFile, MkStr(L"Kuntara.ico"))) return false;
					if (!runShortcutNode->CreateUnit(UKEY_Desc, MkStr(L"군타라 온라인을 실행합니다."))) return false;
				}
			}

			if (!psNode->CreateUnit(UKEY_UninstallList, dirNode->GetNodeName().GetString())) return false;
		}

		// client path
		MkPathName clientPath;
		clientPath.GetDirectoryPathFromDialog(L"[ " + serviceNode->GetNodeName().GetString() + L" ] 서비스의 클라이언트 디렉토리 지정");
		if (clientPath.Empty()) return false;

		clientPath.BackSpace(1);
		if (!serviceNode->CreateUnit(UKEY_ClientPath, MkStr(clientPath))) return false;

		// chunk info
		if (!serviceNode->CreateUnit(UKEY_ChunkPrefix, MkStr(L"MK_PACK_"))) return false;
		if (!serviceNode->CreateUnit(UKEY_ChunkExtension, MkStr(L"mpc"))) return false;
	}
	return true;
}

bool MkApplicationRegister::GenerateServiceData::_CheckServiceEnable(const MkDataNode& serviceNode, MkStr& errorMsg)
{
	// client path
	MkPathName clientPath;
	{
		MkStr buffer;
		if (serviceNode.GetData(UKEY_ClientPath, buffer, 0) && (!buffer.Empty()))
		{
			clientPath.ConvertToModuleBasisAbsolutePath(buffer);
			if (!clientPath.CheckAvailable())
			{
				errorMsg = UKEY_ClientPath.GetString() + L" 값의 " + clientPath + L" 경로가 존재하지 않음";
				return false;
			}
		}
		else
		{
			errorMsg = UKEY_ClientPath.GetString() + L" 값 확인";
			return false;
		}
	}

	// register program info
	const MkDataNode* rpiNode = serviceNode.GetChildNode(NKEY_RegisterProjectInfo);
	if (rpiNode == NULL)
	{
		errorMsg = NKEY_RegisterProjectInfo.GetString() + L" 노드가 없음";
		return false;
	}

	if (rpiNode->GetChildNodeCount() == 0)
	{
		errorMsg = NKEY_RegisterProjectInfo.GetString() + L" 노드에 등록 프로젝트가 없음";
		return false;
	}

	if (!_CheckPathEnable(*rpiNode, UKEY_UninstallApp, clientPath, errorMsg))
		return false;

	// register custom url scheme
	const MkDataNode* rcsNode = serviceNode.GetChildNode(NKEY_RegisterCustomURLScheme);
	if (rcsNode != NULL)
	{
		if (!_CheckPathEnable(*rcsNode, UKEY_TargetApp, clientPath, errorMsg))
			return false;
	}

	// shortcut - desktop directory
	const MkDataNode* dsNode = serviceNode.GetChildNode(NKEY_DesktopDirectoryShortcut);
	if (dsNode == NULL)
	{
		if (!_CheckShortcutEnable(*dsNode, clientPath, errorMsg))
			return false;
	}

	// shortcut - programs
	const MkDataNode* psNode = serviceNode.GetChildNode(NKEY_ProgramsShortcut);
	if (psNode == NULL)
	{
		if (!_CheckShortcutEnable(*psNode, clientPath, errorMsg))
			return false;
	}

	return true;
}

bool MkApplicationRegister::GenerateServiceData::_CheckPathEnable
(const MkDataNode& targetNode, const MkHashStr& fileKey, const MkPathName& clientPath, MkStr& errorMsg)
{
	MkArray<MkHashStr> children;
	if (targetNode.GetChildNodeList(children) > 0)
	{
		MK_INDEXING_LOOP(children, i)
		{
			const MkHashStr& currKey = children[i];
			MkStr buffer;
			if ((!targetNode.GetChildNode(currKey)->GetData(fileKey, buffer, 0)) || buffer.Empty())
			{
				errorMsg = currKey.GetString() + L" -> " + fileKey.GetString() + L" 값이 없음";
				return false;
			}

			MkPathName appPath = clientPath + buffer;
			if (!appPath.CheckAvailable())
			{
				errorMsg = currKey.GetString() + L" -> " + fileKey.GetString() + L" 파일이 존재하지 않음";
				return false;
			}
		}
	}
	return true;
}

bool MkApplicationRegister::GenerateServiceData::_CheckShortcutEnable(const MkDataNode& targetNode, const MkPathName& clientPath, MkStr& errorMsg)
{
	MkArray<MkHashStr> children;
	if (targetNode.GetChildNodeList(children) > 0)
	{
		MK_INDEXING_LOOP(children, i)
		{
			const MkHashStr& currKey = children[i];
			const MkDataNode& childNode = *targetNode.GetChildNode(currKey);

			bool hasApp = childNode.IsValidKey(UKEY_TargetApp);
			bool hasURL = childNode.IsValidKey(UKEY_TargetURL);
			if (hasApp && hasURL) // 둘 중 하나만 존재해야 함
			{
				errorMsg = currKey.GetString() + L" 에 " + UKEY_TargetApp.GetString() + L", " + UKEY_TargetURL.GetString() + L" 모두 존재";
				return false;
			}
			else if ((!hasApp) && (!hasURL)) // 둘 모두 없으면 directory
			{
				if (!_CheckShortcutEnable(childNode, clientPath, errorMsg)) // 재귀
					return false;
			}

			if (hasApp) // url은 검사 필요 없음
			{
				MkStr buffer;
				childNode.GetData(UKEY_TargetApp, buffer, 0);
				if (buffer.Empty())
				{
					errorMsg = currKey.GetString() + L" -> " + UKEY_TargetApp.GetString() + L" 값이 없음";
					return false;
				}

				MkPathName appPath = clientPath + buffer;
				if (!appPath.CheckAvailable())
				{
					errorMsg = currKey.GetString() + L" -> " + UKEY_TargetApp.GetString() + L" 파일이 존재하지 않음";
					return false;
				}
			}
		}
	}
	return true;
}

//------------------------------------------------------------------------------------------------//

static void __GetShortcutInfo(const MkDataNode& targetNode, const MkPathName& currPath, MkArray< MkArray<MkStr> >& infos)
{
	MkArray<MkHashStr> children;
	if (targetNode.GetChildNodeList(children) > 0)
	{
		MK_INDEXING_LOOP(children, i)
		{
			const MkHashStr& currKey = children[i];
			const MkDataNode& childNode = *targetNode.GetChildNode(currKey);
			MkPathName childPath = currPath.Empty() ? currKey.GetString() : (currPath + L"\\" + currKey.GetString());
			MkPathName targetPath;

			bool hasApp = childNode.IsValidKey(UKEY_TargetApp);
			bool hasURL = childNode.IsValidKey(UKEY_TargetURL);

			if (hasApp) // app 대상이면
			{
				MkStr buffer;
				if (childNode.GetData(UKEY_TargetApp, buffer, 0) && (!buffer.Empty()))
				{
					targetPath.ConvertToModuleBasisAbsolutePath(buffer);
				}
			}
			else if (hasURL) // url 대상이면
			{
				childNode.GetData(UKEY_TargetURL, targetPath, 0);
			}
			else if ((!hasApp) && (!hasURL)) // 둘 모두 없으면 directory
			{
				__GetShortcutInfo(childNode, childPath, infos); // 재귀
			}

			if (!targetPath.Empty()) // 대상이 존재하면 shortcut
			{
				MkStr iconFile, desc;
				childNode.GetData(UKEY_IconFile, iconFile, 0);
				childNode.GetData(UKEY_Desc, desc, 0);

				MkArray<MkStr>& shortcutInfo = infos.PushBack();
				shortcutInfo.Reserve(4);
				shortcutInfo.PushBack(childPath);
				shortcutInfo.PushBack(targetPath);
				shortcutInfo.PushBack(desc);
				shortcutInfo.PushBack(iconFile);
			}
		}
	}
}

static void __GetUninstallList(const MkDataNode& targetNode, const MkPathName& currPath, MkArray<MkStr>& uninstallList)
{
	// 현재 node의 uninstall list
	MkArray<MkHashStr> ulist;
	if (targetNode.GetDataEx(UKEY_UninstallList, ulist))
	{
		MK_INDEXING_LOOP(ulist, i)
		{
			const MkHashStr& currKey = ulist[i];
			uninstallList.PushBack(currPath.Empty() ? currKey.GetString() : (currPath + L"\\" + currKey.GetString()));
		}
	}

	MkArray<MkHashStr> children;
	if (targetNode.GetChildNodeList(children) > 0)
	{
		// 자식 중 차집합만을 대상으로 탐색
		MkArray<MkHashStr> alives;
		children.GetDifferenceOfSets(ulist, alives); // children - ulist;

		MK_INDEXING_LOOP(alives, i)
		{
			const MkHashStr& currKey = alives[i];
			MkPathName childPath = currPath.Empty() ? currKey.GetString() : (currPath + L"\\" + currKey.GetString());
			__GetUninstallList(*targetNode.GetChildNode(currKey), childPath, uninstallList); // 재귀
		}
	}
}

static void __GetUninstallList(const MkDataNode& targetNode, const MkPathName& currPath, MkArray<MkStr>& uninstallFileList, MkArray<MkStr>& uninstallDirList)
{
	// 현재 node의 uninstall list
	MkArray<MkHashStr> ulist;
	if (targetNode.GetDataEx(UKEY_UninstallList, ulist))
	{
		MK_INDEXING_LOOP(ulist, i)
		{
			const MkHashStr& currKey = ulist[i];
			const MkDataNode* childNode = targetNode.GetChildNode(currKey);
			if (childNode != NULL)
			{
				if (childNode->IsValidKey(UKEY_TargetApp) || childNode->IsValidKey(UKEY_TargetURL))
				{
					uninstallFileList.PushBack(currPath.Empty() ? currKey.GetString() : (currPath + L"\\" + currKey.GetString())); // shortcut
				}
				else
				{
					uninstallDirList.PushBack(currPath.Empty() ? currKey.GetString() : (currPath + L"\\" + currKey.GetString())); // directory
				}
			}
		}
	}

	MkArray<MkHashStr> children;
	if (targetNode.GetChildNodeList(children) > 0)
	{
		// 자식 중 차집합만을 대상으로 탐색
		MkArray<MkHashStr> alives;
		children.GetDifferenceOfSets(ulist, alives); // children - ulist;

		MK_INDEXING_LOOP(alives, i)
		{
			const MkHashStr& currKey = alives[i];
			MkPathName childPath = currPath.Empty() ? currKey.GetString() : (currPath + L"\\" + currKey.GetString());
			__GetUninstallList(*targetNode.GetChildNode(currKey), childPath, uninstallFileList, uninstallDirList); // 재귀
		}
	}
}

//------------------------------------------------------------------------------------------------//

bool MkApplicationRegister::UpdateService(const MkPathName& targetServicesFilePath)
{
	MkPathName fullPath;
	fullPath.ConvertToModuleBasisAbsolutePath(targetServicesFilePath);
	if (!fullPath.CheckAvailable())
		return false;

	MkDataNode serviceNode;
	if (!serviceNode.Load(fullPath))
		return false;

	// register program info
	const MkDataNode* rpiNode = serviceNode.GetChildNode(NKEY_RegisterProjectInfo);
	if (rpiNode != NULL)
	{
		MkArray<MkHashStr> children;
		if (rpiNode->GetChildNodeList(children) > 0)
		{
			MK_INDEXING_LOOP(children, i)
			{
				const MkHashStr& currKey = children[i];
				const MkDataNode& childNode = *rpiNode->GetChildNode(currKey);

				MkStr publisherName, iconFile, homePage, uninstallApp;
				childNode.GetData(UKEY_PublisherName, publisherName, 0);
				childNode.GetData(UKEY_IconFile, iconFile, 0);
				childNode.GetData(UKEY_HomePage, homePage, 0);
				childNode.GetData(UKEY_UninstallApp, uninstallApp, 0);

				MkRegistryOperator::RegistApplicationInfo(currKey.GetString(), currKey.GetString(), iconFile, publisherName, homePage, uninstallApp);
			}
		}
	}

	// register custom url scheme
	const MkDataNode* rcsNode = serviceNode.GetChildNode(NKEY_RegisterCustomURLScheme);
	if (rcsNode != NULL)
	{
		MkArray<MkHashStr> children;
		if (rcsNode->GetChildNodeList(children) > 0)
		{
			MK_INDEXING_LOOP(children, i)
			{
				const MkHashStr& currKey = children[i];
				const MkDataNode& childNode = *rcsNode->GetChildNode(currKey);

				MkStr targetApp;
				childNode.GetData(UKEY_TargetApp, targetApp, 0);
				bool acceptArg = false;
				childNode.GetData(UKEY_AcceptArg, acceptArg, 0);

				MkRegistryOperator::RegistCustomURLScheme(currKey.GetString(), targetApp, acceptArg);
			}
		}
	}

	// shortcut - desktop directory
	const MkDataNode* dsNode = serviceNode.GetChildNode(NKEY_DesktopDirectoryShortcut);
	if (dsNode != NULL)
	{
		MkArray< MkArray<MkStr> > infos;
		__GetShortcutInfo(*dsNode, MkStr::EMPTY, infos);

		MK_INDEXING_LOOP(infos, i)
		{
			const MkArray<MkStr>& currInfo = infos[i];
			MkShortcutOperator::CreateShortcutToCommonDesktopDirectory(currInfo[0], currInfo[1], currInfo[2], currInfo[3]);
		}
	}

	// shortcut - programs
	const MkDataNode* psNode = serviceNode.GetChildNode(NKEY_ProgramsShortcut);
	if (psNode != NULL)
	{
		MkArray< MkArray<MkStr> > infos;
		__GetShortcutInfo(*psNode, MkStr::EMPTY, infos);

		MK_INDEXING_LOOP(infos, i)
		{
			const MkArray<MkStr>& currInfo = infos[i];
			MkShortcutOperator::CreateShortcutToCommonPrograms(currInfo[0], currInfo[1], currInfo[2], currInfo[3]);
		}
	}

	return true;
}

bool MkApplicationRegister::UninstallService(const MkPathName& targetServicesFilePath)
{
	MkPathName fullPath;
	fullPath.ConvertToModuleBasisAbsolutePath(targetServicesFilePath);
	if (!fullPath.CheckAvailable())
		return false;

	MkDataNode serviceNode;
	if (!serviceNode.Load(fullPath))
		return false;

	// register program info
	const MkDataNode* rpiNode = serviceNode.GetChildNode(NKEY_RegisterProjectInfo);
	if (rpiNode != NULL)
	{
		MkArray<MkStr> uninstallList;
		__GetUninstallList(*rpiNode, MkStr::EMPTY, uninstallList);

		MK_INDEXING_LOOP(uninstallList, i)
		{
			MkRegistryOperator::DeleteApplicationInfo(uninstallList[i]);
		}
	}

	// register custom url scheme
	const MkDataNode* rcsNode = serviceNode.GetChildNode(NKEY_RegisterCustomURLScheme);
	if (rcsNode != NULL)
	{
		MkArray<MkStr> uninstallList;
		__GetUninstallList(*rcsNode, MkStr::EMPTY, uninstallList);

		MK_INDEXING_LOOP(uninstallList, i)
		{
			MkRegistryOperator::DeleteCustomURLScheme(uninstallList[i]);
		}
	}

	// shortcut - desktop directory
	const MkDataNode* dsNode = serviceNode.GetChildNode(NKEY_DesktopDirectoryShortcut);
	if (dsNode != NULL)
	{
		MkArray<MkStr> uninstallFileList, uninstallDirList;
		__GetUninstallList(*dsNode, MkStr::EMPTY, uninstallFileList, uninstallDirList);

		MK_INDEXING_LOOP(uninstallFileList, i)
		{
			MkShortcutOperator::DeleteShortcutInCommonDesktopDirectory(uninstallFileList[i]);
		}
		MK_INDEXING_LOOP(uninstallDirList, i)
		{
			MkShortcutOperator::DeleteDirectoryInCommonDesktopDirectory(uninstallDirList[i]);
		}
	}

	// shortcut - programs
	const MkDataNode* psNode = serviceNode.GetChildNode(NKEY_ProgramsShortcut);
	if (psNode != NULL)
	{
		MkArray<MkStr> uninstallFileList, uninstallDirList;
		__GetUninstallList(*psNode, MkStr::EMPTY, uninstallFileList, uninstallDirList);

		MK_INDEXING_LOOP(uninstallFileList, i)
		{
			MkShortcutOperator::DeleteShortcutInCommonPrograms(uninstallFileList[i]);
		}
		MK_INDEXING_LOOP(uninstallDirList, i)
		{
			MkShortcutOperator::DeleteDirectoryInCommonPrograms(uninstallDirList[i]);
		}
	}

	// client delete list
	const MkDataNode* deleteNode = serviceNode.GetChildNode(NKEY_ClientDeleteList);
	if (deleteNode != NULL)
	{
		// delete files
		MkArray<MkStr> files;
		deleteNode->GetData(UKEY_Files, files);
		MK_INDEXING_LOOP(files, i)
		{
			MkPathName fullPath;
			fullPath.ConvertToModuleBasisAbsolutePath(files[i]);
			fullPath.DeleteCurrentFile();
		}

		// delete dirs
		MkArray<MkStr> dirs;
		deleteNode->GetData(UKEY_Dirs, dirs);
		MK_INDEXING_LOOP(dirs, i)
		{
			MkPathName fullPath;
			fullPath.ConvertToModuleBasisAbsolutePath(dirs[i]);
			fullPath.DeleteCurrentDirectory();
		}

		// delete chunk
		MkStr chunkPrefix, chunkExtension;
		deleteNode->GetData(UKEY_ChunkPrefix, chunkPrefix, 0);
		deleteNode->GetData(UKEY_ChunkExtension, chunkExtension, 0);

		if ((!chunkPrefix.Empty()) && (!chunkExtension.Empty()))
		{
			MkMap<unsigned int, MkPathName> filePathTable;
			MkPathName::GetModuleDirectory().GetIndexFormFileList(filePathTable, chunkPrefix, chunkExtension, true);

			MkMapLooper<unsigned int, MkPathName> looper(filePathTable);
			MK_STL_LOOP(looper)
			{
				MkPathName fullPath;
				fullPath.ConvertToModuleBasisAbsolutePath(looper.GetCurrentField());
				fullPath.DeleteCurrentFile();
			}
		}
	}

	return true;
}

//------------------------------------------------------------------------------------------------//