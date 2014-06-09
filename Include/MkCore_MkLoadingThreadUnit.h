#ifndef __MINIKEY_CORE_MKLOADINGTHREADUNIT_H__
#define __MINIKEY_CORE_MKLOADINGTHREADUNIT_H__

//------------------------------------------------------------------------------------------------//
// loading thread unit
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBaseThreadUnit.h"


class MkLoadingThreadUnit : public MkBaseThreadUnit
{
public:

	virtual void Update(const MkTimeState& timeState);

	MkLoadingThreadUnit(const MkHashStr& threadName) : MkBaseThreadUnit(threadName) {}
	virtual ~MkLoadingThreadUnit() {}
};

//------------------------------------------------------------------------------------------------//

#endif
