#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

class MkHashStr;
class MkSceneNode;

class MkSceneNodeAllocator
{
public:

	static MkSceneNode* Alloc(const MkHashStr& templateName, const MkHashStr& nodeName);
};
