#ifndef __MINIKEY_CORE_MKDATATEXTTOMEMORYCONVERTER_H__
#define __MINIKEY_CORE_MKDATATEXTTOMEMORYCONVERTER_H__


//------------------------------------------------------------------------------------------------//
// source text -> memory
//
// �ؽ�Ʈ�� �Ľ��� ���̳ʸ� ���·� ��ȯ
// �Ϻ� ���� �˻縸 ������ ���� Ű���带 1:1�� ��ȯ
// ���� ������� �����ʹ� ����ȭ�� ���� ���� ������
// ����ȭ �Ϸ�� �����͸� ���� ��� ���� �ε� �� �� memory�� ��ȯ �� ��
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
