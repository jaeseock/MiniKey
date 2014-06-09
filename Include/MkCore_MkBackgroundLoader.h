#ifndef __MINIKEY_CORE_MKBACKGROUNDLOADER_H__
#define __MINIKEY_CORE_MKBACKGROUNDLOADER_H__


//------------------------------------------------------------------------------------------------//
// global instance - background loader
//
// 등록된 background loading target들을 loading thread에 넘겨 관련 처리를 담당하는 white board
//
// thread-safe
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkCriticalSection.h"
#include "MkCore_MkDeque.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkBaseLoadingTarget.h"
#include "MkCore_MkPathName.h"

#define MK_BG_LOADER MkBackgroundLoader::Instance()


//------------------------------------------------------------------------------------------------//

class MkBackgroundLoader : public MkSingletonPattern<MkBackgroundLoader>
{
public:

	// loadingTarget 등록을 통한 background loading 명령
	bool RegisterLoadingTarget(const MkLoadingTargetPtr& loadingTarget, const MkPathName& filePath, const MkStr& argument);

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	typedef struct _LoadingTargetInfo
	{
		MkLoadingTargetPtr target;
		MkPathName filePath;
		MkStr argument;
	}
	LoadingTargetInfo;

	bool __GetNextLoadingTarget(LoadingTargetInfo& buffer);

	void __Clear(void);

	MkBackgroundLoader() : MkSingletonPattern() {}
	virtual ~MkBackgroundLoader() { __Clear(); }

protected:

	MkCriticalSection m_CS;
	MkDeque<LoadingTargetInfo> m_LoadingTargetList;
};

//------------------------------------------------------------------------------------------------//

#endif
