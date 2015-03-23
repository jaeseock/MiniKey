#pragma once


//------------------------------------------------------------------------------------------------//
// static resource container
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"
#include "MkPA_MkTextNode.h"
#include "MkPA_MkWindowThemeSet.h"


#define MK_STATIC_RES MkStaticResourceContainer::Instance()


class MkStaticResourceContainer : public MkSingletonPattern<MkStaticResourceContainer>
{
public:

	//------------------------------------------------------------------------------------------------//
	// text node
	//------------------------------------------------------------------------------------------------//

	void LoadTextNode(const MkPathName& filePath);
	void LoadTextNode(const MkDataNode* dataNode);

	inline bool TextNodeExist(const MkArray<MkHashStr>& name) const { return m_TextNode.ChildExist(name); }

	const MkTextNode& GetTextNode(const MkArray<MkHashStr>& name) const;

	//------------------------------------------------------------------------------------------------//
	// window theme set
	//------------------------------------------------------------------------------------------------//

	bool LoadWindowThemeSet(const MkDataNode* dataNode);

	inline MkWindowThemeSet& GetWindowThemeSet(void) { return m_WindowThemeSet; }
	inline const MkWindowThemeSet& GetWindowThemeSet(void) const { return m_WindowThemeSet; }

	//------------------------------------------------------------------------------------------------//

	void Update(void);

	void Clear(void);

	MkStaticResourceContainer() : MkSingletonPattern<MkStaticResourceContainer>() {}
	virtual ~MkStaticResourceContainer() { Clear(); }

protected:

	// text node
	MkTextNode m_TextNode;
	MkTextNode m_EmptyTextNode;

	// window theme set
	MkWindowThemeSet m_WindowThemeSet;
};
