
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
		// ���� ���� �ٿ�
		//if (MkFileDownloader::DownloadFileRightNow(infoFileURL, localTempInfoFilePath) != MkFileDownloader::eSuccess)
		if (!MkFileDownloader::DownloadFileRightNow(infoFileURL, localTempInfoFilePath))
		{
			errorMsg = infoFileURL + L"\n���� �ٿ�ε带 �����߽��ϴ�.";
			break;
		}

		// ���� ��� �ε�
		MkDataNode launcherNode;
		if (!launcherNode.Load(localTempInfoFilePath))
		{
			errorMsg = localTempInfoFilePath + L"\n������ �������� data node ������ �ƴմϴ�.";
			break;
		}

		// ���� ���� ��� ����
		MkStr launcherFileName;
		if (!launcherNode.GetData(MkPatchFileGenerator::LauncherFileName, launcherFileName, 0)) // ex> SimpleLauncher.exe
		{
			errorMsg = localTempInfoFilePath + L"\n���Ͽ� " + MkPatchFileGenerator::LauncherFileName.GetString() + L" unit�� �����ϴ�.";
			break;
		}

		if (launcherFileName.Empty())
		{
			errorMsg = localTempInfoFilePath + L"\n������ " + MkPatchFileGenerator::LauncherFileName.GetString() + L" unit�� ��� �ֽ��ϴ�.";
			break;
		}

		localLauncherFilePath = MkPathName::GetRootDirectory() + launcherFileName; // ex> D:\\Client\\SimpleLauncher.exe
		if (localLauncherFilePath.IsDirectoryPath())
		{
			errorMsg = localTempInfoFilePath + L"\n������ " + MkPatchFileGenerator::LauncherFileName.GetString() + L" unit�� �������� ���ϸ��� �ƴմϴ�.";
			break;
		}

		// ���� ��忡�� size, written time �б� �õ�. ������ ��ġ ������ ������ �ǹ�
		unsigned int origSize = 0, origWrittenTime = 0;
		if (launcherNode.GetData(MkPathName::KeyFileSize, origSize, 0) &&
			launcherNode.GetData(MkPathName::KeyWrittenTime, origWrittenTime, 0))
		{
			bool downloadLauncher = true;

			// ���� ���Ͽ��� size, written time �о�鿩 ��
			if (localLauncherFilePath.CheckAvailable())
			{
				unsigned int currFileSize = localLauncherFilePath.GetFileSize();
				unsigned int currFileWrittenTime = localLauncherFilePath.GetWrittenTime();

				downloadLauncher = ((origSize != currFileSize) || (origWrittenTime != currFileWrittenTime));
			}

			// ���� �ٿ�
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
					errorMsg = srcURL + L"\n���� �ٿ�ε带 �����߽��ϴ�.";
					break;
				}

				// ���� ���¸� ���� ����
				unsigned int compSize = 0;
				if (launcherNode.GetData(MkPathName::KeyFileSize, compSize, 1))
				{
					// ���� ���� �о�鿩
					MkByteArray compData;
					MkInterfaceForFileReading frInterface;
					if (!frInterface.SetUp(tmpLauncherFilePath))
					{
						errorMsg = tmpLauncherFilePath + L"\n���� ���� ����.";
						break;
					}
					if (frInterface.Read(compData, MkArraySection(0)) != compSize)
					{
						errorMsg = tmpLauncherFilePath + L"\n�б� �����ų� �ٿ�� ���� ���� ũ�Ⱑ �ٸ��ϴ�.";
						break;
					}
					frInterface.Clear();

					// ���� ����
					MkByteArray origData;
					if (MkZipCompressor::Uncompress(compData.GetPtr(), compData.GetSize(), origData) != origSize)
					{
						errorMsg = tmpLauncherFilePath + L"\n���� ���� �� ũ�Ⱑ �ٸ��ϴ�";
						break;
					}
					compData.Clear();

					// ���� �̸� ���Ϸ� ���� ����
					MkInterfaceForFileWriting fwInterface;
					if (!fwInterface.SetUp(tmpLauncherFilePath, true, true))
					{
						errorMsg = tmpLauncherFilePath + L"\n���� ���� ����.";
						break;
					}
					if (fwInterface.Write(origData, MkArraySection(0)) != origSize)
					{
						errorMsg = tmpLauncherFilePath + L"\n����� ����.";
						break;
					}
					fwInterface.Clear();
					origData.Clear();
				}
				else
				{
					// ũ�⸸ �˻�
					if (tmpLauncherFilePath.GetFileSize() != origSize)
					{
						errorMsg = tmpLauncherFilePath + L"\n�ٿ�� ���� ���� ũ�Ⱑ �ٸ��ϴ�. �´� ũ�� : " + MkStr(origSize);
						break;
					}
				}

				// ���� ���� ������ �����ϰ� ���� �ӽ� ������ �̸��� ����
				localLauncherFilePath.DeleteCurrentFile();
				tmpLauncherFilePath.MoveCurrentFile(localLauncherFilePath);

				// ���� �����ð� ����
				// �����ϴ��� ���࿡�� �̻��� �����Ƿ� �״������
				// �� ���� ��ġ�� �ٽ� �ٿ�ε� �õ�
				localLauncherFilePath.SetWrittenTime(origWrittenTime);
			}
		}
	}
	while (false);

	// ���� ���� ����
	localTempInfoFilePath.DeleteCurrentFile();

	// ���� �޼����� ������ ����
	bool ok = errorMsg.Empty();
	if (ok)
	{
		// url�� launcher�� �����ϱ� ���� cmd line�� �߰�
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
