
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataTypeDefinition.h"
#include "MkCore_MkInterfaceForDataWriting.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkTagDefinitionForDataNode.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkHelperForDataNodeConverter.h"
#include "MkCore_MkDataNodeToMemoryConverter.h"

//------------------------------------------------------------------------------------------------//

template <class DataType>
class __TSI_AssignUnitDataToMemory
{
public:
	static void Proceed(const MkDataNode& targetNode, const MkHashStr& unitName, MkInterfaceForDataWriting& dwInterface)
	{
		MkArray<DataType> dataList;
		targetNode.GetData(unitName, dataList);
		MK_INDEXING_LOOP(dataList, i)
		{
			dwInterface.Write(dataList[i]);
		}
	}
};

//------------------------------------------------------------------------------------------------//

bool MkDataNodeToMemoryConverter::Convert(const MkDataNode& source, MkByteArray& destination) const
{
	MkInterfaceForDataWriting dwInterface;
	MkHelperForDataNodeConverter::ReserveInterface(source, dwInterface);

	if (source.__IsValidNode())
	{
		_ConvertUnits(source, dwInterface);
		_ConvertChildNodes(source, dwInterface);
	}

	dwInterface.Flush(destination);
	return true;
}

bool MkDataNodeToMemoryConverter::Convert(const MkDataNode& source, const MkPathName& filePath) const
{
	MkByteArray buffer;
	if (!Convert(source, buffer))
		return false;

	MkInterfaceForFileWriting fwInterface;
	MK_CHECK(fwInterface.SetUp(filePath, true, true), MkStr(filePath) + L" 경로 파일 열기 실패")
		return false;

	fwInterface.Write(MkTagDefinitionForDataNode::TagForBinaryDataNode, MkArraySection(0)); // binary tag
	fwInterface.Write(buffer, MkArraySection(0));
	fwInterface.Clear();
	return true;
}

//------------------------------------------------------------------------------------------------//

void MkDataNodeToMemoryConverter::_ParseNode(const MkDataNode& targetNode, MkInterfaceForDataWriting& dwInterface) const
{
	// 블록 헤더 정보를 메모리에 기록
	int nodeType = targetNode.IsTemplateNode() ? MkTagDefinitionForDataNode::IndexMarkForTemplateBegin : MkTagDefinitionForDataNode::IndexMarkForNodeBegin;
	dwInterface.Write(nodeType); // node type
	dwInterface.Write(MkStr(targetNode.GetNodeName())); // node name

	MkHashStr templateName;
	if (targetNode.GetAppliedTemplateName(templateName))
	{
		dwInterface.Write(true); // template applied
		dwInterface.Write(MkStr(templateName)); // template name
	}
	else
	{
		dwInterface.Write(false); // normal node
	}

	// unit 정보
	_ConvertUnits(targetNode, dwInterface);
	
	// 자식 노드 정보
	_ConvertChildNodes(targetNode, dwInterface);

	dwInterface.Write(MkTagDefinitionForDataNode::IndexMarkForNodeBlockEnd); // end of node
}

void MkDataNodeToMemoryConverter::_ConvertUnits(const MkDataNode& targetNode, MkInterfaceForDataWriting& dwInterface) const
{
	MkArray<MkHashStr> unitKeyList;
	targetNode.__GetValidUnitList(unitKeyList);
	unitKeyList.SortInAscendingOrder();
	MK_INDEXING_LOOP(unitKeyList, i) // unitKeyList에 등록된 key들은 반드시 존재하는 unit이므로 예외처리 하지 않음
	{
		const MkHashStr& currentKey = unitKeyList[i];
		ePrimitiveDataType unitType = targetNode.GetUnitType(currentKey);
		dwInterface.Write(static_cast<int>(unitType)); // type
		dwInterface.Write(MkStr(currentKey)); // name
		dwInterface.Write(targetNode.GetDataSize(currentKey)); // count

		switch (unitType)
		{
		case ePDT_Bool: __TSI_AssignUnitDataToMemory<bool>::Proceed(targetNode, currentKey, dwInterface); break;
		case ePDT_Int: __TSI_AssignUnitDataToMemory<int>::Proceed(targetNode, currentKey, dwInterface); break;
		case ePDT_UnsignedInt: __TSI_AssignUnitDataToMemory<unsigned int>::Proceed(targetNode, currentKey, dwInterface); break;
		case ePDT_Float: __TSI_AssignUnitDataToMemory<float>::Proceed(targetNode, currentKey, dwInterface); break;
		case ePDT_Int2: __TSI_AssignUnitDataToMemory<MkInt2>::Proceed(targetNode, currentKey, dwInterface); break;
		case ePDT_Vec2: __TSI_AssignUnitDataToMemory<MkVec2>::Proceed(targetNode, currentKey, dwInterface); break;
		case ePDT_Vec3: __TSI_AssignUnitDataToMemory<MkVec3>::Proceed(targetNode, currentKey, dwInterface); break;
		case ePDT_Str: __TSI_AssignUnitDataToMemory<MkStr>::Proceed(targetNode, currentKey, dwInterface); break;
		}
	}
}

void MkDataNodeToMemoryConverter::_ConvertChildNodes(const MkDataNode& targetNode, MkInterfaceForDataWriting& dwInterface) const
{
	MkArray<MkHashStr> templateNodeList, normalNodeList;
	targetNode.__GetClassifiedChildNodeNameList(templateNodeList, normalNodeList); // 우선순위에 따라 분류
	MK_INDEXING_LOOP(templateNodeList, i)
	{
		_ParseNode(*targetNode.GetChildNode(templateNodeList[i]), dwInterface); // 템플릿 노드 먼저
	}
	MK_INDEXING_LOOP(normalNodeList, i)
	{
		_ParseNode(*targetNode.GetChildNode(normalNodeList[i]), dwInterface); // 일반 노드 나중에
	}
}

//------------------------------------------------------------------------------------------------//