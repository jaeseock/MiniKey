#ifndef __MINIKEY_CORE_MKMEMORYTODATATEXTCONVERTER_H__
#define __MINIKEY_CORE_MKMEMORYTODATATEXTCONVERTER_H__


//------------------------------------------------------------------------------------------------//
// memory -> source text
//
// - �������� ����
// - �迭�� unit�� ��� �ּ����� ũ�⸦ ǥ��
// - ���ø� ����� ����� ��� ����, �ڽ� ������ ���� ���ø��� ������ ��� ������
// - ��� ������,
//   0. ö�� ������� ���ĵ� unit
//   1. ���� ȣ��Ǵ� ������� ���ĵ� ���ø� ���
//   2. ö�� ������� ���ĵ� �Ϲ� ���
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"

class MkPathName;
class MkInterfaceForDataReading;

class MkMemoryToDataTextConverter
{
public:

	// memory -> text source file
	bool Convert(const MkByteArray& srcArray, const MkPathName& filePath);

protected:

	void _InitiateHeaderMsg(MkStr& strBuffer, const MkPathName& filePath) const;
	bool _BuildText(MkInterfaceForDataReading& drInterface, unsigned int nodeDepth, MkStr& strBuffer) const;
	unsigned int _BuildBlock(MkInterfaceForDataReading& drInterface, unsigned int nodeDepth, MkStr& strBuffer) const;
};

//------------------------------------------------------------------------------------------------//

#endif
