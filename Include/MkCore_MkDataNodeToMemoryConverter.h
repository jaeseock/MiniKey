#ifndef __MINIKEY_CORE_MKDATANODETOMEMORYCONVERTER_H__
#define __MINIKEY_CORE_MKDATANODETOMEMORYCONVERTER_H__


//------------------------------------------------------------------------------------------------//
// data node -> memory
//
// 노드를 분석해 바이너리 데이터로 전환
// 자동 수행되므로 메모리 얼록 실패가 발생하지 않는 한 오류 가능성 없음
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"


class MkPathName;
class MkDataNode;
class MkInterfaceForDataWriting;

class MkDataNodeToMemoryConverter
{
public:

	// data node -> memory
	bool ConvertToMemory(const MkDataNode& source, MkByteArray& destination) const;

	// ConvertToMemory() -> binary data(tag + compressed data)
	bool ConvertToBinaryData(const MkDataNode& source, MkByteArray& destination) const;

	// ConvertToBinaryData() -> binary file
	bool ConvertToBinaryFile(const MkDataNode& source, const MkPathName& filePath) const;

protected:

	void _ParseNode(const MkDataNode& targetNode, MkInterfaceForDataWriting& dwInterface) const;

	void _ConvertUnits(const MkDataNode& targetNode, MkInterfaceForDataWriting& dwInterface) const;

	void _ConvertChildNodes(const MkDataNode& targetNode, MkInterfaceForDataWriting& dwInterface) const;
};

//------------------------------------------------------------------------------------------------//

#endif
