
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
		MK_CHECK(drInterface.Read(count), targetNode.GetNodeName().GetString() + L" ����� " + unitName + L" ���� �� ���� �б� ����")
			return false;

		MK_CHECK(count > 0, targetNode.GetNodeName().GetString() + L" ����� " + unitName + L" ���� �� ������ 0��")
			return false;

		MkArray<DataType> dataList;
		dataList.Reserve(count);
		for (unsigned int i=0; i<count; ++i)
		{
			DataType buffer;
			MK_CHECK(drInterface.Read(buffer), targetNode.GetNodeName().GetString() + L" ����� " + unitName + L" ���� " + MkStr(i) + L"��° �� �б� ����")
				return false;

			dataList.PushBack(buffer);
		}

		MkHashStr key = unitName;
		if (targetNode.IsValidKey(key))
		{
			MK_CHECK(targetNode.SetData(key, dataList), targetNode.GetNodeName().GetString() + L" ����� " + unitName + L" ���� �� �Ҵ� ����")
				return false;
		}
		else
		{
			MK_CHECK(targetNode.CreateUnit(key, dataList), targetNode.GetNodeName().GetString() + L" ����� " + unitName + L" ���� ���� ����")
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
	MK_CHECK(drInterface.SetUp(srcArray, startPos), L"�Է� �����Ͱ� �� ��� ���� ����")
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
	MK_CHECK(MkFileManager::GetFileData(filePath, srcArray), MkStr(filePath) + L" ��� ���� �б� ����")
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
	// Ÿ��
	// �� �̻��� ������ ���� ��� ����
	int blockType;
	if (!drInterface.Read(blockType))
		return MKDEF_DNC_MSG_COMPLETE;

	// ��� ���� �±��̸� Ż��
	if (blockType == MkTagDefinitionForDataNode::IndexMarkForNodeBlockEnd)
		return MKDEF_DNC_MSG_COMPLETE;

	// �̸�
	MkStr blockName;
	MK_CHECK(drInterface.Read(blockName) && (!blockName.Empty()), L"block name �б� ����")
		return MKDEF_DNC_MSG_ERROR;

	// ���
	if ((blockType == MkTagDefinitionForDataNode::IndexMarkForNodeBegin) || (blockType == MkTagDefinitionForDataNode::IndexMarkForTemplateBegin))
	{
		// ���ø� ���뿩��
		bool templateApplied;
		MK_CHECK(drInterface.Read(templateApplied), blockName + L" ��忡�� templateApplied ������ �б� ����")
			return MKDEF_DNC_MSG_ERROR;

		// ���ø� �̸�
		MkStr templateName;
		if (templateApplied)
		{
			MK_CHECK(drInterface.Read(templateName), blockName + L" ��忡�� ����� ���ø� �̸� �б� ����")
				return MKDEF_DNC_MSG_ERROR;
		}

		// �ڽ� ��� ���� Ȥ�� �˻�
		MkHashStr targetNodeName = blockName;
		MkDataNode* childNode = targetNode.ChildExist(targetNodeName) ?
			targetNode.GetChildNode(targetNodeName) : targetNode.CreateChildNode(targetNodeName);
		MK_CHECK(childNode != NULL, L"�޸� �Ҵ� �����Ͽ� " + targetNode.GetNodeName().GetString() + L" ����� �ڽ����� " + blockName + L" ��� ���� ����")
			return MKDEF_DNC_MSG_ERROR;

		// ���ø� ���� ����̸� �ش� ���ø����� �ʱ�ȭ
		if (templateApplied)
		{
			MK_CHECK(childNode->GetTemplateLink() == NULL, blockName + L" ���� �̹� ���ø��� ����� ���¿��� " + templateName + L" ���ø��� ���� �� �� ����")
				return MKDEF_DNC_MSG_ERROR;
			
			MK_CHECK(childNode->ApplyTemplate(templateName), blockName + L" ��忡 " + templateName + L" ���ø� ��带 ���� �� �� ����")
				return MKDEF_DNC_MSG_ERROR;
		}

		// �ڽ� ��� ����
		if (!_BuildNode(drInterface, *childNode))
			return MKDEF_DNC_MSG_ERROR;

		// ���ø� ����� ��� ���
		if (blockType == MkTagDefinitionForDataNode::IndexMarkForTemplateBegin)
		{
			MK_CHECK(childNode->DeclareToTemplate(), blockName + L" ���� ���ø����� ���� �� �� ����")
				return MKDEF_DNC_MSG_ERROR;
		}
	}
	// ����
	else
	{
		ePrimitiveDataType unitType = static_cast<ePrimitiveDataType>(blockType);
		MK_CHECK(MkPrimitiveDataType::IsValid(unitType), targetNode.GetNodeName().GetString() + L" ����� " + blockName + L" ���� Ÿ���� ������ ���� : " + MkStr(blockType))
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