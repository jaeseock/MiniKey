
#include "MkCore_MkInterfaceForDataWriting.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkTagDefinitionForDataNode.h"
#include "MkCore_MkHelperForDataNodeConverter.h"

//------------------------------------------------------------------------------------------------//

class __DataNodeHelper
{
public:
	// 노드에서 정보 추출
	static void CountNodeProperty
		(const MkDataNode& targetNode,
		unsigned int& nodeCount, unsigned int& templateCount, unsigned int& unitCount, unsigned int (&valueCount)[ePDT_MaxCount])
	{
		++nodeCount; // target node 자체

		MkHashStr templateName;
		if (targetNode.GetAppliedTemplateName(templateName))
		{
			++templateCount; // 템플릿 적용된 노드
		}

		MkArray<MkHashStr> keyList;
		unitCount += targetNode.GetUnitKeyList(keyList); // 노드 직계 유닛 수

		MK_INDEXING_LOOP(keyList, i) // 유닛들에 속한 타입별 값 수
		{
			const MkHashStr& key = keyList[i];
			ePrimitiveDataType unitType = targetNode.GetUnitType(key);
			valueCount[unitType] += targetNode.GetDataSize(key);
		}

		MkArray<MkHashStr> childNodeList;
		if (targetNode.GetChildNodeList(childNodeList) > 0)
		{
			MK_INDEXING_LOOP(childNodeList, i) // 자식 노드 순환하며 재귀호출
			{
				const MkDataNode* childNode = targetNode.GetChildNode(childNodeList[i]);
				assert(childNode != NULL);
				CountNodeProperty(*childNode, nodeCount, templateCount, unitCount, valueCount);
			}
		}
	}
};

//------------------------------------------------------------------------------------------------//

void MkHelperForDataNodeConverter::ReserveInterface(const MkStr& source, MkInterfaceForDataWriting& dwInterface)
{
	unsigned int nodeCount = source.CountKeyword(MkTagDefinitionForDataNode::TagForBlockEnd); // 노드 수
	unsigned int templateCount = source.CountKeyword(MkTagDefinitionForDataNode::TagForApplyTemplate); // 템플릿 적용된 노드 수

	unsigned int unitCount = 0; // unit 수
	unsigned int valueCount[ePDT_MaxCount]; // type별 value 수
	for (int i=ePDT_IndexBegin; i<ePDT_IndexEnd; ++i)
	{
		valueCount[i] = 0;
		const MkStr& currentTag = MkPrimitiveDataType::GetTag(static_cast<ePrimitiveDataType>(i));
		unsigned int currentPos = 0;

		while (true)
		{
			MkStr buffer;
			currentPos = source.GetFirstBlock(currentPos, currentTag, MkTagDefinitionForDataNode::TagForUnitEnd, buffer);
			if (currentPos != MKDEF_ARRAY_ERROR)
			{
				++unitCount;
				valueCount[i] += (buffer.CountKeyword(MkTagDefinitionForDataNode::TagForArrayDivider) + 1);
			}
			else
				break;
		}
	}

	ReserveInterface(nodeCount, templateCount, unitCount, valueCount, dwInterface);
}

void MkHelperForDataNodeConverter::ReserveInterface(const MkDataNode& source, MkInterfaceForDataWriting& dwInterface)
{
	unsigned int nodeCount = 0;
	unsigned int templateCount = 0;
	unsigned int unitCount = 0;
	unsigned int valueCount[ePDT_MaxCount];
	for (int i=ePDT_IndexBegin; i<ePDT_IndexEnd; ++i)
	{
		valueCount[i] = 0;
	}

	__DataNodeHelper::CountNodeProperty(source, nodeCount, templateCount, unitCount, valueCount);
	ReserveInterface(nodeCount, templateCount, unitCount, valueCount, dwInterface);
}

void MkHelperForDataNodeConverter::ReserveInterface
(unsigned int nodeCount, unsigned int templateCount, unsigned int unitCount, const unsigned int (&valueCount)[ePDT_MaxCount],
 MkInterfaceForDataWriting& dwInterface)
{
	unsigned int inputCount[ePDT_MaxCount]; // type별 input 수
	for (int i=ePDT_IndexBegin; i<ePDT_IndexEnd; ++i)
	{
		inputCount[i] = 0;
	}

	// for node
	inputCount[ePDT_Int] += nodeCount; // node type
	inputCount[ePDT_Str] += nodeCount; // node name
	inputCount[ePDT_Bool] += nodeCount; // template applied
	inputCount[ePDT_Str] += templateCount; // template name
	inputCount[ePDT_Int] += nodeCount; // end of node tag

	// for template pushing
	++inputCount[ePDT_Int]; // tag
	++inputCount[ePDT_Str]; // template name

	// for unit
	inputCount[ePDT_Int] += unitCount; // unit type
	inputCount[ePDT_Str] += unitCount; // unit name
	inputCount[ePDT_UnsignedInt] += unitCount; // value count
	for (int i=ePDT_IndexBegin; i<ePDT_IndexEnd; ++i)
	{
		unsigned int totalCount = inputCount[i] + valueCount[i];
		if (totalCount > 0)
		{
			dwInterface.SetInputSize(static_cast<ePrimitiveDataType>(i), totalCount);
		}
	}

	dwInterface.UpdateInputSize();
}

void MkHelperForDataNodeConverter::WriteUnitString(ePrimitiveDataType unitType, const MkStr& unitStr, MkInterfaceForDataWriting& dwInterface)
{
	switch (unitType)
	{
	case ePDT_Bool: dwInterface.Write(unitStr.ToBool()); break;
	case ePDT_Int: dwInterface.Write(unitStr.ToInteger()); break;
	case ePDT_UnsignedInt: dwInterface.Write(unitStr.ToUnsignedInteger()); break;
	case ePDT_DInt: dwInterface.Write(unitStr.ToDoubleInteger()); break;
	case ePDT_DUnsignedInt: dwInterface.Write(unitStr.ToDoubleUnsignedInteger()); break;
	case ePDT_Float: dwInterface.Write(unitStr.ToFloat()); break;
	case ePDT_Int2: dwInterface.Write(unitStr.ToInt2()); break;
	case ePDT_Vec2: dwInterface.Write(unitStr.ToVec2()); break;
	case ePDT_Vec3: dwInterface.Write(unitStr.ToVec3()); break;
	case ePDT_Str: dwInterface.Write(unitStr); break;
	case ePDT_ByteArray:
		{
			MkByteArray buffer;
			unitStr.ExportByteArray(buffer);
			dwInterface.Write(buffer);
		}
		break;
	}
}

//------------------------------------------------------------------------------------------------//
