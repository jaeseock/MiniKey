
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkTagDefinitionForDataNode.h"
#include "MkCore_MkExcelFileInterface.h"
#include "MkCore_MkDataNodeToExcelConverter.h"


#define MKDEF_STR_BUFFER(name, src) \
	MkStr name; \
	name.Reserve(src.GetSize() + 2); \
	name += L"\""; \
	name += src; \
	name += L"\"";

template <class DataType>
class __TSI_GetAndPutUnitData
{
public:
	static void Proceed(const MkDataNode& targetNode, const MkHashStr& unitName, const MkUInt2& startPos, const MkUInt2& dir, MkUInt2& minCell, MkUInt2& maxCell, MkExcelFileInterface& excelWritter)
	{
		MkArray<DataType> dataList;
		targetNode.GetData(unitName, dataList);
		MK_INDEXING_LOOP(dataList, i)
		{
			MkUInt2 currPos = startPos + dir * i;
			excelWritter.PutData(currPos.x, currPos.y, dataList[i]);
			excelWritter.SetBorder(currPos.x, currPos.y, 1);
			minCell.CompareAndKeepMin(currPos);
			maxCell.CompareAndKeepMax(currPos);
		}
	}

	static void Proceed(const MkDataNode& targetNode, const MkHashStr& unitName, const MkUInt2& pos, MkUInt2& minCell, MkUInt2& maxCell, MkExcelFileInterface& excelWritter)
	{
		MkArray<DataType> dataList;
		targetNode.GetData(unitName, dataList);
		if (dataList.GetSize() == 1)
		{
			excelWritter.PutData(pos.x, pos.y, dataList[0]);
		}
		else if (dataList.GetSize() > 1)
		{
			MkStr msgBuffer;
			msgBuffer.Reserve(dataList.GetSize() * 32);
			MK_INDEXING_LOOP(dataList, i)
			{
				if (i > 0)
				{
					msgBuffer += MkTagDefinitionForDataNode::TagForArrayDivider;
				}
				msgBuffer += MkStr(dataList[i]);
			}

			excelWritter.PutData(pos.x, pos.y, msgBuffer);
		}
	}
};

template <> void __TSI_GetAndPutUnitData<MkStr>::Proceed
(const MkDataNode& targetNode, const MkHashStr& unitName, const MkUInt2& startPos, const MkUInt2& dir, MkUInt2& minCell, MkUInt2& maxCell, MkExcelFileInterface& excelWritter)
{
	MkArray<MkStr> dataList;
	targetNode.GetData(unitName, dataList);
	MK_INDEXING_LOOP(dataList, i)
	{
		MkStr tmp = dataList[i]; // Ż�⹮��(escape sequence) �ݿ�
		tmp.ReplaceKeyword(L"\"", MkTagDefinitionForDataNode::TagForDQM);
		tmp.ReplaceKeyword(MkTagDefinitionForDataNode::TagForDQM, L"\\\"");
		tmp.ReplaceCRLFtoTag();

		MkUInt2 currPos = startPos + dir * i;
		MKDEF_STR_BUFFER(buffer, tmp);
		excelWritter.PutData(currPos.x, currPos.y, buffer);
		excelWritter.SetBorder(currPos.x, currPos.y, 1);
		minCell.CompareAndKeepMin(currPos);
		maxCell.CompareAndKeepMax(currPos);
	}
}

template <> void __TSI_GetAndPutUnitData<MkStr>::Proceed
(const MkDataNode& targetNode, const MkHashStr& unitName, const MkUInt2& pos, MkUInt2& minCell, MkUInt2& maxCell, MkExcelFileInterface& excelWritter)
{
	MkArray<MkStr> dataList;
	targetNode.GetData(unitName, dataList);

	MkStr msgBuffer;
	msgBuffer.Reserve(dataList.GetSize() * 256);
	MK_INDEXING_LOOP(dataList, i)
	{
		if (i > 0)
		{
			msgBuffer += MkTagDefinitionForDataNode::TagForArrayDivider;
		}

		MkStr tmp = dataList[i]; // Ż�⹮��(escape sequence) �ݿ�
		tmp.ReplaceKeyword(L"\"", MkTagDefinitionForDataNode::TagForDQM);
		tmp.ReplaceKeyword(MkTagDefinitionForDataNode::TagForDQM, L"\\\"");
		tmp.ReplaceCRLFtoTag();

		MKDEF_STR_BUFFER(tmpBuffer, tmp);
		msgBuffer += tmpBuffer;
	}

	excelWritter.PutData(pos.x, pos.y, msgBuffer);
}


//------------------------------------------------------------------------------------------------//

bool MkDataNodeToExcelConverter::Convert(const MkDataNode& source, const MkPathName& filePath) const
{
	MkExcelFileInterface excelWritter;
	if (!excelWritter.SetUp())
		return false;

	if (!_BuildSheet(source, MKDEF_EXCEL_SHEET_ROOT_NAME, excelWritter))
		return false;

	if (!excelWritter.SetActiveSheet(MKDEF_EXCEL_SHEET_ROOT_NAME))
		return false;
	
	MK_CHECK(excelWritter.SaveAs(filePath), MkStr(filePath) + L"��ο� ���� ���� ����")
		return false;

	return true;
}

//------------------------------------------------------------------------------------------------//

bool MkDataNodeToExcelConverter::_PutStringData(const MkUInt2& pos, const MkStr& value, MkUInt2& minCell, MkUInt2& maxCell, MkExcelFileInterface& excelWritter) const
{
	if (!excelWritter.PutData(pos.x, pos.y, value))
		return false;

	if (!excelWritter.SetBorder(pos.x, pos.y, 1))
		return false;

	minCell.CompareAndKeepMin(pos);
	maxCell.CompareAndKeepMax(pos);
	return true;
}

bool MkDataNodeToExcelConverter::_WriteHeader
(const MkStr& pos, const MkStr& desc, const MkStr& value, MkUInt2& minCell, MkUInt2& maxCell, MkExcelFileInterface& excelWritter) const
{
	MkUInt2 cellPos;
	MkExcelFileInterface::ConvertPosition(pos, cellPos);
	if (!_PutStringData(cellPos, value, minCell, maxCell, excelWritter))
		return false;

	if (cellPos.y > 0)
	{
		cellPos.y -= 1;
		if (!_PutStringData(cellPos, desc, minCell, maxCell, excelWritter))
			return false;
	}
	return true;
}

bool MkDataNodeToExcelConverter::_WriteNodeTag
(const MkUInt2& pos, const MkUInt2& dir, const MkHashStr& childName, const MkDataNode& parentNode, MkHashStr& sheetName, MkUInt2& minCell, MkUInt2& maxCell, MkExcelFileInterface& excelWritter) const
{
	// key
	MkUInt2 currPos = pos + dir;
	MKDEF_STR_BUFFER(nameBuffer, childName.GetString());
	if (!_PutStringData(currPos, nameBuffer, minCell, maxCell, excelWritter))
		return false;

	// template
	currPos += dir;
	MkHashStr templateName;
	if (parentNode.GetChildNode(childName)->GetAppliedTemplateName(templateName))
	{
		MKDEF_STR_BUFFER(templateBuffer, templateName.GetString());
		if (!_PutStringData(currPos, templateBuffer, minCell, maxCell, excelWritter))
			return false;
	}

	// sheet
	currPos += dir;
	_GetSheetName(childName, excelWritter, sheetName);
	MKDEF_STR_BUFFER(sheetBuffer, sheetName.GetString());
	if (!_PutStringData(currPos, sheetBuffer, minCell, maxCell, excelWritter))
		return false;

	return true;
}

void MkDataNodeToExcelConverter::_GetSheetName(const MkHashStr& nodeName, MkExcelFileInterface& excelWritter, MkHashStr& buffer) const
{
	if (!excelWritter.SheetExist(nodeName))
	{
		buffer = nodeName;
		return;
	}

	int i = 1;
	while (true)
	{
		MkStr newName = nodeName.GetString();
		newName += L"_";
		newName += i;
		MkHashStr tmp = newName;

		if (!excelWritter.SheetExist(tmp))
		{
			buffer = tmp;
			break;
		}
		++i;
	}
}

bool MkDataNodeToExcelConverter::_BuildSheet(const MkDataNode& node, const MkHashStr& sheetName, MkExcelFileInterface& excelWritter) const
{
	// node�� type �Ǻ�
	// - Table : ���� ���� ������ ���� ������ ���� ��� ���� �ڽ� ��� ����
	// - Single : �� ��
	bool isSingleType = true;
	MkHashStr templateNameOfTableType;

	if ((node.GetUnitCount() == 0) && (node.GetChildNodeCount() > 1))
	{
		MkArray<MkHashStr> keys;
		node.GetChildNodeList(keys);

		MK_INDEXING_LOOP(keys, i)
		{
			if (node.GetChildNode(keys[i])->GetChildNodeCount() > 0) // ���� ��尡 ����
			{
				keys.Clear();
				break;
			}
		}

		if (!keys.Empty()) // ��� ���� ��尡 ������
		{
			bool sameForm = true;
			const MkDataNode& firstChild = *node.GetChildNode(keys[0]);
			for(unsigned int i=1; i<keys.GetSize(); ++i)
			{
				if (!node.GetChildNode(keys[i])->__SameForm(firstChild))
				{
					sameForm = false;
					break;
				}
			}

			if (sameForm)
			{
				isSingleType = false; // Table Ÿ��
			}

			// ���� ���ø� ���� ����
			if (firstChild.GetAppliedTemplateName(templateNameOfTableType)) // ù �ڽ� ��忡 ���ø��� ������
			{
				for(unsigned int i=1; i<keys.GetSize(); ++i)
				{
					MkHashStr currTemplateName;
					if ((!node.GetChildNode(keys[i])->GetAppliedTemplateName(currTemplateName)) || // ������忡 ���ø� ����
						(currTemplateName != templateNameOfTableType)) // ���ø��� �ִµ� �ٸ�
					{
						templateNameOfTableType.Clear();
						break;
					}
				}
			}
		}
	}

	// sheet ����
	if (!excelWritter.CreateSheet(sheetName))
		return false;

	if (!excelWritter.SetActiveSheet(sheetName))
		return false;

	// sheet�� �ּ�, �ִ� ��
	MkUInt2 minCell(0xffffffff, 0xffffffff), maxCell;

	// type
	if (!_WriteHeader(MKDEF_EXCEL_SHEET_TYPE_POS, L"Type", (isSingleType) ? MKDEF_EXCEL_SHEET_SINGLE_TYPE : MKDEF_EXCEL_SHEET_TABLE_TYPE, minCell, maxCell, excelWritter))
		return false;

	// anchor
	bool isVertical = isSingleType;
	if (!_WriteHeader(MKDEF_EXCEL_SHEET_ANCHOR_POS, L"Anchor", (isVertical) ? MKDEF_EXCEL_SHEET_VERTICAL_ANCHOR : MKDEF_EXCEL_SHEET_HORIZONTAL_ANCHOR, minCell, maxCell, excelWritter))
		return false;

	// start position
	MkStr strStartPosition;
	MkArray<MkHashStr> unitKeys, templateNodeList, normalNodeList;

	do
	{
		if (!node.__IsValidNode())
			break;

		node.__GetValidUnitList(unitKeys);
		node.__GetClassifiedChildNodeNameList(templateNodeList, normalNodeList);

		if (isSingleType && unitKeys.Empty() && templateNodeList.Empty() && normalNodeList.Empty())
			break;

		strStartPosition = MKDEF_EXCEL_SHEET_DEF_STARTPOSITION;
	}
	while (false);

	if (!_WriteHeader(MKDEF_EXCEL_SHEET_STARTPOSITION_POS, L"Start", strStartPosition, minCell, maxCell, excelWritter))
		return false;

	// ���ǹ��� ������ ������ ���⼭ ����
	// sheet ��ü�� ������ �ʾƵ� ������ �� ��带 ������ٴ� �ǵ� ��ü�� �ǹ̰� �����Ƿ� �� sheet�� ����� �ǵ��� ���� ��
	if (strStartPosition.Empty())
		return true;

	// type, key �������. vertical�� ��� row, horizontal�� ��� column �������� ����
	MkUInt2 tkDir = (isVertical) ? MkUInt2(1, 0) : MkUInt2(0, 1);
	// field �������. tkDir�� ����
	MkUInt2 fieldDir = MkUInt2(tkDir.y, tkDir.x);

	MkUInt2 dataStartPos; // ������ ������ġ
	MkExcelFileInterface::ConvertPosition(strStartPosition, dataStartPos);

	MkArray<MkHashStr> targetNodeList, targetSheetList;

	if (isSingleType)
	{
		// unit
		unsigned int unitCount = unitKeys.GetSize();
		if (unitCount > 0)
		{
			unitKeys.SortInAscendingOrder();
			MK_INDEXING_LOOP(unitKeys, i)
			{
				const MkHashStr& currKey = unitKeys[i];
				ePrimitiveDataType primType = node.GetUnitType(currKey);

				// type
				MkUInt2 currPos = dataStartPos + tkDir * i;
				if (!_PutStringData(currPos, MkPrimitiveDataType::GetTag(primType), minCell, maxCell, excelWritter))
					return false;

				// key
				currPos += fieldDir;
				if (!_PutStringData(currPos, currKey.GetString(), minCell, maxCell, excelWritter))
					return false;

				// value
				currPos += fieldDir;
				switch (primType)
				{
				case ePDT_Bool: __TSI_GetAndPutUnitData<bool>::Proceed(node, currKey, currPos, fieldDir, minCell, maxCell, excelWritter); break;
				case ePDT_Int: __TSI_GetAndPutUnitData<int>::Proceed(node, currKey, currPos, fieldDir, minCell, maxCell, excelWritter); break;
				case ePDT_UnsignedInt: __TSI_GetAndPutUnitData<unsigned int>::Proceed(node, currKey, currPos, fieldDir, minCell, maxCell, excelWritter); break;
				case ePDT_DInt: __TSI_GetAndPutUnitData<__int64>::Proceed(node, currKey, currPos, fieldDir, minCell, maxCell, excelWritter); break;
				case ePDT_DUnsignedInt: __TSI_GetAndPutUnitData<unsigned __int64>::Proceed(node, currKey, currPos, fieldDir, minCell, maxCell, excelWritter); break;
				case ePDT_Float: __TSI_GetAndPutUnitData<float>::Proceed(node, currKey, currPos, fieldDir, minCell, maxCell, excelWritter); break;
				case ePDT_Int2: __TSI_GetAndPutUnitData<MkInt2>::Proceed(node, currKey, currPos, fieldDir, minCell, maxCell, excelWritter); break;
				case ePDT_Vec2: __TSI_GetAndPutUnitData<MkVec2>::Proceed(node, currKey, currPos, fieldDir, minCell, maxCell, excelWritter); break;
				case ePDT_Vec3: __TSI_GetAndPutUnitData<MkVec3>::Proceed(node, currKey, currPos, fieldDir, minCell, maxCell, excelWritter); break;
				case ePDT_Str: __TSI_GetAndPutUnitData<MkStr>::Proceed(node, currKey, currPos, fieldDir, minCell, maxCell, excelWritter); break;
				case ePDT_ByteArray: __TSI_GetAndPutUnitData<MkByteArray>::Proceed(node, currKey, currPos, fieldDir, minCell, maxCell, excelWritter); break;
				}
			}
		}

		// node
		targetNodeList = templateNodeList;
		targetNodeList += normalNodeList;
		targetSheetList.Reserve(templateNodeList.GetSize() + normalNodeList.GetSize());

		MK_INDEXING_LOOP(templateNodeList, i)
		{
			MkUInt2 currPos = dataStartPos + tkDir * (unitCount + i);
			if (!_PutStringData(currPos, MkTagDefinitionForDataNode::TagForTemplate, minCell, maxCell, excelWritter))
				return false;

			MkHashStr targetSheetName;
			if (!_WriteNodeTag(currPos, fieldDir, templateNodeList[i], node, targetSheetName, minCell, maxCell, excelWritter))
				return false;

			targetSheetList.PushBack(targetSheetName);
		}

		MK_INDEXING_LOOP(normalNodeList, i)
		{
			MkUInt2 currPos = dataStartPos + tkDir * (unitCount + templateNodeList.GetSize() + i);
			if (!_PutStringData(currPos, MkTagDefinitionForDataNode::TagForNode, minCell, maxCell, excelWritter))
					return false;

			MkHashStr targetSheetName;
			if (!_WriteNodeTag(currPos, fieldDir, normalNodeList[i], node, targetSheetName, minCell, maxCell, excelWritter))
				return false;

			targetSheetList.PushBack(targetSheetName);
		}

		// push template
		if (sheetName.GetString().Equals(0, MKDEF_EXCEL_SHEET_ROOT_NAME))
		{
			MkHashStr templateName;
			if (node.GetAppliedTemplateName(templateName))
			{
				MkUInt2 currPos = dataStartPos + tkDir * (unitCount + templateNodeList.GetSize() + normalNodeList.GetSize());
				if (!_PutStringData(currPos, MkTagDefinitionForDataNode::TagForPushingTemplate, minCell, maxCell, excelWritter))
					return false;

				currPos += fieldDir;
				MKDEF_STR_BUFFER(templateBuffer, templateName.GetString());
				if (!_PutStringData(currPos, templateBuffer, minCell, maxCell, excelWritter))
					return false;
			}
		}
	}
	else
	{
		// ���� ���ø��� �����ϸ� ���
		if (!templateNameOfTableType.Empty())
		{
			MKDEF_STR_BUFFER(templateBuffer, templateNameOfTableType.GetString());
			if (!_PutStringData(dataStartPos, templateBuffer, minCell, maxCell, excelWritter))
				return false;
		}

		// ������ ��� �����ϹǷ� ù���� �ڽĿ��Լ� ���� ���� ����
		MkArray<MkHashStr> childList;
		node.GetChildNodeList(childList);

		const MkDataNode& firstChild = *node.GetChildNode(childList[0]);
		MkArray<MkHashStr> baseUnitKeys;
		firstChild.GetUnitKeyList(baseUnitKeys);
		baseUnitKeys.SortInAscendingOrder();

		// unit, node ������ type, key ���� ���
		MkUInt2 typeStartPos = dataStartPos + tkDir;
		MK_INDEXING_LOOP(baseUnitKeys, i)
		{
			const MkHashStr& currKey = baseUnitKeys[i];
			ePrimitiveDataType primType = firstChild.GetUnitType(currKey);

			MkUInt2 currPos = typeStartPos + tkDir * i;
			if (!_PutStringData(currPos, MkPrimitiveDataType::GetTag(primType), minCell, maxCell, excelWritter))
				return false;

			currPos += fieldDir;
			if (!_PutStringData(currPos, currKey.GetString(), minCell, maxCell, excelWritter))
				return false;
		}
		
		// �ڽ� ��带 ��ȸ�ϸ� ���� ���
		MkUInt2 nameStartPos = dataStartPos + fieldDir + fieldDir;
		MK_INDEXING_LOOP(childList, i)
		{
			const MkHashStr& currName = childList[i];
			const MkDataNode& currNode = *node.GetChildNode(currName);

			// �̸�
			MkUInt2 currPos = nameStartPos + fieldDir * i;
			MKDEF_STR_BUFFER(nameBuffer, currName.GetString());
			if (!_PutStringData(currPos, nameBuffer, minCell, maxCell, excelWritter))
				return false;

			unitKeys.Clear();
			currNode.__GetValidUnitList(unitKeys);

			// unit
			MK_INDEXING_LOOP(baseUnitKeys, j)
			{
				const MkHashStr& currKey = baseUnitKeys[j];
				currPos += tkDir;

				minCell.CompareAndKeepMin(currPos);
				maxCell.CompareAndKeepMax(currPos);
				excelWritter.SetBorder(currPos.x, currPos.y, 1);

				// �ǹ� �ִ� ���̸� ���
				if (unitKeys.FindFirstInclusion(MkArraySection(0), currKey) != MKDEF_ARRAY_ERROR)
				{
					switch (currNode.GetUnitType(currKey))
					{
					case ePDT_Bool: __TSI_GetAndPutUnitData<bool>::Proceed(currNode, currKey, currPos, minCell, maxCell, excelWritter); break;
					case ePDT_Int: __TSI_GetAndPutUnitData<int>::Proceed(currNode, currKey, currPos, minCell, maxCell, excelWritter); break;
					case ePDT_UnsignedInt: __TSI_GetAndPutUnitData<unsigned int>::Proceed(currNode, currKey, currPos, minCell, maxCell, excelWritter); break;
					case ePDT_DInt: __TSI_GetAndPutUnitData<__int64>::Proceed(currNode, currKey, currPos, minCell, maxCell, excelWritter); break;
					case ePDT_DUnsignedInt: __TSI_GetAndPutUnitData<unsigned __int64>::Proceed(currNode, currKey, currPos, minCell, maxCell, excelWritter); break;
					case ePDT_Float: __TSI_GetAndPutUnitData<float>::Proceed(currNode, currKey, currPos, minCell, maxCell, excelWritter); break;
					case ePDT_Int2: __TSI_GetAndPutUnitData<MkInt2>::Proceed(currNode, currKey, currPos, minCell, maxCell, excelWritter); break;
					case ePDT_Vec2: __TSI_GetAndPutUnitData<MkVec2>::Proceed(currNode, currKey, currPos, minCell, maxCell, excelWritter); break;
					case ePDT_Vec3: __TSI_GetAndPutUnitData<MkVec3>::Proceed(currNode, currKey, currPos, minCell, maxCell, excelWritter); break;
					case ePDT_Str: __TSI_GetAndPutUnitData<MkStr>::Proceed(currNode, currKey, currPos, minCell, maxCell, excelWritter); break;
					case ePDT_ByteArray: __TSI_GetAndPutUnitData<MkByteArray>::Proceed(currNode, currKey, currPos, minCell, maxCell, excelWritter); break;
					}
				}
			}
		}
	}

	// auto fit - all column
	for (unsigned int i=minCell.y; i<=maxCell.y; ++i)
	{
		excelWritter.AutoFit(i);
	}

	// child node
	MK_INDEXING_LOOP(targetNodeList, i)
	{
		if (!_BuildSheet(*node.GetChildNode(targetNodeList[i]), targetSheetList[i], excelWritter))
			return false;
	}
	
	return true;
}
