#pragma once

//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"

class MkPathName;

class ApStaticDefinition
{
public:

	bool SetUp(const MkPathName& nodePath, const MkInt2& clientSize);

	//------------------------------------------------------------------------------------------------//

	inline const MkInt2& GetClientSize(void) const { return m_ClientSize; }

	inline const MkFloat2& GetResourceTileSize(void) const { return m_ResourceTileSize; }

	inline const MkUInt2& GetMinFieldSize(void) const { return m_MinFieldSize; }
	inline const MkUInt2& GetMaxFieldSize(void) const { return m_MaxFieldSize; }

	inline unsigned int GetDefaultResourceSlotSize(void) const { return m_DefaultResourceSlotSize; }
	inline unsigned int GetMaxResourceSlotSize(void) const { return m_MaxResourceSlotSize; }

	//------------------------------------------------------------------------------------------------//

	ApStaticDefinition();

	// meyers's implementation
	static ApStaticDefinition& Instance(void);

protected:

	MkInt2 m_ClientSize;

	MkFloat2 m_ResourceTileSize;

	MkUInt2 m_MinFieldSize;
	MkUInt2 m_MaxFieldSize;

	unsigned int m_DefaultResourceSlotSize;
	unsigned int m_MaxResourceSlotSize;
};
