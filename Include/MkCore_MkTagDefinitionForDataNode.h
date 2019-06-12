#ifndef __MINIKEY_CORE_MKTAGDEFINITIONFORDATANODE_H__
#define __MINIKEY_CORE_MKTAGDEFINITIONFORDATANODE_H__

//------------------------------------------------------------------------------------------------//
// data node source text 파싱을 위한 키워드 정의
// MkDataType의 정의와 함께 텍스트 문법을 구성
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStr.h"

//------------------------------------------------------------------------------------------------//

#define MKDEF_DNC_MSG_CONTINUE 0
#define MKDEF_DNC_MSG_COMPLETE 0xfffffffe
#define MKDEF_DNC_MSG_ERROR 0xffffffff

#define MKDEF_EXCEL_SHEET_TYPE_POS L"B1"
#define MKDEF_EXCEL_SHEET_ANCHOR_POS L"B2"
#define MKDEF_EXCEL_SHEET_STARTPOSITION_POS L"B3"
#define MKDEF_EXCEL_SHEET_DEF_STARTPOSITION L"C6"

#define MKDEF_EXCEL_SHEET_COMMENT_TAG L"_"

#define MKDEF_EXCEL_SHEET_ROOT_NAME L"ROOT"

#define MKDEF_EXCEL_SHEET_SINGLE_TYPE L"Single"
#define MKDEF_EXCEL_SHEET_TABLE_TYPE L"Table"

#define MKDEF_EXCEL_SHEET_VERTICAL_ANCHOR L"Vertical"
#define MKDEF_EXCEL_SHEET_HORIZONTAL_ANCHOR L"Horizontal"

//------------------------------------------------------------------------------------------------//

class MkTagDefinitionForDataNode
{
public:

	static const MkStr TagForNode; // L"Node"
	static const int IndexMarkForNodeBegin; // MkDEF_NODE_BEGIN_INDEX

	static const MkStr TagForTemplate; // L"Template"
	static const int IndexMarkForTemplateBegin; // MkDEF_TEMPLATE_BEGIN_INDEX

	static const MkStr TagForPushingTemplate; // L"PushTemplate"
	static const int IndexMarkForPushingTemplate; // MkDEF_PUSHING_TEMPLATE_INDEX

	static const int IndexMarkForNodeBlockEnd; // MkDEF_BLOCK_END_INDEX

	static const MkStr TagForDQM; // L"\\\"" <-> L"__#DQM"

	static const MkStr TagForBlockBegin; // L"{"
	static const MkStr TagForBlockEnd; // L"}"
	static const MkStr TagForApplyTemplate; // L":"
	static const MkStr TagForUnitAssign; // L"="
	static const MkStr TagForUnitEnd; // L";"
	static const MkStr TagForArrayDivider; // L"/"

	static const MkStr StringArrayAutomationKeyword; // L"~"

	//
	static const MkByteArray TagForBinaryDataNode; // "#BDN"
};

//------------------------------------------------------------------------------------------------//

#endif