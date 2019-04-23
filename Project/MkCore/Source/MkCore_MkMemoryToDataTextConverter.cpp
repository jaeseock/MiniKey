
#include "MkCore_MkStr.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataTypeDefinition.h"
#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkInterfaceForDataReading.h"
#include "MkCore_MkTagDefinitionForDataNode.h"
#include "MkCore_MkMemoryToDataTextConverter.h"


// text �Ӹ��� ���� ũ��
#define MKDEF_HEADER_RESERVATION 500

// binary -> text ��ȯ�� �뷮�� �þ��� ������ 20�踦 �ѱ��� ����Ŷ� ����
#define MKDEF_M_TO_T_RESERVATION 20

// �⺻ ��Ŀ�� tab
#define MKDEF_M_TO_T_ANCHOR_TYPE MkStr::TAB

// �⺻ ���鹮�ڴ� space
#define MKDEF_M_TO_T_BLANK_TYPE MkStr::SPACE

//------------------------------------------------------------------------------------------------//

template <class DataType>
class __TSI_AssignUnitToText
{
public:
	static bool Assign(MkInterfaceForDataReading& drInterface, MkArray<MkStr>& dataList, const MkStr& unitName)
	{
		unsigned int count;
		MK_CHECK(drInterface.Read(count), unitName + L" ���� �� ���� �б� ����")
			return false;

		MK_CHECK(count > 0, unitName + L" ���� �� ������ 0��")
			return false;

		dataList.Reserve(count);
		for (unsigned int i=0; i<count; ++i)
		{
			DataType buffer;
			MK_CHECK(drInterface.Read(buffer), unitName + L" ���� " + MkStr(i) + L"��° �� �б� ����")
				return false;

			dataList.PushBack(MkStr(buffer));
		}
		return true;
	}

	static bool Export(MkInterfaceForDataReading& drInterface, MkArray<MkStr>& dataList, const MkStr& unitName)
	{
		unsigned int count;
		MK_CHECK(drInterface.Read(count), unitName + L" ���� �� ���� �б� ����")
			return false;

		MK_CHECK(count > 0, unitName + L" ���� �� ������ 0��")
			return false;

		dataList.Reserve(count);
		for (unsigned int i=0; i<count; ++i)
		{
			DataType buffer;
			MK_CHECK(drInterface.Read(buffer), unitName + L" ���� " + MkStr(i) + L"��° �� �б� ����")
				return false;

			dataList.PushBack().ImportByteArray(buffer);
		}
		return true;
	}
};

//------------------------------------------------------------------------------------------------//

bool MkMemoryToDataTextConverter::Convert(const MkByteArray& srcArray, MkStr& strBuffer)
{
	strBuffer.Clear();
	if (srcArray.Empty())
		return true;
	
	MkInterfaceForDataReading drInterface;
	MK_CHECK(drInterface.SetUp(srcArray, 0), L"MkInterfaceForDataReading �ʱ�ȭ ����")
		return false;

	strBuffer.Reserve(srcArray.GetSize() * MKDEF_M_TO_T_RESERVATION);
	return _BuildText(drInterface, 0, strBuffer);
}

bool MkMemoryToDataTextConverter::Convert(const MkByteArray& srcArray, const MkPathName& filePath)
{
	MkStr strBuffer;
	bool converting = true;

	if (srcArray.Empty())
	{
		strBuffer.Reserve(MKDEF_HEADER_RESERVATION);

		_InitiateHeaderMsg(strBuffer, filePath);
	}
	else
	{
		MkInterfaceForDataReading drInterface;
		MK_CHECK(drInterface.SetUp(srcArray, 0), L"MkInterfaceForDataReading �ʱ�ȭ ����")
			return false;

		strBuffer.Reserve(MKDEF_HEADER_RESERVATION + srcArray.GetSize() * MKDEF_M_TO_T_RESERVATION);

		_InitiateHeaderMsg(strBuffer, filePath);
		strBuffer += MkStr::LF;
		strBuffer += MkStr::LF;

		converting = _BuildText(drInterface, 0, strBuffer);
	}

	if (converting)
	{
		// �ٱ��� ó���� ���� UTF-8�� ����
		MK_CHECK(strBuffer.WriteToTextFile(filePath, true, false), MkStr(filePath) + L"��ο� ���� ���� ����")
			return false;
	}
	return converting;
}

//------------------------------------------------------------------------------------------------//

void MkMemoryToDataTextConverter::_InitiateHeaderMsg(MkStr& strBuffer, const MkPathName& filePath) const
{
	// ��� ����
	strBuffer += L"//--------------------------------------------------------------------//" + MkStr::LF;
	strBuffer += L"// MiniKey data node text source" + MkStr::LF;
	strBuffer += L"//   - file path : " + MkStr(filePath) + MkStr::LF;

	if (MkSystemEnvironment::InstancePtr() != NULL)
	{
		strBuffer += L"//   - time stamp : " + MK_SYS_ENV.GetCurrentSystemDate() + L" (" + MK_SYS_ENV.GetCurrentSystemTime() + L")" + MkStr::LF;
		strBuffer += L"//   - exporter : " + MK_SYS_ENV.GetCurrentLogOnUserName() + L" (" + MkPathName::GetApplicationName() + L")" + MkStr::LF;
	}

	strBuffer += L"//--------------------------------------------------------------------//";
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

	// ��Ŀ
	static const MkStr blankStr = MKDEF_M_TO_T_ANCHOR_TYPE;
	MkStr anchor = MkStr::LF;
	if (nodeDepth > 0)
	{
		anchor.Reserve(MkStr::LF.GetSize() + blankStr.GetSize() * nodeDepth);
		for (unsigned int i=0; i<nodeDepth; ++i)
		{
			anchor += blankStr;
		}
	}

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

		// ���
		if (!strBuffer.CheckPostfix(MkTagDefinitionForDataNode::TagForBlockBegin))
		{
			strBuffer += MkStr::LF; // ��� ��½ÿ��� ���� ��尡 �ƴϸ� �� ���� ������ ��
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

		// body �κ��� depth�� �߰��� ��� ����
		if (!_BuildText(drInterface, nodeDepth + 1, strBuffer))
			return MKDEF_DNC_MSG_ERROR;

		if (strBuffer.GetSize() == initStrSize) // ���� �߰��� ������ ���� �� ���
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
	// ���ø� �о�ֱ�
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
	// ����
	else
	{
		ePrimitiveDataType unitType = static_cast<ePrimitiveDataType>(blockType);
		MK_CHECK(MkPrimitiveDataType::IsValid(unitType), blockName + L" ���� Ÿ���� ������ ���� : " + MkStr(blockType))
			return MKDEF_DNC_MSG_ERROR;

		bool ok = false;
		MkArray<MkStr> dataList;
		switch (unitType)
		{
		case ePDT_Bool: ok = __TSI_AssignUnitToText<bool>::Assign(drInterface, dataList, blockName); break;
		case ePDT_Int: ok = __TSI_AssignUnitToText<int>::Assign(drInterface, dataList, blockName); break;
		case ePDT_UnsignedInt: ok = __TSI_AssignUnitToText<unsigned int>::Assign(drInterface, dataList, blockName); break;
		case ePDT_DInt: ok = __TSI_AssignUnitToText<__int64>::Assign(drInterface, dataList, blockName); break;
		case ePDT_DUnsignedInt: ok = __TSI_AssignUnitToText<unsigned __int64>::Assign(drInterface, dataList, blockName); break;
		case ePDT_Float: ok = __TSI_AssignUnitToText<float>::Assign(drInterface, dataList, blockName); break;
		case ePDT_Int2: ok = __TSI_AssignUnitToText<MkInt2>::Assign(drInterface, dataList, blockName); break;
		case ePDT_Vec2: ok = __TSI_AssignUnitToText<MkVec2>::Assign(drInterface, dataList, blockName); break;
		case ePDT_Vec3: ok = __TSI_AssignUnitToText<MkVec3>::Assign(drInterface, dataList, blockName); break;
		case ePDT_Str:
			{
				ok = __TSI_AssignUnitToText<MkStr>::Assign(drInterface, dataList, blockName);

				// ���ڿ��� ��� �߰����� ó�� �ʿ�
				if (ok)
				{
					MK_INDEXING_LOOP(dataList, i)
					{
						// Ż�⹮��(escape sequence) �ݿ�
						MkStr tmp = dataList[i];
						tmp.ReplaceKeyword(L"\"", MkTagDefinitionForDataNode::TagForDQM);
						tmp.ReplaceKeyword(MkTagDefinitionForDataNode::TagForDQM, L"\\\"");
						tmp.ReplaceCRLFtoTag();

						// DQM���� �ѷ���
						dataList[i].Reserve(tmp.GetSize() + 2);
						dataList[i] = L"\"";
						dataList[i] += tmp;
						dataList[i] += L"\"";
					}
				}
			}
			break;
		case ePDT_ByteArray: ok = __TSI_AssignUnitToText<MkByteArray>::Export(drInterface, dataList, blockName);
			break;
		}
		if (!ok)
			return MKDEF_DNC_MSG_ERROR;

		// ���
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
			strBuffer += MKDEF_M_TO_T_BLANK_TYPE; // size�� �ּ����� �߰�
			strBuffer += L"// [";
			strBuffer += dataSize;
			strBuffer += L"]";

			MkStr arrayAnchor = anchor + MKDEF_M_TO_T_ANCHOR_TYPE;
			unsigned int lastIndex = dataSize - 1;
			MK_INDEXING_LOOP(dataList, i)
			{
				bool lineFeed = ((unitType == ePDT_Str) || (unitType == ePDT_ByteArray) || ((i % 5) == 0)); // line feed ����
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
