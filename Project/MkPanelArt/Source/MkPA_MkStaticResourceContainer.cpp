
#include "MkCore_MkDataNode.h"
#include "MkPA_MkStaticResourceContainer.h"


//------------------------------------------------------------------------------------------------//

void MkStaticResourceContainer::LoadTextNode(const MkPathName& filePath)
{
	m_TextNode.Clear();
	m_TextNode.SetUp(filePath);
}

void MkStaticResourceContainer::LoadTextNode(const MkDataNode* dataNode)
{
	if (dataNode != NULL)
	{
		MkStr filePath;
		if (dataNode->GetData(L"TextNode", filePath, 0) && (!filePath.Empty()))
		{
			LoadTextNode(filePath);
		}
	}
}

const MkTextNode& MkStaticResourceContainer::GetTextNode(const MkHashStr& name) const
{
	return m_TextNode.ChildExist(name) ? *m_TextNode.GetChildNode(name) : m_EmptyTextNode;
}

//------------------------------------------------------------------------------------------------//

bool MkStaticResourceContainer::LoadWindowThemeSet(const MkDataNode* dataNode)
{
	if (dataNode == NULL)
		return true;

	m_WindowThemeSet.Clear();
	bool ok = m_WindowThemeSet.SetUp(*dataNode);
	if (!ok)
	{
		m_WindowThemeSet.Clear();
	}
	return ok;
}

//------------------------------------------------------------------------------------------------//

void MkStaticResourceContainer::Update(void)
{
	m_WindowThemeSet.UnloadUnusedThemeImage();
}

void MkStaticResourceContainer::Clear(void)
{
	m_TextNode.Clear();
	m_WindowThemeSet.Clear();
}

//------------------------------------------------------------------------------------------------//
