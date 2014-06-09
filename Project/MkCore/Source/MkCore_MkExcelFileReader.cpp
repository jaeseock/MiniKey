
#include <comdef.h>
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkExcelFileReader.h"


//------------------------------------------------------------------------------------------------//

bool MkExcelFileReader::SetUp(const MkPathName& filePath)
{
	MK_CHECK(m_EnableCOM, L"COM 초기화가 되지 않아 MkExcelFileReader 사용 불가")
		return false;

	MkPathName fullPath;
	fullPath.ConvertToRootBasisAbsolutePath(filePath);

	MK_CHECK(fullPath.CheckAvailable(), filePath + L" 경로가 올바르지 않음")
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
	VARIANT frameResult;
	VariantInit(&frameResult);
	MK_CHECK(_AutoWrap(DISPATCH_PROPERTYGET, &frameResult, m_Property.applicationPtr, L"Workbooks", 0), L"Excel mainframe 로딩 실패")
		return false;
	
	m_Property.mainframePtr = frameResult.pdispVal;

	// finally
	m_EnableExcel = true;

	// open file
	VARIANT openResult;
	VariantInit(&openResult);
	_bstr_t pathBuf = fullPath;
	VARIANT vaPath;
	vaPath.vt = VT_BSTR;
	vaPath.bstrVal = SysAllocString(pathBuf);

	MK_CHECK(_AutoWrap(DISPATCH_METHOD, &openResult, m_Property.mainframePtr, L"Open", 1, vaPath), filePath + L" 오픈 실패")
		return false;

	m_Property.workSheetPtr = openResult.pdispVal;
	VariantClear(&vaPath);

	// get name list
	unsigned int currIndex = 0;
	while (true)
	{
		if (SetActiveSheet(currIndex))
		{
			VARIANT result;
			VariantInit(&result);
			if (!_AutoWrap(DISPATCH_PROPERTYGET, &result, m_Property.currentSheetPtr, L"name", 0))
				return false;

			m_SheetNameList.PushBack(MkStr(result.bstrVal));
			++currIndex;
		}
		else
			break;
	}

	MK_CHECK(!m_SheetNameList.Empty(), filePath + L"에 아무런 시트도 존재하지 않음")
		return false;

	return true;
}

bool MkExcelFileReader::SetActiveSheet(unsigned int index)
{
	if (!m_EnableExcel)
		return false;

	VARIANT result;
	VariantInit(&result);

	VARIANT vaSheet;
	vaSheet.vt = VT_I4;
	vaSheet.lVal = static_cast<LONG>(index) + 1;

	if (!_AutoWrap(DISPATCH_PROPERTYGET, &result, m_Property.applicationPtr, L"Worksheets", 1, vaSheet))
		return false;

	m_Property.currentSheetPtr = result.pdispVal;
	return true;
}

bool MkExcelFileReader::SetActiveSheet(const MkStr& name)
{
	if (!m_EnableExcel)
		return false;

	if (name.Empty())
		return false;

	if (!m_SheetNameList.Exist(MkArraySection(0), name))
		return false;

	VARIANT result;
	VariantInit(&result);

	_bstr_t nameBuf = name.GetPtr();
	VARIANT vaSheet;
	vaSheet.vt = VT_BSTR;
	vaSheet.bstrVal = SysAllocString(nameBuf);

	if (!_AutoWrap(DISPATCH_PROPERTYGET, &result, m_Property.applicationPtr, L"Worksheets", 1, vaSheet))
		return false;

	VariantClear(&vaSheet);
	m_Property.currentSheetPtr = result.pdispVal;
	return true;
}

bool MkExcelFileReader::SheetExist(const MkStr& name) const
{
	return m_SheetNameList.Exist(MkArraySection(0), name);
}

MkStr MkExcelFileReader::GetSheetName(unsigned int index) const
{
	return m_SheetNameList.IsValidIndex(index) ? m_SheetNameList[index] : L"";
}

unsigned int MkExcelFileReader::GetLastBlankRow(unsigned int column, unsigned int startRow) const
{
	return _GetLastBlankInBinarySearch(column, startRow, 1, 16383, true);
}

unsigned int MkExcelFileReader::GetLastBlankColumn(unsigned int row, unsigned int startColumn) const
{
	return _GetLastBlankInBinarySearch(row, startColumn, 1, 17575, false);
}

bool MkExcelFileReader::GetData(const MkStr& position, MkStr& buffer) const
{
	if (position.Empty() || (!m_EnableExcel) || (m_Property.currentSheetPtr == NULL))
		return false;

	buffer.Clear();

	// range
	MkStr posBuf = position + L":" + position;
	_bstr_t rangeBuf = posBuf.GetPtr();
	VARIANT parm;
	parm.vt = VT_BSTR;
	parm.bstrVal = SysAllocString(rangeBuf);
	VARIANT result;
	VariantInit(&result);
	_AutoWrap(DISPATCH_PROPERTYGET, &result, m_Property.currentSheetPtr, L"Range", 1, parm);
	VariantClear(&parm);
	IDispatch* targetRange = result.pdispVal;
	if (targetRange == NULL)
		return true; // 범위를 벗어남. 셀은 존재하지만 값이 없는 것으로 처리(VT_EMPTY와 동일)
	
	// get
	VARIANT tmp;
	VariantInit(&tmp);
	_AutoWrap(DISPATCH_PROPERTYGET, &tmp, targetRange, L"Value", 0, 0);
	targetRange->Release();
	if (tmp.vt != VT_EMPTY)
	{
		VariantChangeType(&tmp, &tmp, VARIANT_NOUSEROVERRIDE, VT_BSTR);
		buffer = static_cast<const wchar_t*>(tmp.bstrVal);
	}
	VariantClear(&tmp);
	return true;
}

bool MkExcelFileReader::GetDataRC(unsigned int row, unsigned int column, MkStr& buffer) const
{
	if (!m_EnableExcel)
		return false;

	MkStr posName = ConvertPosition(row, column);
	return GetData(posName, buffer);
}

bool MkExcelFileReader::GetDataCR(unsigned int column, unsigned int row, MkStr& buffer) const
{
	return GetDataRC(row, column, buffer);
}

void MkExcelFileReader::Clear(void)
{
	if (m_EnableExcel)
	{
		_AutoWrap(DISPATCH_METHOD, NULL, m_Property.applicationPtr, L"Quit", 0);
		m_Property.currentSheetPtr->Release();
		m_Property.workSheetPtr->Release();
		m_Property.mainframePtr->Release();
		m_Property.applicationPtr->Release();
		m_SheetNameList.Clear();
		m_EnableExcel = false;
	}
}

unsigned int MkExcelFileReader::ConvertColumn(const MkStr& column)
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

MkStr MkExcelFileReader::ConvertColumn(unsigned int column)
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

bool MkExcelFileReader::ConvertPosition(const MkStr& position, unsigned int& row, unsigned int& column)
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

MkStr MkExcelFileReader::ConvertPosition(unsigned int row, unsigned int column)
{
	MkStr position = ConvertColumn(column);
	position += row + 1;
	return position;
}

MkExcelFileReader::MkExcelFileReader()
{
	HRESULT hr = CoInitialize(NULL);
	m_EnableCOM = ((hr == S_OK) || (hr == S_FALSE));
	m_EnableExcel = false;
}

MkExcelFileReader::~MkExcelFileReader()
{
	Clear();

	if (m_EnableCOM)
	{
		CoUninitialize();
	}
}

//------------------------------------------------------------------------------------------------//

bool MkExcelFileReader::_AutoWrap(int autoType, VARIANT* varResult, IDispatch* dispatch, LPOLESTR name, int cArgs...) const
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

unsigned int MkExcelFileReader::_GetLastBlankInBinarySearch
(unsigned int pivot, unsigned int start, unsigned int grid, unsigned int maxRange, bool rowMajor) const
{
	assert(start < maxRange);

	MkStr buffer;
	bool found = (rowMajor) ? GetDataRC(start, pivot, buffer) : GetDataRC(pivot, start, buffer);
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
		bool ok = (rowMajor) ? GetDataRC(endPos, pivot, tmp) : GetDataRC(pivot, endPos, tmp);
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

	// 원하는 지점은 beginPos와 endPos 사이에 존재하므로 binary search
	while (true)
	{
		unsigned int currentPos = (beginPos + endPos) / 2;
		if (beginPos == currentPos)
			break;

		MkStr tmp;
		bool ok = (rowMajor) ? GetDataRC(currentPos, pivot, tmp) : GetDataRC(pivot, currentPos, tmp);
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