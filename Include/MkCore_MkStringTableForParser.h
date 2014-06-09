#ifndef __MINIKEY_CORE_MKSTRINGTABLEFORPARSER_H__
#define __MINIKEY_CORE_MKSTRINGTABLEFORPARSER_H__


#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"


//------------------------------------------------------------------------------------------------//

class MkStringTableForParser
{
public:
	void BuildStringTable(MkStr& source);
	bool Exist(const MkHashStr& key);
	void Clear(void);

	inline const MkStr& GetString(const MkHashStr& key) const { return m_StringTable[key]; }

	MkStringTableForParser() {}
	~MkStringTableForParser() { Clear(); }

protected:
	MkHashMap<MkHashStr, MkStr> m_StringTable;
};
//------------------------------------------------------------------------------------------------//

#endif