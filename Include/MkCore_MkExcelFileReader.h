#ifndef __MINIKEY_CORE_MKEXCELFILEREADER_H__
#define __MINIKEY_CORE_MKEXCELFILEREADER_H__

//------------------------------------------------------------------------------------------------//
// M$ Excel 포맷인 xlsx 파일 읽기 관리자
// ODBC를 이용해 단일 파일에 대한 access 허용
// http://support.microsoft.com/kb/216686/ 참조
//
// 저장 기능을 추가하고 싶으면 http://alones.kr/blog/i/entry/724 참조
//
// (NOTE) 반드시 사용 PC에 MS Excel이 설치되어 있어야 함
// (NOTE) SetUp이 성공했는데 Clear()하지 않은 경우 Excel 프로세스가 계속 OS에 남아 있음
//------------------------------------------------------------------------------------------------//

#include "ole2.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkArray.h"


class MkPathName;

class MkExcelFileReader
{
public:

	// Excel 시작 및 파일 열기
	// fullPath : root directory 기반 상대 혹은 절대 엑셀 파일 경로
	// 반환값은 성공여부
	bool SetUp(const MkPathName& filePath);

	// 현재 열려 있는 파일에 존재하는 시트 수 반환
	inline unsigned int GetSheetSize(void) const { return m_SheetNameList.GetSize(); }

	// 번호로 작업대상 시트 지정
	// 반환값은 성공여부(시트 존재여부)
	bool SetActiveSheet(unsigned int index);

	// 시트명으로 작업대상 시트 지정
	// 반환값은 성공여부(시트 존재여부)
	bool SetActiveSheet(const MkStr& name);

	// 해당 시트 존재여부 반환
	bool SheetExist(const MkStr& name) const;

	// 번호에 해당하는 시트명 반환
	MkStr GetSheetName(unsigned int index) const;

	// column에서 startRow 지점부터 연속된 값이 존재한다고 가정할 때 마지막으로 존재하는 row 반환
	// MKDEF_ARRAY_ERROR가 반환되면 에러
	unsigned int GetLastBlankRow(unsigned int column, unsigned int startRow = 0) const;

	// row에서 startColumn 지점부터 연속된 값이 존재한다고 가정할 때 마지막으로 존재하는 column 반환
	// MKDEF_ARRAY_ERROR가 반환되면 에러
	unsigned int GetLastBlankColumn(unsigned int row, unsigned int startColumn = 0) const;

	// 값 반환
	// row, column은 좌상단에서 0으로 시작하는 인덱스
	// 반환값은 성공여부(해당 셀 존재여부)
	bool GetData(const MkStr& position, MkStr& buffer) const; // 셀 이름(ex> A6, B5)에서 반환
	bool GetDataRC(unsigned int row, unsigned int column, MkStr& buffer) const; // 행과 열에서 반환(YX)
	bool GetDataCR(unsigned int column, unsigned int row, MkStr& buffer) const; // 열과 행에서 반환(XY)

	// 종료. 파일 및 Excel 닫기
	void Clear(void);

	// 위치 변환
	static unsigned int ConvertColumn(const MkStr& column); // column keyword로 index를 얻음
	static MkStr ConvertColumn(unsigned int column); // column index로 keyword를 얻음
	static bool ConvertPosition(const MkStr& position, unsigned int& row, unsigned int& column); // 셀 이름으로 index를 얻음
	static MkStr ConvertPosition(unsigned int row, unsigned int column); // index로 셀 이름을 얻음

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
