#ifndef __MINIKEY_CORE_MKLOGICTHREADUNIT_H__
#define __MINIKEY_CORE_MKLOGICTHREADUNIT_H__

//------------------------------------------------------------------------------------------------//
// logic thread unit
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBaseThreadUnit.h"


class MkLogicThreadUnit : public MkBaseThreadUnit
{
public:

	virtual void Run(void);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(void);

	MkLogicThreadUnit(const MkHashStr& threadName) : MkBaseThreadUnit(threadName) {}
	virtual ~MkLogicThreadUnit() {}
};

//------------------------------------------------------------------------------------------------//

#endif
