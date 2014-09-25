
#include "GameSUI_IslandVisitCB.h"
#include "GameSUI_GlobalSystemCB.h"
#include "GameSharedUI.h"

//------------------------------------------------------------------------------------------------//

#define GDEF_DELETE_NODE(node) if (node != NULL) { node->DetachFromParentNode(); delete node; node = NULL; }

MkBaseWindowNode* GameSharedUI::VisitCB = NULL;
MkBaseWindowNode* GameSharedUI::SystemCB = NULL;

void GameSharedUI::SetUp(void)
{
	VisitCB = new GameSUI_IslandVisitCB;
	SystemCB = new GameSUI_GlobalSystemCB;
}

void GameSharedUI::Clear(void)
{
	GDEF_DELETE_NODE(VisitCB);
	GDEF_DELETE_NODE(SystemCB);
}

//------------------------------------------------------------------------------------------------//
