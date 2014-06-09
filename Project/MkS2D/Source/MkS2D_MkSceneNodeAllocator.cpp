
#include "MkCore_MkCheck.h"

#include "MkS2D_MkProjectDefinition.h"

#include "MkS2D_MkSceneNode.h"
#include "MkS2D_MkBaseWindowNode.h"

#include "MkS2D_MkSceneNodeAllocator.h"

//------------------------------------------------------------------------------------------------//

const static MkHashStr SCENENODE_TEMPLATE_NAME = MKDEF_S2D_BT_SCENENODE_TEMPLATE_NAME;
const static MkHashStr BASEWINNODE_TEMPLATE_NAME = MKDEF_S2D_BT_BASEWINNODE_TEMPLATE_NAME;


MkSceneNode* MkSceneNodeAllocator::Alloc(const MkHashStr& templateName, const MkHashStr& nodeName)
{
	MkSceneNode* instance = NULL;

	if (templateName == SCENENODE_TEMPLATE_NAME)
	{
		instance = new MkSceneNode(nodeName);
	}
	else if (templateName == BASEWINNODE_TEMPLATE_NAME)
	{
		instance = new MkBaseWindowNode(nodeName);
	}

	return instance;
}

//------------------------------------------------------------------------------------------------//