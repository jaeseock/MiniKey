
#include "MkCore_MkStr.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkWindowProcessOp.h"


int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	MkStr::SetUp();
	MkPathName::SetUp();

	MkPathName mmdPath;
	mmdPath.ConvertToModuleBasisAbsolutePath(L"path.mmd");

	MkPathName targetEXE;

	{
		MkDataNode pathNode;
		if (mmdPath.CheckAvailable() && pathNode.Load(mmdPath))
		{
			MkStr pathBuf;
			if (pathNode.GetData(L"path", pathBuf, 0))
			{
				MkPathName tmpPath = pathBuf;
				if (tmpPath.CheckAvailable())
				{
					targetEXE = tmpPath;
				}
			}
		}
	}
	
	while (targetEXE.Empty())
	{
		MkPathName dirPath;
		if (dirPath.GetDirectoryPathFromDialog(L"스팀 서버가 설치 된 폴더를 선택해주세요.\n(steamcmd.exe 파일이 존재하는 디렉토리입니다.)"))
		{
			dirPath.CheckAndAddBackslash();
			dirPath += L"steamcmd.exe";

			if (dirPath.CheckAvailable())
			{
				MkDataNode pathNode;
				pathNode.CreateUnit(L"path", static_cast<MkStr>(dirPath));
				pathNode.SaveToBinary(mmdPath);

				targetEXE = dirPath;
			}
		}

		if (targetEXE.Empty())
		{
			if (::MessageBox(NULL, L"잘못된 경로입니다. 재등록하시겠습니까?", L"SteamCafeProcAgent", MB_ICONQUESTION | MB_YESNO) == IDNO)
				break;
		}
	}

	if (targetEXE.Empty())
	{
		::MessageBox(NULL, L"등록을 취소합니다.\n재등록하시려면 프로그램을 다시 실행해주세요.", L"SteamCafeProcAgent", MB_OK);
		return 0;
	}

	while (true)
	{
		// 실행중이지 않으면
		if (!MkWindowProcessOp::Exist(L"siteserverui.exe"))
		{
			targetEXE.OpenFileInVerb();
		}

		Sleep(20000);
	}
	
	return 0;
}

//------------------------------------------------------------------------------------------------//
