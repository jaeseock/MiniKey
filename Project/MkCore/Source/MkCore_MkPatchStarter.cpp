
#include "MkCore_MkCmdLine.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkZipCompressor.h"
#include "MkCore_MkFileDownloader.h"
#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkPatchStarter.h"

//------------------------------------------------------------------------------------------------//

bool MkPatchStarter::StartLauncher(const MkStr& url, const wchar_t* arg, bool updateLauncher)
{
	MkStr rootURL = url;
	if ((!rootURL.Empty()) && (!rootURL.CheckPostfix(L"/")))
	{
		rootURL += L"/"; // ex> http://210.207.252.151/Test -> http://210.207.252.151/Test/
	}

	MkStr infoFileName = MkPatchFileGenerator::LauncherFileName.GetString() + L"." + MkDataNode::DefaultFileExtension; // ex> LauncherFile.mmd
	MkPathName infoFileDown;
	MkPatchFileGenerator::ConvertFilePathToDownloadable(infoFileName, infoFileDown); // ex> LauncherFile.mmd.rp
	MkStr infoFileURL = rootURL + infoFileDown; // http://210.207.252.151/Test/LauncherFile.mmd.rp

	MkPathName localTempInfoFilePath = MkPathName::GetRootDirectory() + infoFileName; // ex> D:\\Client\\LauncherFile.mmd
	MkPathName localLauncherFilePath;

	MkStr errorMsg;
	do
	{
		// 정보 파일 다운
		//if (MkFileDownloader::DownloadFileRightNow(infoFileURL, localTempInfoFilePath) != MkFileDownloader::eSuccess)
		if (!MkFileDownloader::DownloadFileRightNow(infoFileURL, localTempInfoFilePath))
		{
			errorMsg = infoFileURL + L"\n파일 다운로드를 실패했습니다.";
			break;
		}

		// 정보 노드 로드
		MkDataNode launcherNode;
		if (!launcherNode.Load(localTempInfoFilePath))
		{
			errorMsg = localTempInfoFilePath + L"\n파일은 정상적인 data node 형식이 아닙니다.";
			break;
		}

		// 런쳐 파일 경로 생성
		MkStr launcherFileName;
		if (!launcherNode.GetData(MkPatchFileGenerator::LauncherFileName, launcherFileName, 0)) // ex> SimpleLauncher.exe
		{
			errorMsg = localTempInfoFilePath + L"\n파일에 " + MkPatchFileGenerator::LauncherFileName.GetString() + L" unit이 없습니다.";
			break;
		}

		if (launcherFileName.Empty())
		{
			errorMsg = localTempInfoFilePath + L"\n파일의 " + MkPatchFileGenerator::LauncherFileName.GetString() + L" unit이 비어 있습니다.";
			break;
		}

		localLauncherFilePath = MkPathName::GetRootDirectory() + launcherFileName; // ex> D:\\Client\\SimpleLauncher.exe
		if (localLauncherFilePath.IsDirectoryPath())
		{
			errorMsg = localTempInfoFilePath + L"\n파일의 " + MkPatchFileGenerator::LauncherFileName.GetString() + L" unit은 정상적인 파일명이 아닙니다.";
			break;
		}

		// 정보 노드에서 size, written time 읽기 시도. 없으면 패치 정보가 없음을 의미
		unsigned int origSize = 0, origWrittenTime = 0;
		if (launcherNode.GetData(MkPathName::KeyFileSize, origSize, 0) &&
			launcherNode.GetData(MkPathName::KeyWrittenTime, origWrittenTime, 0))
		{
			bool downloadLauncher = true;

			// 실제 파일에서 size, written time 읽어들여 비교
			if (localLauncherFilePath.CheckAvailable())
			{
				unsigned int currFileSize = localLauncherFilePath.GetFileSize();
				unsigned int currFileWrittenTime = localLauncherFilePath.GetWrittenTime();

				downloadLauncher = ((origSize != currFileSize) || (origWrittenTime != currFileWrittenTime));
			}

			// 런쳐 다운
			if (updateLauncher && downloadLauncher)
			{
				// url
				MkPathName launcherFileDown;
				MkPatchFileGenerator::ConvertFilePathToDownloadable(launcherFileName, launcherFileDown); // ex> SimpleLauncher.exe.rp
				MkStr srcURL = rootURL + MkPatchFileGenerator::PatchDataDirName; // http://210.207.252.151/Test/data\\'
				srcURL.BackSpace(1);
				srcURL += L"/"; // ex> http://210.207.252.151/Test/data/
				srcURL += launcherFileDown; // ex> http://210.207.252.151/Test/data/SimpleLauncher.exe.rp

				// local
				MkPathName tmpLauncherFilePath = MkPathName::GetRootDirectory() + launcherFileDown; // ex> D:\\Client\\SimpleLauncher.exe.rp

				// download
				if (!MkFileDownloader::DownloadFileRightNow(srcURL, tmpLauncherFilePath))
				//if (MkFileDownloader::DownloadFileRightNow(srcURL, tmpLauncherFilePath) != MkFileDownloader::eSuccess)
				{
					errorMsg = srcURL + L"\n파일 다운로드를 실패했습니다.";
					break;
				}

				// 압축 상태면 압축 해제
				unsigned int compSize = 0;
				if (launcherNode.GetData(MkPathName::KeyFileSize, compSize, 1))
				{
					// 압축 파일 읽어들여
					MkByteArray compData;
					MkInterfaceForFileReading frInterface;
					if (!frInterface.SetUp(tmpLauncherFilePath))
					{
						errorMsg = tmpLauncherFilePath + L"\n파일 열기 실패.";
						break;
					}
					if (frInterface.Read(compData, MkArraySection(0)) != compSize)
					{
						errorMsg = tmpLauncherFilePath + L"\n읽기 오류거나 다운된 압축 파일 크기가 다릅니다.";
						break;
					}
					frInterface.Clear();

					// 압축 해제
					MkByteArray origData;
					if (MkZipCompressor::Uncompress(compData.GetPtr(), compData.GetSize(), origData) != origSize)
					{
						errorMsg = tmpLauncherFilePath + L"\n압축 해제 후 크기가 다릅니다";
						break;
					}
					compData.Clear();

					// 동일 이름 파일로 덮어 씌움
					MkInterfaceForFileWriting fwInterface;
					if (!fwInterface.SetUp(tmpLauncherFilePath, true, true))
					{
						errorMsg = tmpLauncherFilePath + L"\n파일 생성 실패.";
						break;
					}
					if (fwInterface.Write(origData, MkArraySection(0)) != origSize)
					{
						errorMsg = tmpLauncherFilePath + L"\n덮어쓰기 오류.";
						break;
					}
					fwInterface.Clear();
					origData.Clear();
				}
				else
				{
					// 크기만 검사
					if (tmpLauncherFilePath.GetFileSize() != origSize)
					{
						errorMsg = tmpLauncherFilePath + L"\n다운된 원본 파일 크기가 다릅니다. 맞는 크기 : " + MkStr(origSize);
						break;
					}
				}

				// 현재 런쳐 파일을 삭제하고 받은 임시 파일의 이름을 변경
				localLauncherFilePath.DeleteCurrentFile();
				tmpLauncherFilePath.MoveCurrentFile(localLauncherFilePath);

				// 파일 수정시간 변경
				// 실패하더라도 실행에는 이상이 없으므로 그대로진행
				// 단 다음 패치시 다시 다운로드 시도
				localLauncherFilePath.SetWrittenTime(origWrittenTime);
			}
		}
	}
	while (false);

	// 정보 파일 삭제
	localTempInfoFilePath.DeleteCurrentFile();

	// 에러 메세지가 없으면 실행
	bool ok = errorMsg.Empty();
	if (ok)
	{
		// url을 launcher에 전달하기 위해 cmd line에 추가
		MkCmdLine cmdLine = arg;
		cmdLine.AddPair(MKDEF_PATCH_DOWNLOAD_URL_KEY, url);

		MkStr args;
		cmdLine.GetFullStr(args);

		localLauncherFilePath.OpenFileInVerb(args);
	}
	else
	{
		::MessageBox(NULL, errorMsg.GetPtr(), L"Starter error", MB_OK);
	}

	return ok;
}

//------------------------------------------------------------------------------------------------//
