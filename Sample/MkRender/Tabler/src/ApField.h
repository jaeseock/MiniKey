#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "ApResourceUnit.h"


class ApField
{
public:

	bool SetUp(const MkUInt2& size, MkSceneNode* parentSceneNode);

	inline const MkUInt2& GetFieldSize(void) const { return m_FieldSize; }

	void Update(const MkTimeState& timeState);
	
	ApField();

protected:

	MkSceneNode* m_SceneNode;
	
	MkUInt2 m_FieldSize;
	MkArray< MkArray<ApResourceUnit> > m_Tiles;

};
