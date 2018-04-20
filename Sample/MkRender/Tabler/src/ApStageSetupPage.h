#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkBasePage.h"
#include "MkCore_MkDataNode.h"


class ApStageSetupPage : public MkBasePage
{
public:

	virtual bool SetUp(MkDataNode& sharingNode);

	virtual void Update(const MkTimeState& timeState);

	virtual void Clear(MkDataNode* sharingNode = NULL);

	ApStageSetupPage(const MkHashStr& name);

	virtual ~ApStageSetupPage() { Clear(); }

protected:

	MkDataNode m_SetupNode;

};
