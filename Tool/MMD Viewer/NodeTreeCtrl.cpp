
// MkDataNodeEditorDlg.cpp : 구현 파일
//

#include "stdafx.h"

#include "MkCore_MkDataNode.h"
#include "NodeTreeCtrl.h"

#include "MkCore_MkTagDefinitionForDataNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//------------------------------------------------------------------------------------------------//

IMPLEMENT_DYNAMIC(CNodeTreeCtrl, CTreeCtrl)

CNodeTreeCtrl::CNodeTreeCtrl() : CTreeCtrl() {}
CNodeTreeCtrl::~CNodeTreeCtrl() {}

BEGIN_MESSAGE_MAP(CNodeTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

HTREEITEM CNodeTreeCtrl::AddDataNodeItem(HTREEITEM parent, const MkDataNode& node)
{
	TVINSERTSTRUCT ti;
	ti.hInsertAfter = TVI_LAST;
	ti.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	ti.hParent = parent;
	ti.item.iSelectedImage = ti.item.iImage = node.IsTemplateNode() ? 1 : 0;

	MkStr bodyMsg;
	_MakeNodeItemName(node, bodyMsg);
	ti.item.pszText = bodyMsg.GetPtr();

	HTREEITEM childItem = InsertItem(&ti);
	if (childItem != NULL)
	{
		SetItemData(childItem, reinterpret_cast<DWORD_PTR>(&node));
	}
	return childItem;
}

template <class DataType>
class __TSI_ExtractUnitToText
{
public:
	static void Preceed(const MkDataNode& node, const MkHashStr& key, MkArray<MkStr>& buffer)
	{
		MkArray<DataType> datas;
		if (node.GetData(key, datas))
		{
			buffer.Reserve(datas.GetSize());
			MK_INDEXING_LOOP(datas, i)
			{
				buffer.PushBack(MkStr(datas[i]));
			}
		}
	}
};

HTREEITEM CNodeTreeCtrl::AddDataUnitItem(HTREEITEM parent, const MkDataNode& parentNode, const MkHashStr& unitName)
{
	TVINSERTSTRUCT ti;
	ti.hInsertAfter = TVI_LAST;
	ti.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	ti.hParent = parent;

	// icon
	int iconIndex = -1;
	ePrimitiveDataType unitType = parentNode.GetUnitType(unitName);
	bool isDataTypeTag = (unitType == ePDT_Str) ? (unitName == MkDataNode::DataTypeTag) : false;
	
	switch (unitType)
	{
	case ePDT_Bool: iconIndex = 3; break;
	case ePDT_Int: iconIndex = 4; break;
	case ePDT_UnsignedInt: iconIndex = 5; break;
	case ePDT_DInt: iconIndex = 6; break;
	case ePDT_DUnsignedInt: iconIndex = 7; break;
	case ePDT_Float: iconIndex = 8; break;
	case ePDT_Int2: iconIndex = 9; break;
	case ePDT_Vec2: iconIndex = 10; break;
	case ePDT_Vec3: iconIndex = 11; break;
	case ePDT_Str: iconIndex = (isDataTypeTag) ? 2 : 12; break;
	case ePDT_ByteArray: iconIndex = 13; break;
	}
	ti.item.iSelectedImage = ti.item.iImage = iconIndex;

	// text
	MkStr unitText;
	unitText.Reserve(1024);
	if (!isDataTypeTag)
	{
		unitText += MkPrimitiveDataType::GetTag(unitType);
		unitText += L" ";
		unitText += unitName.GetString();
	}

	unsigned int dataSize = parentNode.GetDataSize(unitName);
	if (dataSize > 0)
	{
		if (!isDataTypeTag)
		{
			unitText += L" ";
			unitText += MkTagDefinitionForDataNode::TagForUnitAssign;
			unitText += L" ";
		}

		MkArray<MkStr> strBuffer;
		ConvertUnitValueToText(parentNode, unitName, strBuffer, false);

		if (dataSize == 1)
		{
			unitText += strBuffer[0];
		}
		else // dataSize > 1
		{
			unitText += L"[";
			unitText += dataSize;
			unitText += L"]";

			ti.item.pszText = unitText.GetPtr();
			HTREEITEM unitItem = InsertItem(&ti);
			if (unitItem != NULL)
			{
				AddDataArrayItem(unitItem, strBuffer);
			}
			return unitItem;
		}
	}

	ti.item.pszText = unitText.GetPtr();
	return InsertItem(&ti);
}

void CNodeTreeCtrl::AddDataArrayItem(HTREEITEM parent, const MkArray<MkStr>& values)
{
	TVINSERTSTRUCT ti;
	ti.hInsertAfter = TVI_LAST;
	ti.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	ti.item.iSelectedImage = ti.item.iImage = 14;
	ti.hParent = parent;

	MK_INDEXING_LOOP(values, i)
	{
		const MkStr& currValue = values[i];

		MkStr unitText;
		unitText.Reserve(currValue.GetSize() + 12);
		unitText += L"[";
		unitText += i;
		unitText += L"] ";
		unitText += currValue;

		ti.item.pszText = unitText.GetPtr();
		InsertItem(&ti);
	}
}

bool CNodeTreeCtrl::GetDragFilePath(MkPathName& buffer)
{
	if (m_DragFilePath.Empty())
		return false;

	buffer = m_DragFilePath;
	m_DragFilePath.Clear();
	return true;
}

void CNodeTreeCtrl::ConvertUnitValueToText(const MkDataNode& parentNode, const MkHashStr& unitName, MkArray<MkStr>& strBuffer, bool applyDQMTag)
{
	switch (parentNode.GetUnitType(unitName))
	{
	case ePDT_Bool: __TSI_ExtractUnitToText<bool>::Preceed(parentNode, unitName, strBuffer); break;
	case ePDT_Int: __TSI_ExtractUnitToText<int>::Preceed(parentNode, unitName, strBuffer); break;
	case ePDT_UnsignedInt: __TSI_ExtractUnitToText<unsigned int>::Preceed(parentNode, unitName, strBuffer); break;
	case ePDT_DInt: __TSI_ExtractUnitToText<__int64>::Preceed(parentNode, unitName, strBuffer); break;
	case ePDT_DUnsignedInt: __TSI_ExtractUnitToText<unsigned __int64>::Preceed(parentNode, unitName, strBuffer); break;
	case ePDT_Float: __TSI_ExtractUnitToText<float>::Preceed(parentNode, unitName, strBuffer); break;
	case ePDT_Int2: __TSI_ExtractUnitToText<MkInt2>::Preceed(parentNode, unitName, strBuffer); break;
	case ePDT_Vec2: __TSI_ExtractUnitToText<MkVec2>::Preceed(parentNode, unitName, strBuffer); break;
	case ePDT_Vec3: __TSI_ExtractUnitToText<MkVec3>::Preceed(parentNode, unitName, strBuffer); break;
	case ePDT_Str:
		{
			__TSI_ExtractUnitToText<MkStr>::Preceed(parentNode, unitName, strBuffer);

			MK_INDEXING_LOOP(strBuffer, i)
			{
				MkStr tmp = strBuffer[i];

				// 탈출문자(escape sequence) 반영
				if (applyDQMTag)
				{
					tmp.ReplaceKeyword(L"\"", MkTagDefinitionForDataNode::TagForDQM);
					tmp.ReplaceKeyword(MkTagDefinitionForDataNode::TagForDQM, L"\\\"");
				}

				tmp.ReplaceCRLFtoTag();

				// 문자열의 경우 DQM으로 둘러쌈
				strBuffer[i].Reserve(tmp.GetSize() + 2);
				strBuffer[i] = L"\"";
				strBuffer[i] += tmp;
				strBuffer[i] += L"\"";
			}
		}
		break;
	case ePDT_ByteArray:
		{
			MkArray<MkByteArray> datas;
			if (parentNode.GetData(unitName, datas))
			{
				strBuffer.Reserve(datas.GetSize());
				MK_INDEXING_LOOP(datas, i)
				{
					MkStr& str = strBuffer.PushBack();
					str.ImportByteArray(datas[i]);
				}
			}
		}
		break;
	}
}

void CNodeTreeCtrl::_MakeNodeItemName(const MkDataNode& node, MkStr& buffer)
{
	buffer.Clear();
	buffer.Reserve(256);
	buffer += L"\"";
	buffer += (node.GetNodeName().Empty() && (node.GetParentNode() == NULL)) ? L"< ROOT >" : node.GetNodeName().GetString();
	buffer += L"\"";

	MkHashStr templateName;
	if (node.GetAppliedTemplateName(templateName))
	{
		buffer += L" ";
		buffer += MkTagDefinitionForDataNode::TagForApplyTemplate;
		buffer += L" \"";
		buffer += templateName.GetString();
		buffer += L"\"";
	}
}

void CNodeTreeCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTVCUSTOMDRAW* pcd = (NMTVCUSTOMDRAW*)pNMHDR;
	switch (pcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

    case CDDS_ITEMPREPAINT:
		{
			HTREEITEM currItem = (HTREEITEM)pcd->nmcd.dwItemSpec;
			DWORD_PTR itemData = GetItemData(currItem);
			COLORREF txtColor = RGB(255, 102, 204); // violet. error
			
			// node
			if (itemData != NULL)
			{
				const MkDataNode* node = reinterpret_cast<const MkDataNode*>(itemData);

				// yellow : template node
				// white : 고유 node나 unit. array element
				// green : template이 적용 된 node나 unit이지만 값이 다름
				// gray : template이 적용 된 node나 unit이고 값 차이가 없음
				// light purple : data type tag
				if (node->IsTemplateNode())
				{
					txtColor = RGB(255, 255, 0); // yellow
				}
				else
				{
					if (node->GetTemplateLink() != NULL)
					{
						txtColor = node->__IsValidNode(false) ?
							RGB(0, 255, 0) : // green
							RGB(128, 128, 128); // gray
					}
					else
					{
						txtColor = RGB(255, 255, 255); // white
					}
				}
			}
			// unit
			else
			{
				HTREEITEM parentItem = GetNextItem(currItem, TVGN_PARENT);
				if (parentItem != NULL)
				{
					itemData = GetItemData(parentItem);

					// unit
					if (itemData != NULL)
					{
						MkStr currText = GetItemText(currItem).GetBuffer();

						// data type tag
						if ((!currText.Empty()) && (currText.GetAt(0) == L'\"'))
						{
							txtColor = RGB(255, 128, 255); // light purple
						}
						else
						{
							MkStr typePart;
							unsigned int pos = currText.GetFirstWord(0, typePart);
							if (pos != MKDEF_ARRAY_ERROR)
							{
								MkStr namePart;
								pos = currText.GetFirstWord(pos, namePart);
								if (pos != MKDEF_ARRAY_ERROR)
								{
									MkHashStr unitKey = namePart;
									const MkDataNode* node = reinterpret_cast<const MkDataNode*>(itemData);
									if (node->__IsValidUnit(unitKey))
									{
										txtColor = node->IsDefinedUnitByTemplate(unitKey) ?
											RGB(0, 255, 0) : // green
											RGB(255, 255, 255); // white
									}
									else
									{
										txtColor = RGB(128, 128, 128); // gray
									}
								}
							}
						}
					}
					// array element
					else
					{
						txtColor = RGB(255, 255, 255); // white
					}
				}
			}
			pcd->clrText = txtColor;
			//pcd->clrTextBk = RGB(0, 0, 64);

			*pResult = CDRF_DODEFAULT;
		}
		break;
	}
}

void CNodeTreeCtrl::OnDropFiles(HDROP hDropInfo)
{
	wchar_t buffer[1024] = {0, };
	if (::DragQueryFileW(hDropInfo, 0xffffffff, buffer, 1024) > 0)
	{
		::DragQueryFileW(hDropInfo, 0, buffer, 1024);
		m_DragFilePath = buffer;
	}

	CTreeCtrl::OnDropFiles(hDropInfo);
}

//------------------------------------------------------------------------------------------------//
