#pragma once

//------------------------------------------------------------------------------------------------//
// M$ Excel ������ xlsx ���� IO ������
// ODBC�� �̿��� ���� ���Ͽ� ���� access ���
// http://support.microsoft.com/kb/216686/ ����
//
// (NOTE) �ݵ�� ��� PC�� MS Excel�� ��ġ�Ǿ� �־�� ��
// (NOTE) SetUp�� �����ߴµ� Clear()���� ���� ��� Excel ���μ����� ��� OS�� ���� ����
//------------------------------------------------------------------------------------------------//

#include "ole2.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkHashMap.h"


class MkPathName;

class MkExcelFileInterface
{
public:

	// Excel ����
	// ��ȯ���� ��������
	// "_header"��� �̸��� sheet �ϳ��� ����
	bool SetUp(void);

	// Excel ���� �� ���� ����
	// fullPath : root directory ��� ��� Ȥ�� ���� ���� ���� ���
	// ��ȯ���� ��������
	bool SetUp(const MkPathName& filePath);

	// �ش� ��η� ����
	// fullPath : root directory ��� ��� Ȥ�� ���� ���� ���� ���
	// ��ȯ���� ��������
	bool SaveAs(const MkPathName& filePath);

	// ���� ���� �ִ� ���Ͽ� �����ϴ� ��Ʈ �� ��ȯ
	unsigned int GetSheetSize(void) const;

	// �� ��Ʈ ����
	// (NOTE) ���� �۾���� ��Ʈ �ڿ� �߰� ��
	bool CreateSheet(const MkHashStr& name);

	// ��Ʈ �̸� ����
	bool RenameSheet(const MkHashStr& lastName, const MkHashStr& newName);

	// name �̸��� ��Ʈ ����
	// (NOTE) �ּ� �ϳ��� ��Ʈ�� ����(�ϳ��� ��� ���� �Ұ�)
	// (NOTE) ���� �۾���� ��Ʈ�� ��� ù��° ��Ʈ�� �۾� ��Ʈ�� ���� ��
	bool DeleteSheet(const MkHashStr& name);

	// ��ȣ�� �۾���� ��Ʈ ����
	// ��ȯ���� ��������(��Ʈ ���翩��)
	bool SetActiveSheet(unsigned int index);

	// ��Ʈ������ �۾���� ��Ʈ ����
	// ��ȯ���� ��������(��Ʈ ���翩��)
	bool SetActiveSheet(const MkHashStr& name);

	// �ش� ��Ʈ ���翩�� ��ȯ
	bool SheetExist(const MkHashStr& name) const;

	// ��ȣ�� �ش��ϴ� ��Ʈ�� ��ȯ
	MkStr GetSheetName(unsigned int index) const;

	// column���� startRow �������� ���ӵ� ���� �����Ѵٰ� ������ �� ���������� �����ϴ� row ��ȯ
	// MKDEF_ARRAY_ERROR�� ��ȯ�Ǹ� ����
	unsigned int GetLastBlankRow(unsigned int column, unsigned int startRow = 0) const;

	// row���� startColumn �������� ���ӵ� ���� �����Ѵٰ� ������ �� ���������� �����ϴ� column ��ȯ
	// MKDEF_ARRAY_ERROR�� ��ȯ�Ǹ� ����
	unsigned int GetLastBlankColumn(unsigned int row, unsigned int startColumn = 0) const;

	// �ش� ���� �� ����
	bool AutoFit(unsigned int column);

	// �� �Է�
	// row, column�� �»�ܿ��� 0���� �����ϴ� �ε���
	// ��ȯ���� ��������(�ش� �� ���翩��)
	bool PutData(const MkStr& position, bool value) const; // �� �̸�(ex> A6, B5)
	bool PutData(const MkStr& position, int value) const;
	bool PutData(const MkStr& position, unsigned int value) const;
	bool PutData(const MkStr& position, __int64 value) const;
	bool PutData(const MkStr& position, unsigned __int64 value) const;
	bool PutData(const MkStr& position, float value) const;
	bool PutData(const MkStr& position, const MkInt2& value) const;
	bool PutData(const MkStr& position, const MkVec2& value) const;
	bool PutData(const MkStr& position, const MkVec3& value) const;
	bool PutData(const MkStr& position, const MkStr& value) const;
	bool PutData(const MkStr& position, const MkByteArray& value) const;

	bool PutData(unsigned int row, unsigned int column, bool value) const; // ��� ��
	bool PutData(unsigned int row, unsigned int column, int value) const;
	bool PutData(unsigned int row, unsigned int column, unsigned int value) const;
	bool PutData(unsigned int row, unsigned int column, __int64 value) const;
	bool PutData(unsigned int row, unsigned int column, unsigned __int64 value) const;
	bool PutData(unsigned int row, unsigned int column, float value) const;
	bool PutData(unsigned int row, unsigned int column, const MkInt2& value) const;
	bool PutData(unsigned int row, unsigned int column, const MkVec2& value) const;
	bool PutData(unsigned int row, unsigned int column, const MkVec3& value) const;
	bool PutData(unsigned int row, unsigned int column, const MkStr& value) const;
	bool PutData(unsigned int row, unsigned int column, const MkByteArray& value) const;

	// �� ��ȯ
	// row, column�� �»�ܿ��� 0���� �����ϴ� �ε���
	// ��ȯ���� ��������(�ش� �� ���翩��)
	bool GetData(const MkStr& position, MkStr& buffer) const;
	bool GetData(unsigned int row, unsigned int column, MkStr& buffer) const;

	// �ش� ���� ��Ÿ�� ����
	bool SetBorder(const MkStr& position, int style);
	bool SetBorder(unsigned int row, unsigned int column, int style);

	// ����. ���� �� Excel �ݱ�
	void Clear(void);

	// ��ġ ��ȯ
	static unsigned int ConvertColumn(const MkStr& column); // column keyword�� index�� ����
	static MkStr ConvertColumn(unsigned int column); // column index�� keyword�� ����
	static bool ConvertPosition(const MkStr& position, unsigned int& row, unsigned int& column); // �� �̸����� index�� ����
	static bool ConvertPosition(const MkStr& position, MkUInt2& rc); // �� �̸����� index�� ����
	static MkStr ConvertPosition(unsigned int row, unsigned int column); // index�� �� �̸��� ����
	static MkStr ConvertPosition(const MkUInt2& rc); // index�� �� �̸��� ����

	MkExcelFileInterface();
	~MkExcelFileInterface();

protected:

	IDispatch* _GetIDispatchValue(int autoType, IDispatch* dispatch, LPOLESTR name) const;
	IDispatch* _GetIDispatchValue(int autoType, IDispatch* dispatch, LPOLESTR name, const MkStr& strArg) const;

	bool _PutValue(const MkStr& position, VARIANT& var, bool clearVar) const;

	IDispatch* _GetSheetIDispatch(unsigned int index) const;
	IDispatch* _GetSheetIDispatch(unsigned int index, MkHashStr& name) const;
	
	bool _InitMainFrame(void);
	bool _UpdateWorksheets(void);

	bool _AutoWrap(int autoType, VARIANT* varResult, IDispatch* dispatch, LPOLESTR name, int cArgs...) const;

	unsigned int _GetLastBlankInBinarySearch(unsigned int pivot, unsigned int start, unsigned int grid, unsigned int maxRange, bool rowMajor) const;

protected:

	typedef struct _ExcelProperty
	{
		IDispatch* applicationPtr;
		IDispatch* mainframePtr;
		IDispatch* currentBookPtr;
		IDispatch* currentWorksheetsPtr;
	}
	ExcelProperty;

protected:

	bool m_EnableCOM;
	bool m_EnableExcel;
	ExcelProperty m_Property;

	MkHashMap<MkHashStr, IDispatch*> m_Sheets;
	IDispatch* m_ActiveSheet;
};
