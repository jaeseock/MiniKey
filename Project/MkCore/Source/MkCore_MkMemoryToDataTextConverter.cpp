
#include "MkCore_MkStr.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataTypeDefinition.h"
#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkInterfaceForDataReading.h"
#include "MkCore_MkTagDefinitionForDataNode.h"
#include "MkCore_MkMemoryToDataTextConverter.h"


// binary -> text 변환시 용량이 늘어나기는 하지만 20배를 넘기기는 힘들거라 예상
#define MKDEF_M_TO_T_RESERVATION 20

// 기본 앵커는 tab
#define MKDEF_M_TO_T_ANCHOR_TYPE L"\t"

// 기본 공백문자는 space
#define MKDEF_M_TO_T_BLANK_TYPE L" "

//------------------------------------------------------------------------------------------------//

template <class DataType>
class __TSI_AssignUnitToText
{
public:
	static bool Proceed(MkInterfaceForDataReading& drInterface, MkArray<MkStr>& dataList, const MkStr& unitName)
	{
		unsigned int count;
		MK_CHECK(drInterface.Read(count), unitName + L" 유닛 값 갯수 읽기 오류")
			return false;

		MK_CHECK(count > 0, unitName + L" 유닛 값 갯수가 0임")
			return false;

		dataList.Reserve(count);
		for (unsigned int i=0; i<count; ++i)
		{
			DataType buffer;
			MK_CHECK(drInterface.Read(buffer), unitName + L" 유닛 " + MkStr(i) + L"번째 값 읽기 오류")
				return false;

			dataList.PushBack(MkStr(buffer));
		}
		return true;
	}
};

//------------------------------------------------------------------------------------------------//

bool MkMemoryToDataTextConverter::Convert(const MkByteArray& srcArray, const MkPathName& filePath)
{
	MkInterfaceForDataReading drInterface;
	MK_CHECK(drInterface.SetUp(srcArray, 0), L"MkInterfaceForDataReading 초기화 실패")
		return false;

	MkStr strBuffer;
	strBuffer.Reserve(srcArray.GetSize() * MKDEF_M_TO_T_RESERVATION);

	_InitiateHeaderMsg(strBuffer, filePath);

	if (_BuildText(drInterface, 0, strBuffer))
	{
		MK_CHECK(strBuffer.WriteToTextFile(filePath, true), MkStr(filePath) + L"경로에 파일 쓰기 오류")
			return false;
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------//

void MkMemoryToDataTextConverter::_InitiateHeaderMsg(MkStr& strBuffer, const MkPathName& filePath) const
{
	// 헤더 구성
	strBuffer += L"\n";
	strBuffer += L"//--------------------------------------------------------------------//\n";
	strBuffer += L"// MiniKey data node text source\n";
	strBuffer += L"//   - file path  : " + MkStr(filePath) + L"\n";
	strBuffer += L"//   - time stamp : " + MK_SYS_ENV.GetCurrentSystemDate() + L" (" + MK_SYS_ENV.GetCurrentSystemTime() + L"),\n";
	strBuffer += L"//   - exporter   : " + MK_SYS_ENV.GetCurrentLogOnUserName() + L" (" + MkPathName::GetApplicationName() + L")\n";
	strBuffer += L"//--------------------------------------------------------------------//\n\n";
}

bool MkMemoryToDataTextConverter::_BuildText(MkInterfaceForDataReading& drInterface, unsigned int nodeDepth, MkStr& strBuffer) const
{
	unsigned int msg = MKDEF_DNC_MSG_CONTINUE;
	while (msg == MKDEF_DNC_MSG_CONTINUE)
	{
		msg = _BuildBlock(drInterface, nodeDepth, strBuffer);
	}
	return (msg == MKDEF_DNC_MSG_COMPLETE);
}

unsigned int MkMemoryToDataTextConverter::_BuildBlock(MkInterfaceForDataReading& drInterface, unsigned int nodeDepth, MkStr& strBuffer) const
{
	// 타입
	// 더 이상의 정보가 없을 경우 종료
	int blockType;
	if (!drInterface.Read(blockType))
		return MKDEF_DNC_MSG_COMPLETE;

	// 노드 종료 태그이면 탈출
	if (blockType == MkTagDefinitionForDataNode::IndexMarkForNodeBlockEnd)
		return MKDEF_DNC_MSG_COMPLETE;

	// 이름
	MkStr blockName;
	MK_CHECK(drInterface.Read(blockName) && (!blockName.Empty()), L"block name 읽기 오류")
		return MKDEF_DNC_MSG_ERROR;

	// 앵커
	static const MkStr blankStr = MKDEF_M_TO_T_ANCHOR_TYPE;
	MkStr anchor = L"\n";
	if (nodeDepth > 0)
	{
		anchor.Reserve(blankStr.GetSize() * nodeDepth);
		for (unsigned int i=0; i<nodeDepth; ++i)
		{
			anchor += blankStr;
		}
	}

	// 노드
	if ((blockType == MkTagDefinitionForDataNode::IndexMarkForNodeBegin) || (blockType == MkTagDefinitionForDataNode::IndexMarkForTemplateBegin))
	{
		// 템플릿 적용여부
		bool templateApplied;
		MK_CHECK(drInterface.Read(templateApplied), blockName + L" 노드에서 templateApplied 정보값 읽기 오류")
			return MKDEF_DNC_MSG_ERROR;

		// 템플릿 이름
		MkStr templateName;
		if (templateApplied)
		{
			MK_CHECK(drInterface.Read(templateName), blockName + L" 노드에서 적용된 템플릿 이름 읽기 오류")
				return MKDEF_DNC_MSG_ERROR;
		}

		// 기록
		if (!strBuffer.CheckPostfix(MkTagDefinitionForDataNode::TagForBlockBegin))
		{
			strBuffer += L"\n"; // 노드 출력시에는 최초 노드가 아니면 한 줄의 공백을 둠
		}
		
		strBuffer += anchor;
		strBuffer += (blockType == MkTagDefinitionForDataNode::IndexMarkForNodeBegin) ?
			MkTagDefinitionForDataNode::TagForNode : MkTagDefinitionForDataNode::TagForTemplate;
		strBuffer += MKDEF_M_TO_T_BLANK_TYPE;
		strBuffer += L"\"";
		strBuffer += blockName;
		strBuffer += L"\"";
		if (templateApplied)
		{
			strBuffer += MKDEF_M_TO_T_BLANK_TYPE;
			strBuffer += MkTagDefinitionForDataNode::TagForApplyTemplate;
			strBuffer += MKDEF_M_TO_T_BLANK_TYPE;
			strBuffer += L"\"";
			strBuffer += templateName;
			strBuffer += L"\"";
		}
		strBuffer += anchor;
		strBuffer += MkTagDefinitionForDataNode::TagForBlockBegin;

		unsigned int initStrSize = strBuffer.GetSize();

		// body 부분을 depth를 추가해 계속 구성
		if (!_BuildText(drInterface, nodeDepth + 1, strBuffer))
			return MKDEF_DNC_MSG_ERROR;

		if (strBuffer.GetSize() == initStrSize) // 새로 추가된 정보가 없는 빈 노드
		{
			strBuffer.BackSpace(anchor.GetSize() + MkTagDefinitionForDataNode::TagForBlockBegin.GetSize());
			strBuffer += MKDEF_M_TO_T_BLANK_TYPE;
			strBuffer += MkTagDefinitionForDataNode::TagForBlockBegin;
		}
		else
		{
			strBuffer += anchor;
		}
		strBuffer += MkTagDefinitionForDataNode::TagForBlockEnd;
	}
	// 템플릿 밀어넣기
	else if (blockType == MkTagDefinitionForDataNode::IndexMarkForPushingTemplate)
	{
		strBuffer += anchor;
		strBuffer += MkTagDefinitionForDataNode::TagForPushingTemplate;
		strBuffer += MKDEF_M_TO_T_BLANK_TYPE;
		strBuffer += L"\"";
		strBuffer += blockName;
		strBuffer += L"\"";
		strBuffer += MkTagDefinitionForDataNode::TagForUnitEnd;
	}
	// 유닛
	else
	{
		ePrimitiveDataType unitType = static_cast<ePrimitiveDataType>(blockType);
		MK_CHECK(MkPrimitiveDataType::IsValid(unitType), blockName + L" 유닛 타입은 허용되지 않음 : " + MkStr(blockType))
			return MKDEF_DNC_MSG_ERROR;

		bool ok = false;
		MkArray<MkStr> dataList;
		switch (unitType)
		{
		case ePDT_Bool: ok = __TSI_AssignUnitToText<bool>::Proceed(drInterface, dataList, blockName); break;
		case ePDT_Int: ok = __TSI_AssignUnitToText<int>::Proceed(drInterface, dataList, blockName); break;
		case ePDT_UnsignedInt: ok = __TSI_AssignUnitToText<unsigned int>::Proceed(drInterface, dataList, blockName); break;
		case ePDT_Float: ok = __TSI_AssignUnitToText<float>::Proceed(drInterface, dataList, blockName); break;
		case ePDT_Int2: ok = __TSI_AssignUnitToText<MkInt2>::Proceed(drInterface, dataList, blockName); break;
		case ePDT_Vec2: ok = __TSI_AssignUnitToText<MkVec2>::Proceed(drInterface, dataList, blockName); break;
		case ePDT_Vec3: ok = __TSI_AssignUnitToText<MkVec3>::Proceed(drInterface, dataList, blockName); break;
		case ePDT_Str:
			{
				ok = __TSI_AssignUnitToText<MkStr>::Proceed(drInterface, dataList, blockName);

				// 문자열의 경우 추가적인 처리 필요
				if (ok)
				{
					MK_INDEXING_LOOP(dataList, i)
					{
						// 탈출문자(escape sequence) 반영
						MkStr tmp = dataList[i];
						tmp.ReplaceKeyword(L"\"", MkTagDefinitionForDataNode::TagForDQM);
						tmp.ReplaceKeyword(MkTagDefinitionForDataNode::TagForDQM, L"\\\"");
						tmp.ReplaceCRtoTag();

						// DQM으로 둘러쌈
						dataList[i].Reserve(tmp.GetSize() + 2);
						dataList[i] = L"\"";
						dataList[i] += tmp;
						dataList[i] += L"\"";
					}
				}
			}
			break;
		}
		if (!ok)
			return MKDEF_DNC_MSG_ERROR;

		// 기록
		strBuffer += anchor;
		strBuffer += MkPrimitiveDataType::GetTag(unitType);
		strBuffer += MKDEF_M_TO_T_BLANK_TYPE;
		strBuffer += blockName;
		strBuffer += MKDEF_M_TO_T_BLANK_TYPE;
		strBuffer += MkTagDefinitionForDataNode::TagForUnitAssign;
		
		unsigned int dataSize = dataList.GetSize();
		if (dataSize == 1)
		{
			strBuffer += MKDEF_M_TO_T_BLANK_TYPE;
			strBuffer += dataList[0];
		}
		else
		{
			strBuffer += MKDEF_M_TO_T_BLANK_TYPE; // size를 주석으로 추가
			strBuffer += L"// [";
			strBuffer += dataSize;
			strBuffer += L"]";

			MkStr arrayAnchor = anchor + MKDEF_M_TO_T_ANCHOR_TYPE;
			unsigned int lastIndex = dataSize - 1;
			MK_INDEXING_LOOP(dataList, i)
			{
				bool lineFeed = ((unitType == ePDT_Str) || ((i % 5) == 0)); // line feed는 문자열이거나 5개 단위로 이루어짐
				strBuffer += (lineFeed) ? arrayAnchor : MKDEF_M_TO_T_BLANK_TYPE;
				strBuffer += dataList[i];
				if (i < lastIndex)
				{
					strBuffer += MKDEF_M_TO_T_BLANK_TYPE;
					strBuffer += MkTagDefinitionForDataNode::TagForArrayDivider;
				}
			}
		}

		strBuffer += MkTagDefinitionForDataNode::TagForUnitEnd;
	}

	return MKDEF_DNC_MSG_CONTINUE;
}
