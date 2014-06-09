#ifndef __MINIKEY_CORE_MKEXCELTOMEMORYCONVERTER_H__
#define __MINIKEY_CORE_MKEXCELTOMEMORYCONVERTER_H__


//------------------------------------------------------------------------------------------------//
// excel -> memory
//
// 엑셀 파일을 파싱해 바이너리 형태로 전환
// MkDataTextToMemoryConverter와 동일하게 일부 문법 검사만 수행한 다음 키워드를 1:1로 변환하는
// 형태이므로 만들어진 데이터는 마찬가지로 최적화가 되지 않은 상태임
// 최적화 완료된 데이터를 원할 경우 노드로 로딩 한 후 memory로 변환 할 것
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStr.h"

class MkPathName;
class MkInterfaceForDataWriting;
class MkExcelFileReader;

class MkExcelToMemoryConverter
{
public:

	// text source file -> memory
	bool Convert(const MkPathName& filePath, MkByteArray& destination);

	void Clear(void);

	~MkExcelToMemoryConverter() { Clear(); }

protected:

	class _CellPos
	{
	public:
		inline _CellPos& operator = (const _CellPos& pos) { r = pos.r; c = pos.c; return *this; }
		inline _CellPos operator + (const _CellPos& pos) const { return _CellPos(r + pos.r, c + pos.c); }
		inline _CellPos& operator += (const _CellPos& pos) { r += pos.r; c += pos.c; return *this; }

		_CellPos() { r = 0; c = 0; }
		_CellPos(unsigned int ir, unsigned int ic) { r = ir; c = ic; }
		_CellPos(const _CellPos& pos) { r = pos.r; c = pos.c; }

	public:
		unsigned int r; // row
		unsigned int c; // column
	};

	typedef struct _SheetData
	{
		MkStr templateName; // table type sheet의 경우 자식 노드들에게 일괄적으로 적용될 템플릿명
		MkArray<int> type; // type index list(unit, node)
		MkArray<MkStr> key; // key list(type과 pair)
		MkArray<MkStr> name; // single : 비어 있음, table : node name list
		MkArray< MkArray< MkArray<MkStr> > > data; // data[name size][type size][value size]
	}
	SheetData;

protected:

	bool _BuildSheetData(MkExcelFileReader& reader);

	bool _GetBaseValue(MkExcelFileReader& reader, const MkStr& position, const MkArray<MkStr>& availableList, MkStr& result) const;

	bool _ParseTable(MkExcelFileReader& reader, const MkStr& type, const MkStr& anchor, const MkStr& startPosition, SheetData& dest);

	unsigned int _GetRowCount(const MkExcelFileReader& reader, const _CellPos& startPos) const;
	unsigned int _GetColumnCount(const MkExcelFileReader& reader, const _CellPos& startPos) const;

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
