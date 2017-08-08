
#include "stdafx.h"
#include "MkAppManager.h"


MkAppManager& MkAppManager::GetInstance(void)
{
	static MkAppManager mgr;
	return mgr;
}
