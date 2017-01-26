#ifndef __MINIKEY_CORE_MKDATANODETOMEMORYCONVERTER_H__
#define __MINIKEY_CORE_MKDATANODETOMEMORYCONVERTER_H__


//------------------------------------------------------------------------------------------------//
// data node -> memory
//
// ��带 �м��� ���̳ʸ� �����ͷ� ��ȯ
// �ڵ� ����ǹǷ� �޸� ��� ���а� �߻����� �ʴ� �� ���� ���ɼ� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"


class MkPathName;
class MkDataNode;
class MkInterfaceForDataWriting;

class MkDataNodeToMemoryConverter
{
public:

	// data node -> memory
	bool Convert(const MkDataNode& source, MkByteArray& destination) const;

	// data node -> memory -> compress -> insert tag to front -> binary file
	bool Convert(const MkDataNode& source, const MkPathName& filePath) const;

protected:

	void _ParseNode(const MkDataNode& targetNode, MkInterfaceForDataWriting& dwInterface) const;

	void _ConvertUnits(const MkDataNode& targetNode, MkInterfaceForDataWriting& dwInterface) const;

	void _ConvertChildNodes(const MkDataNode& targetNode, MkInterfaceForDataWriting& dwInterface) const;
};

//------------------------------------------------------------------------------------------------//

#endif
