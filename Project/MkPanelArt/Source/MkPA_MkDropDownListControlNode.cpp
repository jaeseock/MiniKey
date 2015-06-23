
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkListBoxControlNode.h"
#include "MkPA_MkDropDownListControlNode.h"


const MkHashStr MkDropDownListControlNode::SelectionNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Selection";
const MkHashStr MkDropDownListControlNode::DropDownBtnNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"DDBtn";
const MkHashStr MkDropDownListControlNode::ListBoxNodeName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"ListBox";

const MkHashStr MkDropDownListControlNode::ObjKey_MaxOnePageItemSize(L"MaxOnePageItemSize");
const MkHashStr MkDropDownListControlNode::ObjKey_TargetItemKey(L"TargetItemKey");


//------------------------------------------------------------------------------------------------//

MkDropDownListControlNode* MkDropDownListControlNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkDropDownListControlNode* node = __TSI_SceneNodeDerivedInstanceOp<MkDropDownListControlNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkDropDownListControlNode ���� ����") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkDropDownListControlNode::SetDropDownList(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, int onePageItemSize)
{
	m_WindowFrameType = frameType;
	float frameSize = MK_STATIC_RES.GetWindowThemeSet().GetFrameSize(themeName, m_WindowFrameType);
	length = GetMax<float>(length, frameSize);

	// background
	SetThemeName(themeName);
	SetComponentType(MkWindowThemeData::eCT_DefaultBox);
	SetClientSize(MkFloat2(length, frameSize));
	SetFormState(MkWindowThemeFormData::eS_Default);

	// seletion tag & drop down btn
	_BuildSelectionAndDropDownButton();

	// list box
	m_ListBoxControlNode = ChildExist(ListBoxNodeName) ?
		dynamic_cast<MkListBoxControlNode*>(GetChildNode(ListBoxNodeName)) : MkListBoxControlNode::CreateChildNode(this, ListBoxNodeName);

	if (m_ListBoxControlNode != NULL)
	{
		const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(themeName, MkWindowThemeData::eCT_DefaultBox, MkHashStr::EMPTY);
		if (formData != NULL)
		{
			length -= formData->GetLeftMargin();
			length -= formData->GetRightMargin();
		}

		m_ListBoxControlNode->SetListBox(GetThemeName(), onePageItemSize, length, m_WindowFrameType);
		m_ListBoxControlNode->SetLocalDepth(-0.1f); // ���� layer�� ��ġ�� ���� ���ϱ� ���� 0.1f��ŭ �տ� ��ġ
		m_ListBoxControlNode->SetAlignmentPivotIsWindowRect(true); // wndow rect ����
		m_ListBoxControlNode->SetAlignmentPosition(eRAP_LeftUnder);
		m_ListBoxControlNode->SetVisible(false);

		m_MaxOnePageItemSize = m_ListBoxControlNode->GetOnePageItemSize();
	}
}

void MkDropDownListControlNode::AddItem(const MkHashStr& uniqueKey, const MkStr& message)
{
	if (m_ListBoxControlNode != NULL)
	{
		MkWindowTagNode* tagNode = m_ListBoxControlNode->AddItem(uniqueKey, message);

		// ���� target item�� ������ ���� ��ϵ� item�� ������� ����
		if ((tagNode != NULL) && m_TargetItemKey.Empty())
		{
			SetTargetItemKey(uniqueKey);
		}
	}
}

bool MkDropDownListControlNode::RemoveItem(const MkHashStr& uniqueKey)
{
	bool ok = false;
	if (m_ListBoxControlNode != NULL)
	{
		ok = m_ListBoxControlNode->RemoveItem(uniqueKey);

		// ���� item�� target item�� ��� ���
		if (ok && (uniqueKey == m_TargetItemKey))
		{
			SetTargetItemKey(MkHashStr::EMPTY);
		}
	}
	return ok;
}

void MkDropDownListControlNode::ClearAllItems(void)
{
	if (m_ListBoxControlNode != NULL)
	{
		m_ListBoxControlNode->ClearAllItems();

		SetTargetItemKey(MkHashStr::EMPTY);
	}
}

unsigned int MkDropDownListControlNode::GetItemCount(void) const
{
	return (m_ListBoxControlNode == NULL) ? 0 : m_ListBoxControlNode->GetItemCount();
}

unsigned int MkDropDownListControlNode::GetItemKeyList(MkArray<MkHashStr>& keyList) const
{
	return (m_ListBoxControlNode == NULL) ? 0 : m_ListBoxControlNode->GetItemKeyList(keyList);
}

void MkDropDownListControlNode::SetTargetItemKey(const MkHashStr& uniqueKey)
{
	if (uniqueKey != m_TargetItemKey)
	{
		m_TargetItemKey = uniqueKey;

		if (m_ListBoxControlNode != NULL)
		{
			const MkWindowTagNode* tagNode = m_TargetItemKey.Empty() ? NULL : m_ListBoxControlNode->GetItemTag(m_TargetItemKey);
			_SetTextToSelectionTag(tagNode);
		}

		MkDataNode arg;
		arg.CreateUnitEx(MkListBoxControlNode::ArgKey_Item, m_TargetItemKey);
		StartNodeReportTypeEvent(ePA_SNE_DropDownItemSet, &arg);
	}
}

void MkDropDownListControlNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if ((m_ListBoxControlNode != NULL) && (m_MaxOnePageItemSize > 0))
	{
		// left cursor pressed�� �ش� window�� drop down button�̸� list box�� toggle�ϰ� event �Ҹ�
		if (((eventType == ePA_SNE_CursorLBtnPressed) || (eventType == ePA_SNE_CursorLBtnDBClicked)) && (path.GetSize() == 1) && (path[0] == DropDownBtnNodeName))
		{
			int itemCnt = static_cast<int>(m_ListBoxControlNode->GetItemCount());
			if (itemCnt > 0)
			{
				// report
				MkArray<MkHashStr> targetPath;
				targetPath.PushBack(ListBoxNodeName);
				MkDataNode arg;
				arg.CreateUnitEx(MkWindowBaseNode::ArgKey_ExclusiveWindow, targetPath);

				if (!m_ListBoxControlNode->GetVisible()) // close -> open
				{
					// item size�� �� ������ �з����� ������ ������ ���
					m_ListBoxControlNode->SetOnePageItemSize(GetMin<int>(itemCnt, m_MaxOnePageItemSize));

					MkArray<MkHashStr> expPath;
					expPath.PushBack(DropDownBtnNodeName);
					arg.CreateUnitEx(MkWindowBaseNode::ArgKey_ExclusiveException, expPath);
				}

				StartNodeReportTypeEvent(ePA_SNE_ToggleExclusiveWindow, &arg);
			}
			return;
		}

		// item btn pressed�� target item ����, list box �ݰ� event �Ҹ�
		if (((eventType == ePA_SNE_ItemLBtnPressed) || (eventType == ePA_SNE_ItemLBtnDBClicked)) && (path.GetSize() == 1) && (path[0] == ListBoxNodeName) && (argument != NULL))
		{
			MkHashStr targetItem;
			if (argument->GetDataEx(MkListBoxControlNode::ArgKey_Item, targetItem, 0))
			{
				SetTargetItemKey(targetItem);

				// report
				MkArray<MkHashStr> targetPath;
				targetPath.PushBack(ListBoxNodeName);
				MkDataNode arg;
				arg.CreateUnitEx(MkWindowBaseNode::ArgKey_ExclusiveWindow, targetPath);

				StartNodeReportTypeEvent(ePA_SNE_ToggleExclusiveWindow, &arg);
				return;
			}
		}
	}

	MkWindowBaseNode::SendNodeReportTypeEvent(eventType, path, argument);
}

void MkDropDownListControlNode::Save(MkDataNode& node) const
{
	// selection, drop down btn ����. list box�� ���
	static MkArray<MkHashStr> exceptions(2);
	if (exceptions.Empty())
	{
		exceptions.PushBack(SelectionNodeName);
		exceptions.PushBack(DropDownBtnNodeName);
	}
	_AddExceptionList(node, SystemKey_NodeExceptions, exceptions);

	// list box�� ���� ä�� ����Ǿ�� �ϱ� ������ visible ���¸� invisible�� ������ ���� �� ����
	bool restore = (m_ListBoxControlNode == NULL) ? false : m_ListBoxControlNode->GetVisible();
	if (restore)
	{
		m_ListBoxControlNode->SetVisible(false);
	}
	
	// run
	MkWindowBaseNode::Save(node);

	// visible ����
	if (restore)
	{
		m_ListBoxControlNode->SetVisible(true);
	}
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkDropDownListControlNode);

void MkDropDownListControlNode::SetObjectTemplate(MkDataNode& node)
{
	MkWindowBaseNode::SetObjectTemplate(node);

	node.CreateUnit(ObjKey_MaxOnePageItemSize, static_cast<int>(0));
	node.CreateUnit(ObjKey_TargetItemKey, MkStr::EMPTY);
}

void MkDropDownListControlNode::LoadObject(const MkDataNode& node)
{
	MkWindowBaseNode::LoadObject(node);

	node.GetData(ObjKey_MaxOnePageItemSize, m_MaxOnePageItemSize, 0);
}

void MkDropDownListControlNode::LoadComplete(const MkDataNode& node)
{
	// seletion tag & drop down btn
	_BuildSelectionAndDropDownButton();

	// list box
	m_ListBoxControlNode = dynamic_cast<MkListBoxControlNode*>(GetChildNode(ListBoxNodeName));

	// target item
	MkHashStr targetItemKey;
	if (node.GetDataEx(ObjKey_TargetItemKey, targetItemKey, 0))
	{
		SetTargetItemKey(targetItemKey);
	}
}

void MkDropDownListControlNode::SaveObject(MkDataNode& node) const
{
	MkWindowBaseNode::SaveObject(node);

	node.SetData(ObjKey_MaxOnePageItemSize, m_MaxOnePageItemSize, 0);
	node.SetDataEx(ObjKey_TargetItemKey, m_TargetItemKey, 0);
}

void MkDropDownListControlNode::Clear(void)
{
	m_MaxOnePageItemSize = 0;
	m_ListBoxControlNode = NULL;
	m_TargetItemKey.Clear();

	MkWindowBaseNode::Clear();
}

MkDropDownListControlNode::MkDropDownListControlNode(const MkHashStr& name) : MkWindowBaseNode(name)
{
	m_MaxOnePageItemSize = 0;
	m_ListBoxControlNode = NULL;
}

void MkDropDownListControlNode::_BuildSelectionAndDropDownButton(void)
{
	// seletion tag
	MkWindowTagNode* tagNode = ChildExist(SelectionNodeName) ?
		dynamic_cast<MkWindowTagNode*>(GetChildNode(SelectionNodeName)) : MkWindowTagNode::CreateChildNode(this, SelectionNodeName);

	if (tagNode != NULL)
	{
		tagNode->SetLocalDepth(-0.1f); // bg�� ��ġ�� ���� ���ϱ� ���� 0.1f��ŭ �տ� ��ġ
		tagNode->SetTextName(MK_STATIC_RES.GetWindowThemeSet().GetCaptionTextNode(GetThemeName(), m_WindowFrameType), MkStr::EMPTY);
		tagNode->SetAlignmentPosition(eRAP_LeftCenter);
	}

	// drop down btn
	MkWindowBaseNode* btnNode = ChildExist(DropDownBtnNodeName) ?
		dynamic_cast<MkWindowBaseNode*>(GetChildNode(DropDownBtnNodeName)) : MkWindowBaseNode::CreateChildNode(this, DropDownBtnNodeName);

	if (btnNode != NULL)
	{
		btnNode->SetThemeName(GetThemeName());
		btnNode->SetComponentType(MkWindowThemeData::GetLEDButtonComponent(m_WindowFrameType, MkWindowThemeData::eLED_Blue));
		btnNode->SetFormState(MkWindowThemeFormData::eS_Default);
		btnNode->SetLocalDepth(-0.1f); // bg�� ��ġ�� ���� ���ϱ� ���� 0.1f��ŭ �տ� ��ġ
		btnNode->SetAlignmentPosition(eRAP_RightCenter);
	}
}

void MkDropDownListControlNode::_SetTextToSelectionTag(const MkWindowTagNode* tagNode)
{
	MkStr msg;
	if (tagNode != NULL)
	{
		const MkTextNode* textNode = tagNode->GetTagTextPtr();
		if (textNode != NULL)
		{
			msg = textNode->GetText();
		}
	}

	if (ChildExist(SelectionNodeName))
	{
		MkWindowTagNode* targetNode = dynamic_cast<MkWindowTagNode*>(GetChildNode(SelectionNodeName));
		if (targetNode != NULL)
		{
			MkTextNode* textNode = targetNode->GetTagTextPtr();
			if (textNode != NULL)
			{
				textNode->SetText(msg);
				targetNode->CommitTagText();
			}
		}
	}
}

//------------------------------------------------------------------------------------------------//