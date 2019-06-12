#pragma once


//------------------------------------------------------------------------------------------------//
// data node -> source excel
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"

class MkStr;
class MkPathName;
class MkHashStr;
class MkDataNode;

class MkDataNodeToExcelConverter
{
public:

	// data node -> excel source file
	bool Convert(const MkDataNode& source, const MkPathName& filePath) const;

protected:

	bool _PutStringData(const MkUInt2& pos, const MkStr& value, MkUInt2& minCell, MkUInt2& maxCell, MkExcelFileInterface& excelWritter) const;
	bool _WriteHeader(const MkStr& pos, const MkStr& desc, const MkStr& value, MkUInt2& minCell, MkUInt2& maxCell, MkExcelFileInterface& excelWritter) const;
	bool _WriteNodeTag(const MkUInt2& pos, const MkUInt2& dir, const MkHashStr& childName, const MkDataNode& parentNode, MkHashStr& sheetName, MkUInt2& minCell, MkUInt2& maxCell, MkExcelFileInterface& excelWritter) const;

	void _GetSheetName(const MkHashStr& nodeName, MkExcelFileInterface& excelWritter, MkHashStr& buffer) const;

	bool _BuildSheet(const MkDataNode& node, const MkHashStr& sheetName, MkExcelFileInterface& excelWritter) const;
};

//------------------------------------------------------------------------------------------------//
