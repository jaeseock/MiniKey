
#include "MkCore_MkPageManager.h"
#include "MkCore_MkBasePage.h"


//------------------------------------------------------------------------------------------------//

void MkBasePage::SetPageFlowTable(const MkHashStr& condition, const MkHashStr& targetPage) const
{
	MK_PAGE_MGR.SetPageFlowTable(GetPageName(), condition, targetPage);
}

bool MkBasePage::SetMoveMessage(const MkHashStr& condition) const
{
	return (MK_PAGE_MGR.GetCurrentPageName() == GetPageName()) ? MK_PAGE_MGR.SetMoveMessage(condition) : false;
}

//------------------------------------------------------------------------------------------------//