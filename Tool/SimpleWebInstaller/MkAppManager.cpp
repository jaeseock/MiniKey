
#include "pch.h"
#include "MkAppManager.h"

#pragma comment (lib, "MkCore.lib")

MkAppManager& MkAppManager::GetInstance(void)
{
	static MkAppManager mgr;
	return mgr;
}
