#ifndef __MINIKEY_CORE_MKMEMORYTODATANODECONVERTER_H__
#define __MINIKEY_CORE_MKMEMORYTODATANODECONVERTER_H__


//------------------------------------------------------------------------------------------------//
// memory -> data node
//
// ������(Ű���� ��Ʈ)�κ��� ��带 ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"

class MkPathName;
class MkDataNode;
class MkInterfaceForDataReading;

class MkMemoryToDataNodeConverter
{
public:

	// memory -> data node
	bool Convert(const MkByteArray& srcArray, MkDataNode& destination, bool checkBinTag) const;

	// binary file -> memory -> data node
	bool Convert(const MkPathName& filePath, MkDataNode& destination) const;

protected:

	bool _BuildNode(MkInterfaceForDataReading& drInterface, MkDataNode& destination) const;
	unsigned int _BuildBlock(MkInterfaceForDataReading& drInterface, MkDataNode& targetNode) const;
};

//------------------------------------------------------------------------------------------------//

#endif
