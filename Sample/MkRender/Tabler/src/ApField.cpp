
#include "MkCore_MkCheck.h"

#include "MkPA_MkSceneNode.h"

#include "ApStaticDefinition.h"
#include "ApField.h"


//------------------------------------------------------------------------------------------------//

bool ApField::SetUp(const MkUInt2& size, MkSceneNode* parentSceneNode)
{
	MK_CHECK(parentSceneNode != NULL, L"parentSceneNode가 NULL임")
		return false;

	m_SceneNode = parentSceneNode->CreateChildNode(L"<Field>");
	MK_CHECK(m_SceneNode != NULL, L"m_SceneNode가 NULL임")
		return false;

	m_FieldSize = size;

	// field의 LB position
	const MkFloat2& RTS = ApStaticDefinition::Instance().GetResourceTileSize();

	MkFloat2 totalTileSize = RTS;
	totalTileSize.x *= static_cast<float>(size.x);
	totalTileSize.y *= static_cast<float>(size.y);

	MkFloat2 fieldLocalPos;
	fieldLocalPos.x = (static_cast<float>(ApStaticDefinition::Instance().GetClientSize().x) - totalTileSize.x) * 0.5f;
	fieldLocalPos.y = RTS.y * 3.f + 10.f + 10.f + 50.f;
	m_SceneNode->SetLocalPosition(fieldLocalPos + RTS * 0.5f);

	m_Tiles.Reserve(m_FieldSize.y);

	MkFloat2 tileLocalPos(0.f, totalTileSize.y - RTS.y);

	for (unsigned int ri=0; ri<m_FieldSize.y; ++ri) // row
	{
		MkStr indexKey;
		indexKey += (ri + 1);

		MkArray<ApResourceUnit>& currRow = m_Tiles.PushBack();
		currRow.Reserve(m_FieldSize.x);

		for (unsigned int ci=0; ci<m_FieldSize.x; ++ci) // column
		{
			indexKey += (ci + 1);

			MkSceneNode* tileNode = m_SceneNode->CreateChildNode(indexKey);
			MK_CHECK(tileNode != NULL, indexKey + L"번 tileNode가 NULL임")
				return false;

			ApResourceUnit& currUnit = currRow.PushBack();
			currUnit.SetUp(ApResourceType(ApResourceType::eK_Normal, ri, ci), tileNode, L"Image\\stage_total.png");
			//
			currUnit.FieldCommand_Neutral();
			//

			indexKey.BackSpace(1);

			tileNode->SetLocalPosition(tileLocalPos);
			tileLocalPos.x += RTS.x;
		}

		tileLocalPos.x = 0.f;
		tileLocalPos.y -= RTS.y;
	}
	return true;
}

void ApField::Update(const MkTimeState& timeState)
{
	MK_INDEXING_LOOP(m_Tiles, i)
	{
		MK_INDEXING_LOOP(m_Tiles[i], j)
		{
			m_Tiles[i][j].Update(timeState);
		}
	}
}

ApField::ApField()
{
	m_SceneNode = NULL;
}

//------------------------------------------------------------------------------------------------//