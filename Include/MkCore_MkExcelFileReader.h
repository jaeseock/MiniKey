#ifndef __MINIKEY_CORE_MKEXCELFILEREADER_H__
#define __MINIKEY_CORE_MKEXCELFILEREADER_H__

//------------------------------------------------------------------------------------------------//
// M$ Excel ������ xlsx ���� �б� ������
// ODBC�� �̿��� ���� ���Ͽ� ���� access ���
// http://support.microsoft.com/kb/216686/ ����
//
// ���� ����� �߰��ϰ� ������ http://alones.kr/blog/i/entry/724 ����
//
// (NOTE) �ݵ�� ��� PC�� MS Excel�� ��ġ�Ǿ� �־�� ��
// (NOTE) SetUp�� �����ߴµ� Clear()���� ���� ��� Excel ���μ����� ��� OS�� ���� ����
//------------------------------------------------------------------------------------------------//

#include "ole2.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkArray.h"


class MkPathName;

class MkExcelFileReader
{
public:

	// Excel ���� �� ���� ����
	// fullPath : root directory ��� ��� Ȥ�� ���� ���� ���� ���
	// ��ȯ���� ��������
	bool SetUp(const MkPathName& filePath);

	// ���� ���� �ִ� ���Ͽ� �����ϴ� ��Ʈ �� ��ȯ
	inline unsigned int GetSheetSize(void) const { return m_SheetNameList.GetSize(); }

	// ��ȣ�� �۾���� ��Ʈ ����
	// ��ȯ���� ��������(��Ʈ ���翩��)
	bool SetActiveSheet(unsigned int index);

	// ��Ʈ������ �۾���� ��Ʈ ����
	// ��ȯ���� ��������(��Ʈ ���翩��)
	bool SetActiveSheet(const MkStr& name);

	// �ش� ��Ʈ ���翩�� ��ȯ
	bool SheetExist(const MkStr& name) const;

	// ��ȣ�� �ش��ϴ� ��Ʈ�� ��ȯ
	MkStr GetSheetName(unsigned int index) const;

	// column���� startRow �������� ���ӵ� ���� �����Ѵٰ� ������ �� ���������� �����ϴ� row ��ȯ
	// MKDEF_ARRAY_ERROR�� ��ȯ�Ǹ� ����
	unsigned int GetLastBlankRow(unsigned int column, unsigned int startRow = 0) const;

	// row���� startColumn �������� ���ӵ� ���� �����Ѵٰ� ������ �� ���������� �����ϴ� column ��ȯ
	// MKDEF_ARRAY_ERROR�� ��ȯ�Ǹ� ����
	unsigned int GetLastBlankColumn(unsigned int row, unsigned int startColumn = 0) const;

	// �� ��ȯ
	// row, column�� �»�ܿ��� 0���� �����ϴ� �ε���
	// ��ȯ���� ��������(�ش� �� ���翩��)
	bool GetData(const MkStr& position, MkStr& buffer) const; // �� �̸�(ex> A6, B5)���� ��ȯ
	bool GetDataRC(unsigned int row, unsigned int column, MkStr& buffer) const; // ��� ������ ��ȯ(YX)
	bool GetDataCR(unsigned int column, unsigned int row, MkStr& buffer) const; // ���� �࿡�� ��ȯ(XY)

	// ����. ���� �� Excel �ݱ�
	void Clear(void);

	// ��ġ ��ȯ
	static unsigned int ConvertColumn(const MkStr& column); // column keyword�� index�� ����
	static MkStr ConvertColumn(unsigned int column); // column index�� keyword�� ����
	static bool ConvertPosition(const MkStr& position, unsigned int& row, unsigned int& column); // �� �̸����� index�� ����
	static MkStr ConvertPosition(unsigned int row, unsigned int column); // index�� �� �̸��� ����

	MkExcelFileReader();
	~MkExcelFileReader();

protected:

	bool _AutoWrap(int autoType, VARIANT* varResult, IDispatch* dispatch, LPOLESTR name, int cArgs...) const;

	unsigned int _GetLastBlankInBinarySearch(unsigned int pivot, unsigned int start, unsigned int grid, unsigned int maxRange, bool rowMajor) const;

protected:

	typedef struct _ExcelProperty
	{
		IDispatch* applicationPtr;
		IDispatch* mainframePtr;
		IDispatch* workSheetPtr;
		IDispatch* currentSheetPtr;
	}
	ExcelProperty;

protected:

	bool m_EnableCOM;
	bool m_EnableExcel;

	ExcelProperty m_Property;

	MkArray<MkStr> m_SheetNameList;
};

#endif
