
#include "MkCore_MkApplicationRegister.h"

#define MKDEF_REGISTER_FILENAME L"AppServiceData.mmd"


int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	MkStr::SetUp();
	MkPathName::SetUp();

	MkApplicationRegister::UpdateService(MKDEF_REGISTER_FILENAME);

	return 0;
}



