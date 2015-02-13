#ifndef __MINIKEY_CORE_MKDATATEXTTOMEMORYCONVERTER_H__
#define __MINIKEY_CORE_MKDATATEXTTOMEMORYCONVERTER_H__


//------------------------------------------------------------------------------------------------//
// source text -> memory
//
// �ؽ�Ʈ�� �Ľ��� ���̳ʸ� ���·� ��ȯ
// �Ϻ� ���� �˻縸 ������ ���� Ű���带 1:1�� ��ȯ�ϱ� ������ ������� �����ʹ� ����ȭ�� ���� ���� ������
// ����ȭ �Ϸ�� �����͸� ���� ��� ���� �ε� �� �� memory�� ��ȯ �� ��
//
// ���ڿ� �迭�� prefix�� index�� ����ϴ� ��� �ڵ�ȭ ����. ������,
// ("~" / "PREFIX" / "1st FIRST_INDEX" / "1st LAST_INDEX" / "2nd FIRST_INDEX" / "2nd LAST_INDEX" / ...)
// ex>
//	�Ʒ� �� ������ ��ġ��
//	str SubsetList = "P_0" / "P_1" / "P_2" / "P_9" / "P_10" / "P_11";
//	str SubsetList = "~" / "P_" / "0" / "2" / "9" / "11";
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStringTableForParser.h"


class MkPathName;
class MkInterfaceForDataWriting;

class MkDataTextToMemoryConverter
{
public:

	// text source file -> memory
	bool Convert(const MkPathName& filePath, MkByteArray& destination);
	bool Convert(const MkByteArray& source, MkByteArray& destination);

	void Clear(void);

	~MkDataTextToMemoryConverter() { Clear(); }

protected:

	bool _Convert(MkStr& textSource, const MkPathName& filePath, MkByteArray& destination);
	unsigned int _SplitBlock(const MkStr& source, unsigned int startPosition, int& blockType, MkStr& blockName, MkStr& templateName, MkStr& body) const;
	bool _ParseText(const MkStr& source, MkInterfaceForDataWriting& dwInterface) const;
	unsigned int _ParseBlock(const MkStr& source, MkInterfaceForDataWriting& dwInterface, unsigned int startPosition) const;

protected:

	MkStr m_TargetFilePath;
	MkStringTableForParser m_StringTable;
};

//------------------------------------------------------------------------------------------------//

#endif
