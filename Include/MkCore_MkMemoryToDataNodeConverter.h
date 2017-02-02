#ifndef __MINIKEY_CORE_MKMEMORYTODATANODECONVERTER_H__
#define __MINIKEY_CORE_MKMEMORYTODATANODECONVERTER_H__


//------------------------------------------------------------------------------------------------//
// memory -> data node
//
// 데이터(키워드 세트)로부터 노드를 구성
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"

class MkPathName;
class MkDataNode;
class MkInterfaceForDataReading;

class MkMemoryToDataNodeConverter
{
public:

	// memory -> data node
	bool ConvertMemory(const MkByteArray& srcArray, MkDataNode& destination) const;

	// binary data -> check tag -> uncompress data -> ConvertMemory()
	bool ConvertBinaryData(const MkByteArray& srcArray, MkDataNode& destination) const;

	// binary file -> ConvertBinaryData()
	bool ConvertBinaryFile(const MkPathName& filePath, MkDataNode& destination) const;

protected:

	bool _BuildNode(MkInterfaceForDataReading& drInterface, MkDataNode& destination) const;
	unsigned int _BuildBlock(MkInterfaceForDataReading& drInterface, MkDataNode& targetNode) const;
};

//------------------------------------------------------------------------------------------------//

#endif
