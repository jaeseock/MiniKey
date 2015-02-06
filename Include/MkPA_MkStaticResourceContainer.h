#pragma once


//------------------------------------------------------------------------------------------------//
// static resource container
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkPA_MkTextNode.h"


#define MK_STATIC_RES MkStaticResourceContainer::Instance()


class MkStaticResourceContainer : public MkSingletonPattern<MkStaticResourceContainer>
{
public:

	//------------------------------------------------------------------------------------------------//
	// text node
	//------------------------------------------------------------------------------------------------//

	void LoadTextNode(const MkPathName& filePath);
	void LoadTextNode(const MkDataNode* dataNode);

	const MkTextNode& GetTextNode(const MkHashStr& name) const;

	//------------------------------------------------------------------------------------------------//

	void Clear(void);

	MkStaticResourceContainer() : MkSingletonPattern<MkStaticResourceContainer>() {}
	virtual ~MkStaticResourceContainer() { Clear(); }

protected:

	MkTextNode m_TextNode;
	MkTextNode m_EmptyTextNode;
};
