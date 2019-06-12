#pragma once

//------------------------------------------------------------------------------------------------//
// M$ Excel 포맷인 xlsx 파일 IO 관리자
// ODBC를 이용해 단일 파일에 대한 access 허용
// http://support.microsoft.com/kb/216686/ 참조
//
// (NOTE) 반드시 사용 PC에 MS Excel이 설치되어 있어야 함
// (NOTE) SetUp이 성공했는데 Clear()하지 않은 경우 Excel 프로세스가 계속 OS에 남아 있음
//------------------------------------------------------------------------------------------------//

#include "ole2.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkHashMap.h"


class MkPathName;

class MkExcelFileInterface
{
public:

	// Excel 시작
	// 반환값은 성공여부
	// "_header"라는 이름의 sheet 하나로 시작
	bool SetUp(void);

	// Excel 시작 및 파일 열기
	// fullPath : root directory 기반 상대 혹은 절대 엑셀 파일 경로
	// 반환값은 성공여부
	bool SetUp(const MkPathName& filePath);

	// 해당 경로로 저장
	// fullPath : root directory 기반 상대 혹은 절대 엑셀 파일 경로
	// 반환값은 성공여부
	bool SaveAs(const MkPathName& filePath);

	// 현재 열려 있는 파일에 존재하는 시트 수 반환
	unsigned int GetSheetSize(void) const;

	// 새 시트 생성
	// (NOTE) 현재 작업대상 시트 뒤에 추가 됨
	bool CreateSheet(const MkHashStr& name);

	// 시트 이름 변경
	bool RenameSheet(const MkHashStr& lastName, const MkHashStr& newName);

	// name 이름의 시트 삭제
	// (NOTE) 최소 하나의 시트는 남김(하나인 경우 삭제 불가)
	// (NOTE) 현재 작업대상 시트인 경우 첫번째 시트가 작업 시트로 지정 됨
	bool DeleteSheet(const MkHashStr& name);

	// 번호로 작업대상 시트 지정
	// 반환값은 성공여부(시트 존재여부)
	bool SetActiveSheet(unsigned int index);

	// 시트명으로 작업대상 시트 지정
	// 반환값은 성공여부(시트 존재여부)
	bool SetActiveSheet(const MkHashStr& name);

	// 해당 시트 존재여부 반환
	bool SheetExist(const MkHashStr& name) const;

	// 번호에 해당하는 시트명 반환
	MkStr GetSheetName(unsigned int index) const;

	// column에서 startRow 지점부터 연속된 값이 존재한다고 가정할 때 마지막으로 존재하는 row 반환
	// MKDEF_ARRAY_ERROR가 반환되면 에러
	unsigned int GetLastBlankRow(unsigned int column, unsigned int startRow = 0) const;

	// row에서 startColumn 지점부터 연속된 값이 존재한다고 가정할 때 마지막으로 존재하는 column 반환
	// MKDEF_ARRAY_ERROR가 반환되면 에러
	unsigned int GetLastBlankColumn(unsigned int row, unsigned int startColumn = 0) const;

	// 해당 열의 폭 설정
	bool AutoFit(unsigned int column);

	// 값 입력
	// row, column은 좌상단에서 0으로 시작하는 인덱스
	// 반환값은 성공여부(해당 셀 존재여부)
	bool PutData(const MkStr& position, bool value) const; // 셀 이름(ex> A6, B5)
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

	bool PutData(unsigned int row, unsigned int column, bool value) const; // 행과 열
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

	// 값 반환
	// row, column은 좌상단에서 0으로 시작하는 인덱스
	// 반환값은 성공여부(해당 셀 존재여부)
	bool GetData(const MkStr& position, MkStr& buffer) const;
	bool GetData(unsigned int row, unsigned int column, MkStr& buffer) const;

	// 해당 셀의 스타일 설정
	bool SetBorder(const MkStr& position, int style);
	bool SetBorder(unsigned int row, unsigned int column, int style);

	// 종료. 파일 및 Excel 닫기
	void Clear(void);

	// 위치 변환
	static unsigned int ConvertColumn(const MkStr& column); // column keyword로 index를 얻음
	static MkStr ConvertColumn(unsigned int column); // column index로 keyword를 얻음
	static bool ConvertPosition(const MkStr& position, unsigned int& row, unsigned int& column); // 셀 이름으로 index를 얻음
	static bool ConvertPosition(const MkStr& position, MkUInt2& rc); // 셀 이름으로 index를 얻음
	static MkStr ConvertPosition(unsigned int row, unsigned int column); // index로 셀 이름을 얻음
	static MkStr ConvertPosition(const MkUInt2& rc); // index로 셀 이름을 얻음

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
