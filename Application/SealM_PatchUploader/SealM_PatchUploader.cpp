// SealM_PatchUploader.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <windows.h>
#include <iostream>

#include "MkCore_MkDataNode.h"
#include "MkCore_MkFtpInterface.h"
#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkZipCompressor.h"


int main()
{
	// mk 초기화
    MkStr::SetUp();
    MkPathName::SetUp();

	int uploadingResult = -1;
	MkStr logMsg;

	do
	{
		// 세팅
		MkDataNode settingNode;
		if (!settingNode.Load(L"SealM_PatchUploaderSetting.txt"))
		{
			logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일이 없음";
			break;
		}

		// argv
		MkStr buildType; // ex> assets/apk/exe/chunk/srvlist

		// 그룹(국가). TW, SEA, ...
		MkStr arg2;

		// ex>
		// assets일 경우 Public_Common_Hosting\Android\Group
		// apk일 경우 Public_Common_Hosting
		// chunk일 경우 Public_Common_Hosting
		MkStr arg3;

		// apk일 경우 파일 prefix(SealM_Android_Dev_Debug_)
		MkStr arg4;
		
		const wchar_t* cmdLine = ::GetCommandLine();
		int lineCount = 0;
		LPWSTR* cmdLines = ::CommandLineToArgvW(cmdLine, &lineCount);
		for (int i = 1; i < lineCount; ++i)
		{
			switch (i)
			{
			case 1: buildType = cmdLines[i]; break;
			case 2: arg2 = cmdLines[i]; break;
			case 3: arg3 = cmdLines[i]; break;
			case 4: arg4 = cmdLines[i]; break;
			}
		}

		logMsg += L"buildType : " + buildType + MkStr::LF;
		logMsg += L"arg2 : " + arg2 + MkStr::LF;
		logMsg += L"arg3 : " + arg3 + MkStr::LF;
		if (!arg4.Empty())
		{
			logMsg += L"arg4 : " + arg4 + MkStr::LF;
		}
		logMsg += MkStr::LF;

		buildType.ToLower();
		bool isBundle = buildType.Equals(0, L"assets");
		bool isApk = buildType.Equals(0, L"apk");
		bool isExe = buildType.Equals(0, L"exe");
		bool isChunk = buildType.Equals(0, L"chunk");
		bool isSrvList = buildType.Equals(0, L"srvlist");
		bool isAny = buildType.Equals(0, L"any");

		if ((!isBundle) && (!isApk) && (!isExe) && (!isChunk) && (!isSrvList) && (!isAny))
		{
			logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 buildType 잘못 입력";
			break;
		}

		// setting 노드에서 업로드 정보 받아옴
		if (!settingNode.ChildExist(buildType))
		{
			logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 " + buildType + L" 항목이 없음";
			break;
		}

		const MkDataNode& typeNode = *settingNode.GetChildNode(buildType);
		if (!typeNode.ChildExist(arg2))
		{
			logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 " + arg2 + L" 항목이 없음";
			break;
		}

		const MkDataNode& groupNode = *typeNode.GetChildNode(arg2);
		if (!groupNode.ChildExist(arg3))
		{
			logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일의 " + arg2 + L" 그룹에 " + arg3 + L" 항목이 없음";
			break;
		}

		const MkDataNode& targetNode = *groupNode.GetChildNode(arg3);

		MkStr url, remotePath, userName, password;
		bool passiveMode;
		
		if (isBundle || isApk || isChunk || isAny)
		{
			if (!targetNode.GetData(L"URL", url, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 URL이 없음";
				break;
			}
			if (!targetNode.GetData(L"RemotePath", remotePath, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 RemotePath가 없음";
				break;
			}
			if (!targetNode.GetData(L"UserName", userName, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 UserName이 없음";
				break;
			}
			if (!targetNode.GetData(L"Password", password, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 Password가 없음";
				break;
			}
			if (!targetNode.GetData(L"PassiveMode", passiveMode, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 PassiveMode가 없음";
				break;
			}

			logMsg += L"URL : " + url + MkStr::LF;
			logMsg += L"RemotePath : " + remotePath + MkStr::LF;
			logMsg += L"UserName : " + userName + MkStr::LF;
			logMsg += L"Password : " + password + MkStr::LF;
			logMsg += L"PassiveMode : " + MkStr(passiveMode) + MkStr::LF;
		}

		MkPathName localHomePath;
		if (isBundle || isApk || isExe)
		{
			MkStr tmpPath;
			if (!targetNode.GetData(L"LocalHomePath", tmpPath, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 LocalHomePath가 없음";
				break;
			}

			localHomePath = tmpPath;
			localHomePath.CheckAndAddBackslash();
			logMsg += L"LocalHomePath : " + localHomePath + MkStr::LF;
		}

		MkPathName tempArchivePath;
		tempArchivePath.ConvertToModuleBasisAbsolutePath(L"_Temp");
		tempArchivePath.CheckAndAddBackslash();
		tempArchivePath.MakeDirectoryPath();
		
		// 번들
		if (isBundle)
		{
			MkArray<MkStr> catalogs;
			if (!targetNode.GetData(L"Catalogs", catalogs))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 Catalogs가 없음";
				break;
			}

			logMsg += L"Catalogs" + MkStr::LF;
			MK_INDEXING_LOOP(catalogs, i)
			{
				logMsg += L"   - " + catalogs[i] + MkStr::LF;
			}
			logMsg += MkStr::LF;

			// 빌드 결과 경로
			MkPathName buildResultFullPath = localHomePath + arg3;
			buildResultFullPath.CheckAndAddBackslash(); // ex> D:\ProjectSealM\Bundles\Public_Common_Hosting\Android\Group\

			// 존재여부 확인
			if (!buildResultFullPath.CheckAvailable())
			{
				logMsg += buildResultFullPath;
				logMsg += L" 경로는 존재하지 않음";
				break;
			}

			// 내부상태 탐색
			MkDataNode node;
			buildResultFullPath.ExportSystemStructure(node);

			MkArray<MkHashStr> subDirs;
			MkPathName::__GetSubDirectories(node, subDirs); // 하위 디렉토리만

			unsigned __int64 lastestBuildTime = 0;
			unsigned int lastestIndex;

			MK_INDEXING_LOOP(subDirs, i)
			{
				MkStr dirName = subDirs[i].GetString(); // 디렉토리명
				dirName.RemoveKeyword(L"-");

				unsigned __int64 currBuildTime = dirName.ToDoubleUnsignedInteger();
				if (currBuildTime > lastestBuildTime)
				{
					lastestBuildTime = currBuildTime;
					lastestIndex = i;
				}
			}

			if (lastestBuildTime == 0)
			{
				logMsg += buildResultFullPath;
				logMsg += L" 경로에 빌드된 번들이 없음";
				break;
			}

			// 로컬의 빌드된 번들 원본 경로
			MkPathName bundleSrcPath = buildResultFullPath + subDirs[lastestIndex].GetString();
			bundleSrcPath.CheckAndAddBackslash(); // ex> D:\ProjectSealM\Bundles\Public_Common_Hosting\Android\Group\211203-1430\

			logMsg += L"bundleSrcPath : " + bundleSrcPath + MkStr::LF;
			logMsg += MkStr::LF;

			// ftp 탐색 경로
			// ex> "Public_Common_Hosting", "Android", "Group"
			MkArray<MkStr> targetPathTokens;
			MkArray<MkStr> targetTokensSeperator;
			targetTokensSeperator.PushBack(L"\\");
			targetTokensSeperator.PushBack(L"/");
			arg3.Tokenize(targetPathTokens, targetTokensSeperator);

			// ftp 업로드
			MkFtpInterface ftpInterface;
			if (!ftpInterface.Connect(url, remotePath, userName, password, passiveMode))
			{
				logMsg += L"ftp 접속 실패";
				break;
			}

			unsigned int successCount = 0;
			MK_INDEXING_LOOP(targetPathTokens, i)
			{
				if (!ftpInterface.MoveToChild(targetPathTokens[i], true))
					break;

				++successCount;
			}

			if (successCount < targetPathTokens.GetSize())
			{
				logMsg += L"ftp 탐색 실패";
				break;
			}

			// catalog 파일들 업로드
			successCount = 0;
			MK_INDEXING_LOOP(catalogs, i)
			{
				//MkPathName catalogFilePath = bundleSrcPath + catalogs[i];
				MkPathName catalogFilePath = buildResultFullPath + catalogs[i];
				if (!ftpInterface.UploadFile(catalogFilePath))
					break;

				++successCount;
			}

			if (successCount < catalogs.GetSize())
			{
				logMsg += L"카탈로그 업로드 실패";
				break;
			}

			// 번들 업로드
			if (!ftpInterface.MoveToChild(subDirs[lastestIndex].GetString(), true)) // ex> "211203-1430"
			{
				logMsg += L"ftp 탐색 실패";
				break;
			}

			if (!ftpInterface.UploadDirectory(bundleSrcPath))
			{
				logMsg += L"번들 업로드 실패";
				break;
			}

			uploadingResult = 0;
			logMsg += L"번들 업로드 성공!!!";
		}
		// apk/aab
		else if (isApk)
		{
			logMsg += MkStr::LF;

			// 빌드 결과 경로
			MkPathName buildResultFullPath = localHomePath + arg3;
			buildResultFullPath.CheckAndAddBackslash(); // ex> F:\WorkResult\SealM_Android_Hosting_Resource\Public_Common_Hosting\

			// 존재여부 확인
			if (!buildResultFullPath.CheckAvailable())
			{
				logMsg += buildResultFullPath;
				logMsg += L" 경로는 존재하지 않음";
				break;
			}

			// 내부상태 탐색
			MkDataNode node;
			buildResultFullPath.ExportSystemStructure(node);

			MkArray<MkHashStr> subFiles;
			MkPathName::__GetSubFiles(node, subFiles); // 하위 파일만

			unsigned __int64 lastestBuildTime = 0;
			MkArray<unsigned int> lastestIndice;

			arg4.ToLower();

			MK_INDEXING_LOOP(subFiles, i)
			{
				MkPathName fileNamePath = subFiles[i].GetString(); // ex> SealM_Android_Dev_Public_Debug_211203_1424.apk
				fileNamePath.ToLower();
				if (fileNamePath.CheckPrefix(arg4))
				{
					MkStr timePart;
					fileNamePath.GetSubStr(MkArraySection(arg4.GetSize(), 11), timePart); // 시간 부분만 추출. ex> 211203_1424
					timePart.RemoveKeyword(L"_"); // 숫자파트만 남김. ex> 2112031424

					unsigned __int64 currBuildTime = timePart.ToDoubleUnsignedInteger();
					if (currBuildTime > lastestBuildTime)
					{
						lastestBuildTime = currBuildTime;
						lastestIndice.Flush();
						lastestIndice.PushBack(i);
					}
					else if (currBuildTime == lastestBuildTime)
					{
						lastestIndice.PushBack(i);
					}
				}
			}

			if (lastestBuildTime == 0)
			{
				logMsg += buildResultFullPath;
				logMsg += L" 경로에 빌드된 apk/aab가 없음";
				break;
			}

			// 로컬의 빌드된 apk 원본 경로
			MkArray<MkPathName> apkSrcPath(lastestIndice.GetSize());
			MK_INDEXING_LOOP(lastestIndice, i)
			{
				MkPathName tmpPath = buildResultFullPath + subFiles[lastestIndice[i]].GetString();
				if (tmpPath.GetFileSize() < 1024 * 1024 * 1024) // 1G 이하만 허용
				{
					apkSrcPath.PushBack(tmpPath);
					logMsg += L"apkSrcPath : " + tmpPath + MkStr::LF;
				}
			}
			logMsg += MkStr::LF;

			// ftp 업로드
			MkFtpInterface ftpInterface;
			if (!ftpInterface.Connect(url, remotePath, userName, password, passiveMode))
			{
				logMsg += L"ftp 접속 실패";
				break;
			}

			unsigned int successCount = 0;
			MK_INDEXING_LOOP(apkSrcPath, i)
			{
				if (!ftpInterface.UploadFile(apkSrcPath[i]))
					break;

				++successCount;
			}

			if (successCount == apkSrcPath.GetSize())
			{
				uploadingResult = 0;
				logMsg += L"apk 업로드 성공!!!" + MkStr::LF;
				MK_INDEXING_LOOP(lastestIndice, i)
				{
					logMsg += L"http://intpatch-sealm.playwith.co.kr/apk/" + subFiles[lastestIndice[i]].GetString() + MkStr::LF;
				}
			}
			else
			{
				logMsg += L"apk 업로드 실패" + MkStr::LF;
			}
		}
		// windows exe
		// 클라이언트 구성만 하고 배포(패치, 업로드)는 "SimplePatchFileGenerator.exe"에게 맞김
		else if (isExe)
		{
			logMsg += MkStr::LF;

			// 빌드 결과 경로
			MkPathName buildResultFullPath = localHomePath + arg3;
			buildResultFullPath.CheckAndAddBackslash(); // ex> F:\WorkResult\SealM_Windows_Hosting_Resource\Public_Common_Hosting\

			// 존재여부 확인
			if (!buildResultFullPath.CheckAvailable())
			{
				logMsg += buildResultFullPath;
				logMsg += L" 경로는 존재하지 않음";
				break;
			}

			// 내부상태 탐색
			MkDataNode node;
			buildResultFullPath.ExportSystemStructure(node);

			MkArray<MkHashStr> subDirs;
			MkPathName::__GetSubDirectories(node, subDirs); // 하위 디렉토리만

			unsigned __int64 lastestBuildTime = 0;
			unsigned int lastestIndex;

			MK_INDEXING_LOOP(subDirs, i)
			{
				MkStr dirName = subDirs[i].GetString(); // 디렉토리명
				dirName.RemoveKeyword(L"_");

				unsigned __int64 currBuildTime = dirName.ToDoubleUnsignedInteger();
				if (currBuildTime > lastestBuildTime)
				{
					lastestBuildTime = currBuildTime;
					lastestIndex = i;
				}
			}

			if (lastestBuildTime == 0)
			{
				logMsg += buildResultFullPath;
				logMsg += L" 경로에 빌드된 클라이언트가 없음";
				break;
			}

			// 로컬의 빌드된 클라이언트 원본 경로
			MkPathName clientSrcPath = buildResultFullPath + subDirs[lastestIndex].GetString();
			clientSrcPath.CheckAndAddBackslash(); // ex> F:\WorkResult\SealM_Windows_Hosting_Resource\Public_Common_Hosting\211224_1316\

			logMsg += L"clientSrcPath : " + clientSrcPath + MkStr::LF;
			
			// 복사 할 패쳐의 클라이언트 경로
			MkStr clientPath;
			if (!targetNode.GetData(L"ClientPath", clientPath, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 ClientPath가 없음";
				break;
			}

			MkPathName clientTargetPath = clientPath;
			clientTargetPath.CheckAndAddBackslash();
			clientTargetPath.MakeDirectoryPath();

			logMsg += L"clientTargetPath : " + clientTargetPath + MkStr::LF;
			logMsg += MkStr::LF;

			// src에서 target으로 모든 파일 복사
			MkArray<MkPathName> allFiles;
			clientSrcPath.GetFileList(allFiles);
			bool copyFailed = false;
			MK_INDEXING_LOOP(allFiles, i)
			{
				MkPathName srcFullPath = clientSrcPath + allFiles[i];
				MkPathName targetFullPath = clientTargetPath + allFiles[i];
				if (srcFullPath.CopyCurrentFile(targetFullPath, false))
				{
					logMsg += L"  (O) " + allFiles[i] + MkStr::LF;
				}
				else
				{
					logMsg += L"  (X) " + allFiles[i] + MkStr::LF;

					copyFailed |= true;
				}
			}

			logMsg += MkStr::LF;
			if (copyFailed)
			{
				logMsg += L"복사 실패";
			}
			else
			{
				logMsg += L"복사 성공";
				uploadingResult = 0;
			}
			logMsg += MkStr::LF;
		}
		// data table(.bytes)
		else if (isChunk)
		{
			logMsg += MkStr::LF;

			MkStr srcResPath;
			if (!targetNode.GetData(L"SrcResPath", srcResPath, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 SrcResPath가 없음";
				break;
			}

			MkArray<MkStr> chunkFiles;
			if (!targetNode.GetData(L"ChunkFiles", chunkFiles))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 ChunkFiles가 없음";
				break;
			}

			if (chunkFiles.Empty())
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일의 ChunkFiles에 기준 파일이 없음";
				break;
			}

			MkStr infoFileName;
			if (!targetNode.GetData(L"InfoFileName", infoFileName, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 InfoFileName가 없음";
				break;
			}

			MkPathName baseResPath = srcResPath;
			baseResPath.CheckAndAddBackslash();

			MkDataNode infoNode;
			MkDataNode& patchInfoNode = *infoNode.CreateChildNode(L"PatchInfo");

			// 테이블 버전 정보를 얻음
			{
				MkPathName targetFilePath = baseResPath + chunkFiles[0];
				if (!targetFilePath.CheckAvailable())
				{
					logMsg += L"테이블 파일이 없음 : " + targetFilePath;
					break;
				}

				MkByteArray fileData;
				MkInterfaceForFileReading frInterface;
				if (!frInterface.SetUp(targetFilePath))
					return false;

				frInterface.Read(fileData, MkArraySection(0));
				frInterface.Clear();

				if (fileData.GetSize() < sizeof(int))
				{
					logMsg += L"정상적인 테이블 파일이 아님 : " + targetFilePath;
					break;
				}

				int versionLength = 0;
				memcpy_s(reinterpret_cast<unsigned char*>(&versionLength), sizeof(int), fileData.GetPtr(), sizeof(int));
				//
				unsigned char* versionBytes = new unsigned char[versionLength];
				memcpy_s(versionBytes, sizeof(unsigned char) * versionLength, &fileData.GetPtr()[sizeof(int)], sizeof(unsigned char)* versionLength);

				MkStr version = MkByteArray(versionBytes, sizeof(unsigned char) * versionLength);
				MkArray<MkStr> tokens;
				version.Tokenize(tokens, L".");
				if (tokens.GetSize() != 2)
				{
					logMsg += L"정상적인 테이블 파일이 아님 : " + targetFilePath;
					break;
				}

				MkArray<int> verData;
				verData.PushBack(tokens[0].ToInteger());
				verData.PushBack(tokens[1].ToInteger());

				MkDataNode& targetFileNode = *patchInfoNode.CreateChildNode(chunkFiles[0]);
				if (!targetFileNode.CreateUnit(L"Version", verData))
				{
					logMsg += L"버전 정보 기록 실패";
					break;
				}

				logMsg += L"테이블 파일 버전(" + chunkFiles[0] + L") : " + version + MkStr::LF;
				logMsg += MkStr::LF;
			}

			// 언어 버전 정보를 얻음
			MK_INDEXING_LOOP(chunkFiles, i)
			{
				if (i == 0)
					continue;
				
				MkPathName targetFilePath = baseResPath + chunkFiles[i];
				if (!targetFilePath.CheckAvailable())
				{
					logMsg += L"언어 파일이 없음 : " + targetFilePath;
					break;
				}

				MkByteArray fileData;
				MkInterfaceForFileReading frInterface;
				if (!frInterface.SetUp(targetFilePath))
					return false;

				frInterface.Read(fileData, MkArraySection(0));
				frInterface.Clear();

				BYTE headerSize = 0;
				memcpy_s(&headerSize, sizeof(BYTE), fileData.GetPtr() + 4, sizeof(BYTE));
				if (headerSize == 0)
				{
					logMsg += L"정상적인 언어 파일이 아님 : " + targetFilePath;
					continue;
				}

				BYTE* headerBuffer = new BYTE[headerSize + 1];
				headerBuffer[headerSize] = NULL;
				memcpy_s(headerBuffer, sizeof(BYTE) * headerSize, fileData.GetPtr() + 5, sizeof(BYTE) * headerSize);

				MkStr headerStr;
				headerStr.ImportMultiByteString(std::string(reinterpret_cast<char*>(headerBuffer)));
				delete[] headerBuffer;

				MkStr version;
				if (headerStr.GetFirstBlock(0, L"\"Version\":\"", L"\"", version) == MKDEF_ARRAY_ERROR)
				{
					logMsg += L"정상적인 언어 파일이 아님 : " + targetFilePath;
					continue;
				}

				MkArray<MkStr> tokens;
				version.Tokenize(tokens, L".");
				if (tokens.GetSize() != 2)
				{
					logMsg += L"정상적인 언어 파일이 아님 : " + targetFilePath;
					continue;
				}

				MkArray<int> verData;
				verData.PushBack(tokens[0].ToInteger());
				verData.PushBack(tokens[1].ToInteger());

				MkDataNode& targetFileNode = *patchInfoNode.CreateChildNode(chunkFiles[i]);
				if (!targetFileNode.CreateUnit(L"Version", verData))
				{
					logMsg += L"버전 정보 기록 실패";
					continue;
				}

				logMsg += L"언어 파일 버전(" + chunkFiles[i] + L") : " + version + MkStr::LF;
				logMsg += MkStr::LF;
			}

			// 원본 파일의 md5를 얻어 정보 파일을 생성
			MkArray<MkPathName> targetSrcFiles(chunkFiles.GetSize());
			MkArray<MkPathName> targetCompFiles(chunkFiles.GetSize());
			MK_INDEXING_LOOP(chunkFiles, i)
			{
				MkPathName targetFilePath = baseResPath + chunkFiles[i];
				MkDataNode* targetFileNode = patchInfoNode.GetChildNode(chunkFiles[i]);
				if (targetFilePath.CheckAvailable() && (targetFileNode != NULL))
				{
					MkByteArray md5Value;
					if (targetFilePath.GetMD5Value(md5Value) && // md5 값을 얻고
						targetFileNode->CreateUnit(L"MD5", md5Value)) // 노드에 생성
					{
						targetSrcFiles.PushBack(targetFilePath);

						targetCompFiles.PushBack(tempArchivePath + chunkFiles[i]);

						logMsg += L"MD5 추출 성공 : " + targetFilePath + MkStr::LF;
					}
				}
			}
			logMsg += MkStr::LF;

			MkPathName infoFilePath = tempArchivePath + infoFileName;
			if (infoNode.SaveToBinary(infoFilePath))
			{
				logMsg += L"md5 정보 파일 생성 성공 : " + infoFilePath + MkStr::LF;
				logMsg += MkStr::LF;
			}
			else
			{
				logMsg += L"md5 정보 파일 생성 실패 : " + infoFilePath + MkStr::LF;
				break;
			}

			MK_INDEXING_LOOP(targetSrcFiles, i)
			{
				// 압축할 원본 파일 읽음
				MkByteArray origFileData;
				MkInterfaceForFileReading frInterface;
				if (frInterface.SetUp(targetSrcFiles[i]))
				{
					frInterface.Read(origFileData, MkArraySection(0));
					frInterface.Clear();
				}

				// 압축
				MkByteArray compFileData;
				MkZipCompressor::Compress(origFileData.GetPtr(), origFileData.GetSize(), compFileData);
				unsigned int origSize = origFileData.GetSize();
				origFileData.Clear();

				// 저장
				MkInterfaceForFileWriting fwInterface;
				if (fwInterface.SetUp(targetCompFiles[i], true, true))
				{
					fwInterface.Write(compFileData, MkArraySection(0));
					fwInterface.Clear();
				}

				logMsg += L"압축해 저장 성공 : " + targetCompFiles[i] + L"(" + MkStr(origSize) + L" -> " + MkStr(compFileData.GetSize()) + L")" + MkStr::LF;
			}
			logMsg += MkStr::LF;

			// 결과를 올릴 위치
			MkStr destDirPath;
			targetNode.GetData(L"DestDirPath", destDirPath, 0);

			// ftp 업로드
			if (destDirPath.Empty())
			{
				logMsg += L"청크 ftp 업로드 시도" + MkStr::LF;

				MkFtpInterface ftpInterface;
				if (!ftpInterface.Connect(url, remotePath, userName, password, passiveMode))
				{
					logMsg += L"ftp 접속 실패";
					break;
				}

				if (!ftpInterface.MoveToChild(arg3, true))
				{
					logMsg += L"ftp 탐색 실패";
					break;
				}

				// chunk 파일들 업로드
				int successCount = 0;
				MK_INDEXING_LOOP(targetCompFiles, i)
				{
					if (!ftpInterface.UploadFile(targetCompFiles[i]))
						break;

					++successCount;
				}

				if (successCount < targetCompFiles.GetSize())
				{
					logMsg += L"청크 업로드 실패";
					break;
				}

				if (!ftpInterface.UploadFile(infoFilePath))
				{
					logMsg += L"정보 파일 업로드 실패";
					break;
				}

				uploadingResult = 0;
				logMsg += L"청크 업로드 성공!!!" + MkStr::LF;
			}
			else
			{
				MkPathName destPath = destDirPath;
				destPath.CheckAndAddBackslash();

				if (destPath.CheckAvailable())
				{
					destPath.DeleteCurrentDirectory();
				}
				destPath.MakeDirectoryPath();

				logMsg += L"청크 복사 시도 >> " + destPath + MkStr::LF;

				int successCount = 0;
				MK_INDEXING_LOOP(targetCompFiles, i)
				{
					if (!targetCompFiles[i].CopyCurrentFile(destPath + targetCompFiles[i].GetFileName(), false))
						break;

					++successCount;
				}

				if (successCount < targetCompFiles.GetSize())
				{
					logMsg += L"청크 복사 실패";
					break;
				}

				if (!infoFilePath.CopyCurrentFile(destPath + infoFilePath.GetFileName(), false))
				{
					logMsg += L"정보 파일 복사 실패";
					break;
				}

				uploadingResult = 0;
				logMsg += L"청크 복사 성공!!!" + MkStr::LF;
			}
		}
		// 서버 리스트 파일
		else if (isSrvList)
		{
			logMsg += MkStr::LF;

			MkStr listFilePath;
			if (!targetNode.GetData(L"ListFilePath", listFilePath, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 ListFilePath가 없음";
				break;
			}
			logMsg += L"ListFilePath : " + listFilePath + MkStr::LF;

			MkStr destDirPath;
			if (!targetNode.GetData(L"DestDirPath", destDirPath, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 DestDirPath가 없음";
				break;
			}
			logMsg += L"DestDirPath : " + destDirPath + MkStr::LF;

			// 리스트 파일에 버전 정보를 삽입
			MkPathName targetFilePath = listFilePath;
			if (!targetFilePath.CheckAvailable())
			{
				logMsg += L"리스트 파일이 없음 : " + targetFilePath;
				break;
			}

			MkStr fileStr;
			if (!fileStr.ReadTextFile(targetFilePath, true, false)) // UTF-8
			{
				logMsg += L"리스트 파일 읽기 실패 : " + targetFilePath;
				break;
			}
			logMsg += L"리스트 파일 읽기 : " + targetFilePath + MkStr::LF;

			const MkStr VERSION_KEY = L"\"FileVersion\"";
			unsigned int keyPos = fileStr.GetFirstKeywordPosition(VERSION_KEY);

			MkStr dateStr = MkSystemEnvironment::GetCurrentSystemDate(); // 년.월.일
			dateStr.RemoveKeyword(L".");
			dateStr.PopFront(2); // L"20"
			MkStr timeStr = MkSystemEnvironment::GetCurrentSystemTime(); // 시:분:초
			timeStr.RemoveKeyword(L":");
			timeStr.BackSpace(2); // 초단위 제거
			MkStr timeStamp = dateStr + timeStr; // ex> 2112271729
			logMsg += L"timeStamp : " + timeStamp + MkStr::LF;

			// 새로 삽입
			if (keyPos == MKDEF_ARRAY_ERROR)
			{
				unsigned int insertPos = fileStr.GetFirstKeywordPosition(L"\"loginServerList\"");
				if (insertPos == MKDEF_ARRAY_ERROR)
				{
					logMsg += L"정상적인 서버 리스트 파일이 아님 : " + targetFilePath;
					break;
				}

				MkStr tmpStr = VERSION_KEY + L": " + timeStamp + L"," + MkStr::CRLF + MkStr::CRLF + MkStr::TAB;
				fileStr.Insert(insertPos, tmpStr);

				logMsg += L"삽입 : " + timeStamp + MkStr::LF;
			}
			// 버전 변경
			else
			{
				unsigned int currPos = fileStr.GetFirstKeywordPosition(MkArraySection(keyPos + VERSION_KEY.GetSize()), L":");
				currPos = fileStr.GetFirstValidPosition(currPos + 1);

				for (unsigned int i = 0; i < 10; ++i)
				{
					fileStr[currPos + i] = timeStamp[i];
				}

				logMsg += L"갱신 : " + timeStamp + MkStr::LF;
			}

			// 변경 된 파일을 임시로 저장
			MkStr listFileName = targetFilePath.GetFileName();
			MkPathName tmpTextFileName = L"t_" + listFileName;
			MkPathName tmpTextFilePath = tempArchivePath + tmpTextFileName;
			if (!fileStr.WriteToTextFile(tmpTextFilePath, true, false)) // UTF-8
			{
				logMsg += L"파일 저장 실패 : " + tmpTextFilePath;
				break;
			}
			logMsg += L"임시 파일 저장 : " + tmpTextFilePath + MkStr::LF;

			// 바이너리로 열어
			MkByteArray origFileData;
			MkInterfaceForFileReading frInterface;
			if (!frInterface.SetUp(tmpTextFilePath))
				return false;

			frInterface.Read(origFileData, MkArraySection(0));
			frInterface.Clear();

			// 압축
			MkByteArray compFileData;
			MkZipCompressor::Compress(origFileData.GetPtr(), origFileData.GetSize(), compFileData);
			unsigned int origSize = origFileData.GetSize();
			origFileData.Clear();

			// 저장
			MkPathName targetCompFilePath = tempArchivePath + listFileName;

			MkInterfaceForFileWriting fwInterface;
			if (fwInterface.SetUp(targetCompFilePath, true, true))
			{
				fwInterface.Write(compFileData, MkArraySection(0));
				fwInterface.Clear();
			}
			logMsg += L"압축해 저장 성공 : " + targetCompFilePath + L"(" + MkStr(origSize) + L" -> " + MkStr(compFileData.GetSize()) + L")" + MkStr::LF;

			MkPathName destPath = destDirPath;
			destPath.CheckAndAddBackslash();

			if (destPath.CheckAvailable())
			{
				destPath.DeleteCurrentDirectory();
			}
			destPath.MakeDirectoryPath();

			logMsg += L"청크 복사 시도 >> " + destPath + MkStr::LF;

			if (!targetCompFilePath.CopyCurrentFile(destPath + listFileName, false))
			{
				logMsg += L"청크 복사 실패";
				break;
			}

			uploadingResult = 0;
			logMsg += L"청크 복사 성공!!!" + MkStr::LF;
		}
		// any files
		else if (isAny)
		{
			logMsg += MkStr::LF;

			MkStr srcResPath;
			if (!targetNode.GetData(L"SrcResPath", srcResPath, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 SrcResPath가 없음";
				break;
			}

			MkArray<MkStr> preLoadFiles, bundleFiles;
			targetNode.GetData(L"PreLoadFiles", preLoadFiles);
			targetNode.GetData(L"BundleFiles", bundleFiles);

			MkStr infoFileName;
			if (!targetNode.GetData(L"InfoFileName", infoFileName, 0))
			{
				logMsg += L"SealM_PatchUploaderSetting.txt 설정 파일에 InfoFileName가 없음";
				break;
			}

			MkPathName baseResPath = srcResPath;
			baseResPath.CheckAndAddBackslash();

			MkDataNode infoNode;

			// 원본 파일의 파일 크기로 정보 파일을 생성
			MkArray<MkPathName> targetSrcFiles(preLoadFiles.GetSize() + bundleFiles.GetSize());
			//MkArray<MkPathName> targetCompFiles(preLoadFiles.GetSize() + bundleFiles.GetSize());

			if (!preLoadFiles.Empty())
			{
				MK_INDEXING_LOOP(preLoadFiles, i)
				{
					MkPathName targetFilePath = baseResPath + preLoadFiles[i];
					if (targetFilePath.CheckAvailable())
					{
						MkDataNode* destNode = infoNode.ChildExist(L"PreLoad") ? infoNode.GetChildNode(L"PreLoad") : infoNode.CreateChildNode(L"PreLoad");
						if (destNode->CreateUnit(preLoadFiles[i], targetFilePath.GetFileSize())) // 노드에 생성
						{
							targetSrcFiles.PushBack(targetFilePath);
							//targetCompFiles.PushBack(tempArchivePath + preLoadFiles[i]);

							logMsg += L"Any(PreLoad) : " + targetFilePath + MkStr::LF;
						}
					}
				}
				logMsg += MkStr::LF;
			}

			if (!bundleFiles.Empty())
			{
				MK_INDEXING_LOOP(bundleFiles, i)
				{
					MkPathName targetFilePath = baseResPath + bundleFiles[i];
					if (targetFilePath.CheckAvailable())
					{
						MkDataNode* destNode = infoNode.ChildExist(L"Bundle") ? infoNode.GetChildNode(L"Bundle") : infoNode.CreateChildNode(L"Bundle");
						if (destNode->CreateUnit(bundleFiles[i], targetFilePath.GetFileSize())) // 노드에 생성
						{
							targetSrcFiles.PushBack(targetFilePath);
							//targetCompFiles.PushBack(tempArchivePath + bundleFiles[i]);

							logMsg += L"Any(Bundle) : " + targetFilePath + MkStr::LF;
						}
					}
				}
				logMsg += MkStr::LF;
			}
			
			MkPathName infoFilePath = tempArchivePath + infoFileName;
			if (infoNode.SaveToBinary(infoFilePath))
			{
				logMsg += L"any 정보 파일 생성 성공 : " + infoFilePath + MkStr::LF;
				logMsg += MkStr::LF;
			}
			else
			{
				logMsg += L"any 정보 파일 생성 실패 : " + infoFilePath + MkStr::LF;
				break;
			}

			MK_INDEXING_LOOP(targetSrcFiles, i)
			{
				/*
				// 압축할 원본 파일 읽음
				MkByteArray origFileData;
				MkInterfaceForFileReading frInterface;
				if (frInterface.SetUp(targetSrcFiles[i]))
				{
					frInterface.Read(origFileData, MkArraySection(0));
					frInterface.Clear();
				}

				// 압축
				MkByteArray compFileData;
				MkZipCompressor::Compress(origFileData.GetPtr(), origFileData.GetSize(), compFileData);
				unsigned int origSize = origFileData.GetSize();
				origFileData.Clear();

				// 저장
				MkInterfaceForFileWriting fwInterface;
				if (fwInterface.SetUp(targetCompFiles[i], true, true))
				{
					fwInterface.Write(compFileData, MkArraySection(0));
					fwInterface.Clear();
				}

				logMsg += L"압축해 저장 성공 : " + targetCompFiles[i] + L"(" + MkStr(origSize) + L" -> " + MkStr(compFileData.GetSize()) + L")" + MkStr::LF;
				*/
			}
			logMsg += MkStr::LF;

			// 결과를 올릴 위치
			MkStr destDirPath;
			targetNode.GetData(L"DestDirPath", destDirPath, 0);

			// ftp 업로드
			if (destDirPath.Empty())
			{
				MkFtpInterface ftpInterface;
				if (!ftpInterface.Connect(url, remotePath, userName, password, passiveMode))
				{
					logMsg += L"ftp 접속 실패";
					break;
				}

				if (!ftpInterface.MoveToChild(arg3, true))
				{
					logMsg += L"ftp 탐색 실패";
					break;
				}

				// any 파일들 업로드
				int successCount = 0;
				//MK_INDEXING_LOOP(targetCompFiles, i)
				MK_INDEXING_LOOP(targetSrcFiles, i)
				{
					//if (!ftpInterface.UploadFile(targetCompFiles[i]))
					if (!ftpInterface.UploadFile(targetSrcFiles[i])) // 현재로서는 동영상뿐이라 압축하지 않고 그대로 올림. decoding 비용이 뼈아프다;;
						break;

					++successCount;
				}

				//if (successCount < targetCompFiles.GetSize())
				if (successCount < targetSrcFiles.GetSize())
				{
					logMsg += L"any 업로드 실패";
					break;
				}

				if (!ftpInterface.UploadFile(infoFilePath))
				{
					logMsg += L"any 파일 업로드 실패";
					break;
				}

				uploadingResult = 0;
				logMsg += L"any 업로드 성공!!!" + MkStr::LF;
			}
			else
			{
				MkPathName destPath = destDirPath;
				destPath.CheckAndAddBackslash();

				if (destPath.CheckAvailable())
				{
					destPath.DeleteCurrentDirectory();
				}
				destPath.MakeDirectoryPath();

				logMsg += L"any 복사 시도 >> " + destPath + MkStr::LF;

				// any 파일들 업로드
				int successCount = 0;
				//MK_INDEXING_LOOP(targetCompFiles, i)
				MK_INDEXING_LOOP(targetSrcFiles, i)
				{
					if (!targetSrcFiles[i].CopyCurrentFile(destPath + targetSrcFiles[i].GetFileName(), false))
					//if (!ftpInterface.UploadFile(targetCompFiles[i]))
					//if (!ftpInterface.UploadFile(targetSrcFiles[i])) // 현재로서는 동영상뿐이라 압축하지 않고 그대로 올림. decoding 비용이 뼈아프다;;
						break;

					++successCount;
				}

				//if (successCount < targetCompFiles.GetSize())
				if (successCount < targetSrcFiles.GetSize())
				{
					logMsg += L"any 복사 실패";
					break;
				}

				if (!infoFilePath.CopyCurrentFile(destPath + infoFilePath.GetFileName(), false))
				//if (!ftpInterface.UploadFile(infoFilePath))
				{
					logMsg += L"any 파일 복사 실패";
					break;
				}

				uploadingResult = 0;
				logMsg += L"any 복사 성공!!!" + MkStr::LF;
			}
		}
	}
	while (false);

	if (!logMsg.Empty())
	{
		logMsg += MkStr::LF;
		wprintf_s(logMsg);

		//std::wcout << logMsg.GetPtr() << std::endl;
	}

	return uploadingResult;
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
