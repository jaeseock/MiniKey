
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

void MkStaticResourceContainer::Clear(void)
{
	m_TextNode.Clear();
}

//------------------------------------------------------------------------------------------------//
