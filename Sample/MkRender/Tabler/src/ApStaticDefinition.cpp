
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "ApStaticDefinition.h"


//------------------------------------------------------------------------------------------------//

bool ApStaticDefinition::SetUp(const MkPathName& nodePath, const MkInt2& clientSize)
{
	MkDataNode node;
	if (!node.Load(nodePath))
		return false;

	m_ClientSize = clientSize;

	MkDataNode* stageNode = node.GetChildNode(L"Stage");
	if (stageNode != NULL)
	{
		stageNode->GetDataEx(L"ResourceTileSize", m_ResourceTileSize, 0);

		MkArray<MkInt2> fs;
		if (stageNode->GetData(L"FieldSizeRange", fs))
		{
			m_MinFieldSize = MkUInt2(static_cast<unsigned int>(fs[0].x), static_cast<unsigned int>(fs[0].y));
			m_MaxFieldSize = MkUInt2(static_cast<unsigned int>(fs[1].x), static_cast<unsigned int>(fs[1].y));
		}

		stageNode->GetData(L"DefaultResourceSlotSize", m_DefaultResourceSlotSize, 0);
		stageNode->GetData(L"MaxResourceSlotSize", m_MaxResourceSlotSize, 0);
	}
	
	return true;
}

ApStaticDefinition& ApStaticDefinition::Instance(void)
{
	static ApStaticDefinition instance;
	return instance;
}

ApStaticDefinition::ApStaticDefinition()
{
}
