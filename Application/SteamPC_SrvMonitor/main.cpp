
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
		if (dirPath.GetDirectoryPathFromDialog(L"���� ������ ��ġ �� ������ �������ּ���.\n(steamcmd.exe ������ �����ϴ� ���丮�Դϴ�.)"))
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
			if (::MessageBox(NULL, L"�߸��� ����Դϴ�. �����Ͻðڽ��ϱ�?", L"SteamCafeProcAgent", MB_ICONQUESTION | MB_YESNO) == IDNO)
				break;
		}
	}

	if (targetEXE.Empty())
	{
		::MessageBox(NULL, L"����� ����մϴ�.\n�����Ͻ÷��� ���α׷��� �ٽ� �������ּ���.", L"SteamCafeProcAgent", MB_OK);
		return 0;
	}

	while (true)
	{
		// ���������� ������
		if (!MkWindowProcessOp::Exist(L"siteserverui.exe"))
		{
			targetEXE.OpenFileInVerb();
		}

		Sleep(20000);
	}
	
	return 0;
}

//------------------------------------------------------------------------------------------------//
