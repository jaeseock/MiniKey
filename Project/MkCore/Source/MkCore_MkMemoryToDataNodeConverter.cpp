
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataTypeDefinition.h"
#include "MkCore_MkFileManager.h"
#include "MkCore_MkInterfaceForDataReading.h"
#include "MkCore_MkTagDefinitionForDataNode.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkMemoryToDataNodeConverter.h"

//------------------------------------------------------------------------------------------------//

template <class DataType>
class __TSI_AssignUnitToNode
{
public:
	static bool Proceed(MkInterfaceForDataReading& drInterface, MkDataNode& targetNode, const MkStr& unitName)
	{
		unsigned int count;
		MK_CHECK(drInterface.Read(count), targetNode.GetNodeName().GetString() + L" 노드의 " + unitName + L" 유닛 값 갯수 읽기 오류")
			return false;

		MK_CHECK(count > 0, targetNode.GetNodeName().GetString() + L" 노드의 " + unitName + L" 유닛 값 갯수가 0임")
			return false;

		MkArray<DataType> dataList;
		dataList.Reserve(count);
		for (unsigned int i=0; i<count; ++i)
		{
			DataType buffer;
			MK_CHECK(drInterface.Read(buffer), targetNode.GetNodeName().GetString() + L" 노드의 " + unitName + L" 유닛 " + MkStr(i) + L"번째 값 읽기 오류")
				return false;

			dataList.PushBack(buffer);
		}

		MkHashStr key = unitName;
		if (targetNode.IsValidKey(key))
		{
			MK_CHECK(targetNode.SetData(key, dataList), targetNode.GetNodeName().GetString() + L" 노드의 " + unitName + L" 유닛 값 할당 실패")
				return false;
		}
		else
		{
			MK_CHECK(targetNode.CreateUnit(key, dataList), targetNode.GetNodeName().GetString() + L" 노드의 " + unitName + L" 유닛 생성 실패")
				return false;
		}
		return true;
	}
};

//------------------------------------------------------------------------------------------------//

bool MkMemoryToDataNodeConverter::Convert(const MkByteArray& srcArray, MkDataNode& destination, bool checkBinTag) const
{
	unsigned int startPos = 0;
	if (checkBinTag)
	{
		MkByteArray binTag;
		srcArray.GetSubArray(MkArraySection(0, MkTagDefinitionForDataNode::TagForBinaryDataNode.GetSize()), binTag);
		if (binTag != MkTagDefinitionForDataNode::TagForBinaryDataNode)
			return false;

		startPos = MkTagDefinitionForDataNode::TagForBinaryDataNode.GetSize();
	}

	MkInterfaceForDataReading drInterface;
	MK_CHECK(drInterface.SetUp(srcArray, startPos), L"입력 데이터가 비어서 노드 구성 실패")
		return false;

	bool ok = _BuildNode(drInterface, destination);
	if (ok)
	{
		destination.Rehash();
	}
	return ok;
}

bool MkMemoryToDataNodeConverter::Convert(const MkPathName& filePath, MkDataNode& destination) const
{
	MkByteArray srcArray;
	MK_CHECK(MkFileManager::GetFileData(filePath, srcArray), MkStr(filePath) + L" 경로 파일 읽기 실패")
		return false;

	return Convert(srcArray, destination, true);
}

//------------------------------------------------------------------------------------------------//

bool MkMemoryToDataNodeConverter::_BuildNode(MkInterfaceForDataReading& drInterface, MkDataNode& destination) const
{
	unsigned int msg = MKDEF_DNC_MSG_CONTINUE;
	while (msg == MKDEF_DNC_MSG_CONTINUE)
	{
		msg = _BuildBlock(drInterface, destination);
	}
	return (msg == MKDEF_DNC_MSG_COMPLETE);
}

unsigned int MkMemoryToDataNodeConverter::_BuildBlock(MkInterfaceForDataReading& drInterface, MkDataNode& targetNode) const
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

		// 자식 노드 생성 혹은 검색
		MkHashStr targetNodeName = blockName;
		MkDataNode* childNode = targetNode.ChildExist(targetNodeName) ?
			targetNode.GetChildNode(targetNodeName) : targetNode.CreateChildNode(targetNodeName);
		MK_CHECK(childNode != NULL, L"메모리 할당 실패하여 " + targetNode.GetNodeName().GetString() + L" 노드의 자식으로 " + blockName + L" 노드 생성 실패")
			return MKDEF_DNC_MSG_ERROR;

		// 템플릿 적용 노드이면 해당 템플릿으로 초기화
		if (templateApplied)
		{
			MK_CHECK(childNode->GetTemplateLink() == NULL, blockName + L" 노드는 이미 템플릿이 적용된 상태여서 " + templateName + L" 템플릿을 적용 할 수 없음")
				return MKDEF_DNC_MSG_ERROR;
			
			MK_CHECK(childNode->ApplyTemplate(templateName), blockName + L" 노드에 " + templateName + L" 템플릿 노드를 적용 할 수 없음")
				return MKDEF_DNC_MSG_ERROR;
		}

		// 자식 노드 구성
		if (!_BuildNode(drInterface, *childNode))
			return MKDEF_DNC_MSG_ERROR;

		// 템플릿 노드일 경우 등록
		if (blockType == MkTagDefinitionForDataNode::IndexMarkForTemplateBegin)
		{
			MK_CHECK(childNode->DeclareToTemplate(), blockName + L" 노드는 템플릿으로 선언 될 수 없음")
				return MKDEF_DNC_MSG_ERROR;
		}
	}
	// 유닛
	else
	{
		ePrimitiveDataType unitType = static_cast<ePrimitiveDataType>(blockType);
		MK_CHECK(MkPrimitiveDataType::IsValid(unitType), targetNode.GetNodeName().GetString() + L" 노드의 " + blockName + L" 유닛 타입은 허용되지 않음 : " + MkStr(blockType))
			return MKDEF_DNC_MSG_ERROR;

		bool ok = false;
		switch (unitType)
		{
		case ePDT_Bool: ok = __TSI_AssignUnitToNode<bool>::Proceed(drInterface, targetNode, blockName); break;
		case ePDT_Int: ok = __TSI_AssignUnitToNode<int>::Proceed(drInterface, targetNode, blockName); break;
		case ePDT_UnsignedInt: ok = __TSI_AssignUnitToNode<unsigned int>::Proceed(drInterface, targetNode, blockName); break;
		case ePDT_Float: ok = __TSI_AssignUnitToNode<float>::Proceed(drInterface, targetNode, blockName); break;
		case ePDT_Int2: ok = __TSI_AssignUnitToNode<MkInt2>::Proceed(drInterface, targetNode, blockName); break;
		case ePDT_Vec2: ok = __TSI_AssignUnitToNode<MkVec2>::Proceed(drInterface, targetNode, blockName); break;
		case ePDT_Vec3: ok = __TSI_AssignUnitToNode<MkVec3>::Proceed(drInterface, targetNode, blockName); break;
		case ePDT_Str: ok = __TSI_AssignUnitToNode<MkStr>::Proceed(drInterface, targetNode, blockName); break;
		}
		if (!ok)
			return MKDEF_DNC_MSG_ERROR;
	}

	return MKDEF_DNC_MSG_CONTINUE;
}

//------------------------------------------------------------------------------------------------//