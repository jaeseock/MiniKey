#ifndef __MINIKEY_CORE_MKEXCELTOMEMORYCONVERTER_H__
#define __MINIKEY_CORE_MKEXCELTOMEMORYCONVERTER_H__


//------------------------------------------------------------------------------------------------//
// excel -> memory
//
// ���� ������ �Ľ��� ���̳ʸ� ���·� ��ȯ
// MkDataTextToMemoryConverter�� �����ϰ� �Ϻ� ���� �˻縸 ������ ���� Ű���带 1:1�� ��ȯ�ϴ�
// �����̹Ƿ� ������� �����ʹ� ���������� ����ȭ�� ���� ���� ������
// ����ȭ �Ϸ�� �����͸� ���� ��� ���� �ε� �� �� memory�� ��ȯ �� ��
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStr.h"

class MkPathName;
class MkInterfaceForDataWriting;
class MkExcelFileInterface;

class MkExcelToMemoryConverter
{
public:

	// text source file -> memory
	bool Convert(const MkPathName& filePath, MkByteArray& destination);

	void Clear(void);

	~MkExcelToMemoryConverter() { Clear(); }

protected:

	typedef struct _SheetData
	{
		MkStr templateName; // table type sheet�� ��� �ڽ� ���鿡�� �ϰ������� ����� ���ø���
		MkArray<int> type; // type index list(unit, node)
		MkArray<MkStr> key; // key list(type�� pair)
		MkArray<MkStr> name; // single : ��� ����, table : node name list
		MkArray< MkArray< MkArray<MkStr> > > data; // data[name size][type size][value size]
	}
	SheetData;

protected:

	bool _BuildSheetData(MkExcelFileInterface& reader);

	bool _GetBaseValue(MkExcelFileInterface& reader, const MkStr& position, const MkArray<MkStr>& availableList, MkStr& result) const;

	bool _ParseTable(MkExcelFileInterface& reader, const MkStr& type, const MkStr& anchor, const MkStr& startPosition, SheetData& dest);

	unsigned int _GetRowCount(const MkExcelFileInterface& reader, const MkUInt2& startPos) const;
	unsigned int _GetColumnCount(const MkExcelFileInterface& reader, const MkUInt2& startPos) const;

	bool _StringFilter(const MkStr& source, MkStr& buffer) const;
	bool _StringFilter(const MkStr& source, MkArray<MkStr>& buffer) const;
	void _ValueFilter(const MkStr& source, MkArray<MkStr>& buffer) const;

	void _ReserveInterface(MkInterfaceForDataWriting& dwInterface) const;

	bool _ConvertSheetToBinary(const MkStr& sheetName, MkInterfaceForDataWriting& dwInterface) const;

protected:

	MkStr m_TargetFilePath;
	MkStr m_TargetSheetName;

	MkArray<MkStr> m_SheetNameList;
	MkArray<SheetData> m_SheetDataList;
};

//------------------------------------------------------------------------------------------------//

#endif
