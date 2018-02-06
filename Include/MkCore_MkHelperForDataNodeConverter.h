#ifndef __MINIKEY_CORE_MKHELPERFORDATANODECONVERTER_H__
#define __MINIKEY_CORE_MKHELPERFORDATANODECONVERTER_H__


#include "MkCore_MkDataTypeDefinition.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"


//------------------------------------------------------------------------------------------------//

class MkInterfaceForDataWriting;
class MkDataNode;

class MkHelperForDataNodeConverter
{
public:

	static void ReserveInterface(const MkStr& source, MkInterfaceForDataWriting& dwInterface);

	static void ReserveInterface(const MkDataNode& source, MkInterfaceForDataWriting& dwInterface);

	static void ReserveInterface
		(unsigned int nodeCount, unsigned int templateCount, unsigned int unitCount, const unsigned int (&valueCount)[ePDT_MaxCount],
		MkInterfaceForDataWriting& dwInterface);

	static void WriteUnitString(ePrimitiveDataType unitType, const MkStr& unitStr, MkInterfaceForDataWriting& dwInterface);
};

//------------------------------------------------------------------------------------------------//

#endif