
#include <comdef.h>
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkExcelFileInterface.h"


//------------------------------------------------------------------------------------------------//

bool MkExcelFileInterface::SetUp(void)
{
	Clear();
	do
	{
		if (!_InitMainFrame())
			break;

		m_Property.currentBookPtr = _GetIDispatchValue(DISPATCH_PROPERTYGET, m_Property.mainframePtr, L"Add");
		if (m_Property.currentBookPtr == NULL)
			break;

		if (!_UpdateWorksheets())
			break;

		MkArray<MkHashStr> keys;
		m_Sheets.GetKeyList(keys);

		MK_INDEXING_LOOP(keys, i)
		{
			if (i > 0)
			{
				DeleteSheet(keys[i]);
			}
		}

		RenameSheet(keys[0], L"_header");
		return true;
	}
	while (false);
	Clear();
	return false;
}

bool MkExcelFileInterface::SetUp(const MkPathName& filePath)
{
	MkPathName fullPath;
	fullPath.ConvertToRootBasisAbsolutePath(filePath);

	MK_CHECK(fullPath.CheckAvailable(), filePath + L" 경로가 올바르지 않음")
		return false;

	do
	{
		if (!_InitMainFrame())
			break;

		m_Property.currentBookPtr = _GetIDispatchValue(DISPATCH_METHOD, m_Property.mainframePtr, L"Open", fullPath);
		if (m_Property.currentBookPtr == NULL)
			break;

		if (!_UpdateWorksheets())
			break;

		return true;
	}
	while (false);
	Clear();
	return false;
}

bool MkExcelFileInterface::SaveAs(const MkPathName& filePath)
{
	if ((!m_EnableExcel) || (m_ActiveSheet == NULL))
		return false;

	MkPathName fullPath;
	fullPath.ConvertToRootBasisAbsolutePath(filePath);

	if (fullPath.CheckAvailable())
	{
		fullPath.DeleteCurrentFile();
	}

	fullPath.MakeDirectoryPath();

	_GetIDispatchValue(DISPATCH_METHOD, m_ActiveSheet, L"SaveAS", fullPath);
	return true;
}

unsigned int MkExcelFileInterface::GetSheetSize(void) const
{
	return m_Sheets.GetSize();
}

bool MkExcelFileInterface::CreateSheet(const MkHashStr& name)
{
	if ((!m_EnableExcel) || m_Sheets.Exist(name) || (m_ActiveSheet == NULL) || (m_Property.currentWorksheetsPtr == NULL))
		return false;

	VARIANT result;
	VariantInit(&result);

	VARIANT frontVar;
	frontVar.pdispVal = m_ActiveSheet;
	frontVar.vt = VT_DISPATCH;

	VARIANT endVar;
	endVar.pdispVal = NULL;
	endVar.vt = VT_NULL;

    if (!_AutoWrap(DISPATCH_METHOD, &result, m_Property.currentWorksheetsPtr, L"Add", 2, frontVar, endVar))
		return false;

	_GetIDispatchValue(DISPATCH_PROPERTYPUT, result.pdispVal, L"name", name.GetString());
	m_Sheets.Create(name, result.pdispVal);
	return true;
}

bool MkExcelFileInterface::RenameSheet(const MkHashStr& lastName, const MkHashStr& newName)
{
	if (lastName == newName)
		return true;

	if ((!m_Sheets.Exist(lastName)) || m_Sheets.Exist(newName))
		return false;

	IDispatch* sheet = m_Sheets[lastName];
	_GetIDispatchValue(DISPATCH_PROPERTYPUT, sheet, L"name", newName.GetString());

	m_Sheets.Erase(lastName);
	m_Sheets.Create(newName, sheet);
	return true;
}

bool MkExcelFileInterface::DeleteSheet(const MkHashStr& name)
{
	if ((!m_Sheets.Exist(name)) || (m_Sheets.GetSize() <= 1))
		return false;

	IDispatch* targetSheet = m_Sheets[name];
	if (targetSheet == m_ActiveSheet)
	{
		if (!SetActiveSheet(0))
			return false;
	}

	bool ok = _AutoWrap(DISPATCH_METHOD, NULL, targetSheet, L"Delete", 0);
	if (ok)
	{
		m_Sheets.Erase(name);
	}
	return ok;
}

bool MkExcelFileInterface::SetActiveSheet(unsigned int index)
{
	IDispatch* sheet = _GetSheetIDispatch(index);
	bool ok = (sheet != NULL);
	if (ok)
	{
		m_ActiveSheet = sheet;
	}
	return ok;
}

bool MkExcelFileInterface::SetActiveSheet(const MkHashStr& name)
{
	if (!m_Sheets.Exist(name))
		return false;

	m_ActiveSheet = m_Sheets[name];
	return true;
}

bool MkExcelFileInterface::SheetExist(const MkHashStr& name) const
{
	return m_Sheets.Exist(name);
}

MkStr MkExcelFileInterface::GetSheetName(unsigned int index) const
{
	MkHashStr sheetName;
	_GetSheetIDispatch(index, sheetName);
	return sheetName.GetString();
}

unsigned int MkExcelFileInterface::GetLastBlankRow(unsigned int column, unsigned int startRow) const
{
	return _GetLastBlankInBinarySearch(column, startRow, 1, 16383, true);
}

unsigned int MkExcelFileInterface::GetLastBlankColumn(unsigned int row, unsigned int startColumn) const
{
	return _GetLastBlankInBinarySearch(row, startColumn, 1, 17575, false);
}

bool MkExcelFileInterface::AutoFit(unsigned int column)
{
	if ((!m_EnableExcel) || (m_ActiveSheet == NULL))
		return false;

	// range
	MkStr position = ConvertColumn(column);
	IDispatch* targetRange = _GetIDispatchValue(DISPATCH_PROPERTYGET, m_ActiveSheet, L"Columns", position);
	if (targetRange == NULL)
		return false;

	IDispatch* targetColumn = _GetIDispatchValue(DISPATCH_PROPERTYGET, targetRange, L"EntireColumn");
	targetRange->Release();
	if (targetColumn == NULL)
		return false;

	bool ok = _AutoWrap(DISPATCH_METHOD, NULL, targetColumn, L"AutoFit", 0);
	targetColumn->Release();
	return ok;
}

bool MkExcelFileInterface::PutData(const MkStr& position, bool value) const
{
	return PutData(position, (value) ? MkStr(L"Yes") : MkStr(L"No"));
}

bool MkExcelFileInterface::PutData(const MkStr& position, int value) const
{
	VARIANT argVar;
	argVar.vt = VT_I4;
	argVar.lVal = value;
	return _PutValue(position, argVar, false);
}

bool MkExcelFileInterface::PutData(const MkStr& position, unsigned int value) const
{
	VARIANT argVar;
	argVar.vt = VT_UI4;
	argVar.ulVal = value;
	return _PutValue(position, argVar, false);
}

bool MkExcelFileInterface::PutData(const MkStr& position, __int64 value) const
{
	VARIANT argVar;
	argVar.vt = VT_I8;
	argVar.llVal = value;
	return _PutValue(position, argVar, false);
}

bool MkExcelFileInterface::PutData(const MkStr& position, unsigned __int64 value) const
{
	VARIANT argVar;
	argVar.vt = VT_UI8;
	argVar.ullVal = value;
	return _PutValue(position, argVar, false);
}

bool MkExcelFileInterface::PutData(const MkStr& position, float value) const
{
	VARIANT argVar;
	argVar.vt = VT_R4;
	argVar.fltVal = value;
	return _PutValue(position, argVar, false);
}

bool MkExcelFileInterface::PutData(const MkStr& position, const MkInt2& value) const
{
	return PutData(position, MkStr(value));
}

bool MkExcelFileInterface::PutData(const MkStr& position, const MkVec2& value) const
{
	return PutData(position, MkStr(value));
}

bool MkExcelFileInterface::PutData(const MkStr& position, const MkVec3& value) const
{
	return PutData(position, MkStr(value));
}

bool MkExcelFileInterface::PutData(const MkStr& position, const MkStr& value) const
{
	VARIANT argVar;
	argVar.vt = VT_BSTR;
	argVar.bstrVal = SysAllocString(value.GetPtr());
	return _PutValue(position, argVar, true);
}

bool MkExcelFileInterface::PutData(const MkStr& position, const MkByteArray& value) const
{
	MkStr buffer;
	buffer.ImportByteArray(value);
	return PutData(position, buffer);
}

bool MkExcelFileInterface::PutData(unsigned int row, unsigned int column, bool value) const { return PutData(ConvertPosition(row, column), value); }
bool MkExcelFileInterface::PutData(unsigned int row, unsigned int column, int value) const { return PutData(ConvertPosition(row, column), value); }
bool MkExcelFileInterface::PutData(unsigned int row, unsigned int column, unsigned int value) const { return PutData(ConvertPosition(row, column), value); }
bool MkExcelFileInterface::PutData(unsigned int row, unsigned int column, __int64 value) const { return PutData(ConvertPosition(row, column), value); }
bool MkExcelFileInterface::PutData(unsigned int row, unsigned int column, unsigned __int64 value) const { return PutData(ConvertPosition(row, column), value); }
bool MkExcelFileInterface::PutData(unsigned int row, unsigned int column, float value) const { return PutData(ConvertPosition(row, column), value); }
bool MkExcelFileInterface::PutData(unsigned int row, unsigned int column, const MkInt2& value) const { return PutData(ConvertPosition(row, column), value); }
bool MkExcelFileInterface::PutData(unsigned int row, unsigned int column, const MkVec2& value) const { return PutData(ConvertPosition(row, column), value); }
bool MkExcelFileInterface::PutData(unsigned int row, unsigned int column, const MkVec3& value) const { return PutData(ConvertPosition(row, column), value); }
bool MkExcelFileInterface::PutData(unsigned int row, unsigned int column, const MkStr& value) const { return PutData(ConvertPosition(row, column), value); }
bool MkExcelFileInterface::PutData(unsigned int row, unsigned int column, const MkByteArray& value) const { return PutData(ConvertPosition(row, column), value); }

bool MkExcelFileInterface::GetData(const MkStr& position, MkStr& buffer) const
{
	if (position.Empty() || (!m_EnableExcel) || (m_ActiveSheet == NULL))
		return false;

	buffer.Clear();

	// range
	IDispatch* targetRange = _GetIDispatchValue(DISPATCH_PROPERTYGET, m_ActiveSheet, L"Range", position + L":" + position);
	if (targetRange == NULL)
		return true; // 범위를 벗어남. 셀은 존재하지만 값이 없는 것으로 처리(VT_EMPTY와 동일)
	
	// get
	VARIANT result;
	VariantInit(&result);
	_AutoWrap(DISPATCH_PROPERTYGET, &result, targetRange, L"Value", 0);
	targetRange->Release();

	if (result.vt != VT_EMPTY)
	{
		VariantChangeType(&result, &result, VARIANT_NOUSEROVERRIDE, VT_BSTR);
		buffer = static_cast<const wchar_t*>(result.bstrVal);
	}
	VariantClear(&result);
	return true;
}

bool MkExcelFileInterface::GetData(unsigned int row, unsigned int column, MkStr& buffer) const
{
	MkStr posName = ConvertPosition(row, column);
	return GetData(posName, buffer);
}

bool MkExcelFileInterface::SetBorder(const MkStr& position, int style)
{
	if (position.Empty() || (!m_EnableExcel) || (m_ActiveSheet == NULL))
		return false;

	VARIANT argVar;
	argVar.vt = VT_I4;
	argVar.lVal = style;

	// range
	IDispatch* targetRange = _GetIDispatchValue(DISPATCH_PROPERTYGET, m_ActiveSheet, L"Range", position + L":" + position);
	if (targetRange == NULL)
		return false;

	bool ok = _AutoWrap(DISPATCH_METHOD, NULL, targetRange, L"BorderAround", 1, argVar);
	targetRange->Release();
	return ok;
}

bool MkExcelFileInterface::SetBorder(unsigned int row, unsigned int column, int style)
{
	return SetBorder(ConvertPosition(row, column), style);
}

void MkExcelFileInterface::Clear(void)
{
	if (m_EnableExcel)
	{
		VARIANT svar;
		svar.vt = VT_I4;
		svar.lVal = 1;
		_AutoWrap(DISPATCH_PROPERTYPUT, NULL, m_Property.currentBookPtr, L"Saved", 1, svar);
		_AutoWrap(DISPATCH_METHOD, NULL, m_Property.mainframePtr, L"Close", 0);
		_AutoWrap(DISPATCH_METHOD, NULL, m_Property.applicationPtr, L"Quit", 0);

		m_Sheets.Clear();
		m_ActiveSheet = NULL;

		MK_RELEASE(m_Property.currentWorksheetsPtr);
		MK_RELEASE(m_Property.currentBookPtr);
		MK_RELEASE(m_Property.mainframePtr);
		MK_RELEASE(m_Property.applicationPtr);

		m_EnableExcel = false;
	}
}

unsigned int MkExcelFileInterface::ConvertColumn(const MkStr& column)
{
	MkStr cBuf = column;
	unsigned int cSize = cBuf.GetSize();
	assert((cSize >= 1) && (cSize <= 3));
	int level[3] = {-1, -1, -1};
	switch (cSize)
	{
	case 2: cBuf = L" " + cBuf; break;
	case 1: cBuf = L"  " + cBuf; break;
	}
	switch (cSize)
	{
	case 3: level[0] = static_cast<int>(cBuf.GetAt(0)) - 65;
	case 2: level[1] = static_cast<int>(cBuf.GetAt(1)) - 65;
	case 1: level[2] = static_cast<int>(cBuf.GetAt(2)) - 65;
		break;
	}

	if (level[0] < 0)
	{
		++level[1];
	}
	++level[0];

	return static_cast<unsigned int>(level[0] * 676 + level[1] * 26 + level[2]);
}

MkStr MkExcelFileInterface::ConvertColumn(unsigned int column)
{
	assert(column < 17576);
	int level[3];
	level[0] = column / 676;
	column = column % 676;
	level[1] = column / 26;
	level[2] = column % 26;

	if (level[0] == 0)
	{
		--level[1];
	}
	--level[0];

	MkStr cKey[3];
	for (int i=0; i<3; ++i)
	{
		if (level[i] >= 0)
		{
			wchar_t asc = 65 + static_cast<wchar_t>(level[i]);
			cKey[i] = asc;
		}
	}

	return (cKey[0] + cKey[1] + cKey[2]);
}

bool MkExcelFileInterface::ConvertPosition(const MkStr& position, unsigned int& row, unsigned int& column)
{
	unsigned int size = position.GetSize();
	unsigned int columeEndPos = 0;
	for (unsigned int i=0; i<size; ++i)
	{
		wchar_t tag = position.GetAt(i);
		if ((tag < 65) || (tag > 90))
		{
			columeEndPos = i;
			break;
		}
	}

	if (columeEndPos == 0)
		return false;

	MkStr rBuf;
	position.GetSubStr(MkArraySection(columeEndPos), rBuf);
	row = rBuf.ToInteger() - 1;

	MkStr cBuf;
	position.GetSubStr(MkArraySection(0, columeEndPos), cBuf);
	column = ConvertColumn(cBuf);

	return true;
}

bool MkExcelFileInterface::ConvertPosition(const MkStr& position, MkUInt2& rc)
{
	return ConvertPosition(position, rc.x, rc.y);
}

MkStr MkExcelFileInterface::ConvertPosition(unsigned int row, unsigned int column)
{
	MkStr position = ConvertColumn(column);
	position += row + 1;
	return position;
}

MkStr MkExcelFileInterface::ConvertPosition(const MkUInt2& rc)
{
	return ConvertPosition(rc.x, rc.y);
}

MkExcelFileInterface::MkExcelFileInterface()
{
	HRESULT hr = CoInitialize(NULL);
	m_EnableCOM = ((hr == S_OK) || (hr == S_FALSE));
	m_EnableExcel = false;

	m_Property.currentWorksheetsPtr = NULL;
	m_Property.currentBookPtr = NULL;
	m_Property.mainframePtr = NULL;
	m_Property.applicationPtr = NULL;

	m_ActiveSheet = NULL;
}

MkExcelFileInterface::~MkExcelFileInterface()
{
	Clear();

	if (m_EnableCOM)
	{
		CoUninitialize();
	}
}

//------------------------------------------------------------------------------------------------//

IDispatch* MkExcelFileInterface::_GetIDispatchValue(int autoType, IDispatch* dispatch, LPOLESTR name) const
{
	VARIANT result;
    VariantInit(&result);
    MK_CHECK(_AutoWrap(autoType, &result, dispatch, name, 0), L"IDispatch 실패")
		return NULL;
    return result.pdispVal;
}

IDispatch* MkExcelFileInterface::_GetIDispatchValue(int autoType, IDispatch* dispatch, LPOLESTR name, const MkStr& strArg) const
{
	VARIANT result;
    VariantInit(&result);

	VARIANT argVar;
	argVar.vt = VT_BSTR;
	argVar.bstrVal = SysAllocString(strArg.GetPtr());

	MK_CHECK(_AutoWrap(autoType, &result, dispatch, name, 1, argVar), L"IDispatch 실패(" + strArg + L")")
	{
		VariantClear(&argVar);
		return NULL;
	}

	VariantClear(&argVar);
    return result.pdispVal;
}

bool MkExcelFileInterface::_PutValue(const MkStr& position, VARIANT& var, bool clearVar) const
{
	if (position.Empty() || (!m_EnableExcel) || (m_ActiveSheet == NULL))
		return false;

	// range
	IDispatch* targetRange = _GetIDispatchValue(DISPATCH_PROPERTYGET, m_ActiveSheet, L"Range", position + L":" + position);
	if (targetRange == NULL)
		return false;

	bool ok = _AutoWrap(DISPATCH_PROPERTYPUT, NULL, targetRange, L"Value", 1, var);
	targetRange->Release();
	if (clearVar)
	{
		VariantClear(&var);
	}
	return ok;
}

IDispatch* MkExcelFileInterface::_GetSheetIDispatch(unsigned int index) const
{
	if (m_EnableExcel && (m_Property.applicationPtr != NULL))
	{
		VARIANT result;
		VariantInit(&result);

		VARIANT vaSheet;
		vaSheet.vt = VT_I4;
		vaSheet.lVal = static_cast<LONG>(index) + 1;

		if (_AutoWrap(DISPATCH_PROPERTYGET, &result, m_Property.applicationPtr, L"Worksheets", 1, vaSheet))
			return result.pdispVal;
	}
	return NULL;
}

IDispatch* MkExcelFileInterface::_GetSheetIDispatch(unsigned int index, MkHashStr& name) const
{
	IDispatch* sheet = _GetSheetIDispatch(index);
	if (sheet != NULL)
	{
		VARIANT result;
		VariantInit(&result);
		if (_AutoWrap(DISPATCH_PROPERTYGET, &result, sheet, L"name", 0))
		{
			name = result.bstrVal;
		}
	}
	return sheet;
}

bool MkExcelFileInterface::_InitMainFrame(void)
{
	Clear();

	MK_CHECK(m_EnableCOM, L"COM 초기화가 되지 않아 MkExcelFileInterface 사용 불가")
		return false;

	// class id
	CLSID clsid;
	HRESULT hr = CLSIDFromProgID(L"Excel.Application", &clsid);
	MK_CHECK(SUCCEEDED(hr), L"Excel.Application에 해당하는 ClassID가 없음")
		return false;

	// application
	hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&m_Property.applicationPtr);
	MK_CHECK(SUCCEEDED(hr), L"Excel Application이 존재하지 않음")
		return false;

	// mainframe
	m_Property.mainframePtr = _GetIDispatchValue(DISPATCH_PROPERTYGET, m_Property.applicationPtr, L"Workbooks");

	// finally
	m_EnableExcel = (m_Property.mainframePtr != NULL);
	return m_EnableExcel;
}

bool MkExcelFileInterface::_UpdateWorksheets(void)
{
	if (m_EnableExcel == NULL)
		return false;

	m_Property.currentWorksheetsPtr = _GetIDispatchValue(DISPATCH_PROPERTYGET, m_Property.currentBookPtr, L"Worksheets");
	if (m_Property.currentWorksheetsPtr == NULL)
		return false;

	// active sheet
	m_ActiveSheet = _GetIDispatchValue(DISPATCH_PROPERTYGET, m_Property.applicationPtr, L"ActiveSheet");
	if (m_ActiveSheet == NULL)
		return false;

	// get name list
	unsigned int index = 0;
	while (true)
	{
		MkHashStr sheetName;
		IDispatch* currSheet = _GetSheetIDispatch(index, sheetName);
		if (currSheet != NULL)
		{
			m_Sheets.Create(sheetName, currSheet);
			++index;
		}
		else
			break;
	}

	return (!m_Sheets.Empty());
}

bool MkExcelFileInterface::_AutoWrap(int autoType, VARIANT* varResult, IDispatch* dispatch, LPOLESTR name, int cArgs...) const
{
	if (dispatch == NULL)
		return false;

	// Begin variable-argument list
	va_list marker;
	va_start(marker, cArgs);

	// Variables used
	DISPPARAMS dp = { NULL, NULL, 0, 0 };
	DISPID dispidNamed = DISPID_PROPERTYPUT;
	DISPID dispID;
	HRESULT hr;
	
	// Get DISPID for name passed
	hr = dispatch->GetIDsOfNames(IID_NULL, &name, 1, LOCALE_USER_DEFAULT, &dispID);
	if (FAILED(hr))
		return false;

	// Allocate memory for arguments
	VARIANT *pArgs = new VARIANT[cArgs+1];

	// Extract arguments
	for (int i=0; i<cArgs; i++) 
	{
		pArgs[i] = va_arg(marker, VARIANT);
	}

	// Build DISPPARAMS
	dp.cArgs = cArgs;
	dp.rgvarg = pArgs;

	// Handle special-case for property-puts
	if (autoType & DISPATCH_PROPERTYPUT)
	{
		dp.cNamedArgs = 1;
		dp.rgdispidNamedArgs = &dispidNamed;
	}

	// Make the call
	hr = dispatch->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, autoType, &dp, varResult, NULL, NULL);
	delete [] pArgs;
	if (FAILED(hr))
		return false;

	// End variable-argument section
	va_end(marker);

	return true;
}

unsigned int MkExcelFileInterface::_GetLastBlankInBinarySearch
(unsigned int pivot, unsigned int start, unsigned int grid, unsigned int maxRange, bool rowMajor) const
{
	assert(start < maxRange);

	MkStr buffer;
	bool found = (rowMajor) ? GetData(start, pivot, buffer) : GetData(pivot, start, buffer);
	if (!found)
		return MKDEF_ARRAY_ERROR;

	if (buffer.Empty())
		return start;

	unsigned int beginPos = start;
	unsigned int endPos = beginPos + grid;
	grid *= 2;

	// grid의 배수 단위로 검사하여 beginPos(마지막으로 차 있는 셀)과 endPos(최초로 비어 있는 셀)을 결정
	while (true)
	{
		MkStr tmp;
		bool ok = (rowMajor) ? GetData(endPos, pivot, tmp) : GetData(pivot, endPos, tmp);
		if (!ok)
			return MKDEF_ARRAY_ERROR;

		if (!tmp.Empty())
		{
			if (endPos < maxRange)
			{
				beginPos = endPos;
				endPos += grid;
				grid *= 2;
				if (endPos >= maxRange)
				{
					endPos = maxRange;
				}
			}
			else
				return MKDEF_ARRAY_ERROR;
		}
		else
			break;
	}

	// 원하는 지점은 beginPos와 endPos 사이에 존재하므로 이진탐색
	while (true)
	{
		unsigned int currentPos = (beginPos + endPos) / 2;
		if (beginPos == currentPos)
			break;

		MkStr tmp;
		bool ok = (rowMajor) ? GetData(currentPos, pivot, tmp) : GetData(pivot, currentPos, tmp);
		if (!ok)
			return MKDEF_ARRAY_ERROR;

		if (tmp.Empty())
		{
			endPos = currentPos;
		}
		else
		{
			beginPos = currentPos;
		}
	}
	return (beginPos + 1);
}

//------------------------------------------------------------------------------------------------//