#pragma once


#include "MkCore_MkWin32Application.h"


//------------------------------------------------------------------------------------------------//

#define APP_MGR MkAppManager::GetInstance()

class MkAppManager
{
public:

	inline MkWin32Application& GetApplication(void) { return m_App; }

	MkAppManager() {}
	~MkAppManager() {}

	static MkAppManager& GetInstance(void);

protected:

	MkWin32Application m_App;
};
