
#include "MkCore_MkDataNode.h"
#include "MkPA_MkEditBoxControlNode.h"
#include "MkPA_MkDropDownListControlNode.h"
#include "MkPA_MkListBoxControlNode.h"

#include "AppWindowCreationHelper.h"
#include "SystemWindowBodyNode.h"


//------------------------------------------------------------------------------------------------//

void SystemWindowBodyNode::CreateControls(void)
{
	// load, save, clear
	float currLineHeight = m_ClientRect.size.y - 46.f;
	AppWindowCreationHelper::AddNormalButton(this, L"Load", L"파일 열기(png/mii)", 5.f, currLineHeight, 118.f);
	AppWindowCreationHelper::AddNormalButton(this, L"Reload", L"파일 다시 읽기", 133.f, currLineHeight, 118.f);
	
	currLineHeight -= 30.f;
	AppWindowCreationHelper::AddNormalButton(this, L"SaveMII", L"mii 파일 저장", 5.f, currLineHeight, 118.f);
	AppWindowCreationHelper::AddCheckBoxWindow(this, L"SaveMIIToText", L"텍스트로 저장", 140.f, currLineHeight + 4.f, false);

	// group name
	currLineHeight -= 28.f;
	AppWindowCreationHelper::AddStaticTextPanel(this, L"ST:GroupName", L"★ 그룹 이름", 5.f, currLineHeight);

	currLineHeight -= 26.f;
	AppWindowCreationHelper::AddEditBoxWindow(this, L"GroupNameEB", MkStr::EMPTY, 5.f, currLineHeight, 118.f);
	AppWindowCreationHelper::AddNormalButton(this, L"ApplyGroupName", L"그룹명 적용", 133.f, currLineHeight - 4.f, 118.f);

	// subset
	currLineHeight -= 32.f;
	AppWindowCreationHelper::AddStaticTextPanel(this, L"ST:FocusSS", L"★ 서브셋", 5.f, currLineHeight);

	currLineHeight -= 26.f;
	AppWindowCreationHelper::AddStaticTextPanel(this, L"ST:SubsetName", L"이름", 10.f, currLineHeight + 2.f);
	AppWindowCreationHelper::AddEditBoxWindow(this, L"SubsetNameEB", MkStr::EMPTY, 36.f, currLineHeight, 158.f);
	AppWindowCreationHelper::AddNormalButton(this, L"ApplySubsetName", L"변경", 207.f, currLineHeight - 4.f, 44.f);

	currLineHeight -= 30.f;
	AppWindowCreationHelper::AddStaticTextPanel(this, L"ST:SSPos", L"위치", 10.f, currLineHeight + 2.f);
	AppWindowCreationHelper::AddEditBoxWindow(this, L"SubsetPosEB", MkStr::EMPTY, 36.f, currLineHeight, 60.f);
	AppWindowCreationHelper::AddStaticTextPanel(this, L"ST:SSSize", L"크기", 108.f, currLineHeight + 2.f);
	AppWindowCreationHelper::AddEditBoxWindow(this, L"SubsetSizeEB", MkStr::EMPTY, 134.f, currLineHeight, 60.f);
	AppWindowCreationHelper::AddNormalButton(this, L"ApplySubset", L"적용", 207.f, currLineHeight - 4.f, 44.f);

	// sequence
	currLineHeight -= 32.f;
	AppWindowCreationHelper::AddStaticTextPanel(this, L"ST:SeqSS", L"★ 시퀀스", 5.f, currLineHeight);

	currLineHeight -= 26.f;
	AppWindowCreationHelper::AddStaticTextPanel(this, L"ST:SeqName", L"이름", 10.f, currLineHeight + 2.f);
	AppWindowCreationHelper::AddEditBoxWindow(this, L"SeqNameEB", MkStr::EMPTY, 36.f, currLineHeight, 107.f);
	AppWindowCreationHelper::AddNormalButton(this, L"AddSeqName", L"추가", 153.f, currLineHeight - 4.f, 44.f);
	AppWindowCreationHelper::AddNormalButton(this, L"ApplySeqName", L"변경", 207.f, currLineHeight - 4.f, 44.f);

	currLineHeight -= 30.f;
	AppWindowCreationHelper::AddDropdownListWindow(this, L"SeqDDList", 10.f, currLineHeight, 133.f, 6);
	AppWindowCreationHelper::AddNormalButton(this, L"EditCurrSeq", L"편집", 153.f, currLineHeight - 4.f, 44.f);
	AppWindowCreationHelper::AddNormalButton(this, L"DelCurrSeq", L"삭제", 207.f, currLineHeight - 4.f, 44.f);

	currLineHeight -= 34.f;
	AppWindowCreationHelper::AddNormalButton(this, L"ToggleCurrSeq", L"재생 윈도우 토글", 153.f, currLineHeight, 98.f);
}

void SystemWindowBodyNode::SendNodeReportTypeEvent(ePA_SceneNodeEvent eventType, MkArray<MkHashStr>& path, MkDataNode* argument)
{
	if (eventType == ePA_SNE_CursorLBtnReleased)
	{
		if ((path.GetSize() == 1))
		{
			if (path[0].Equals(0, L"Load"))
			{
				m_BitField.Set(eWE_Load);
			}
			else if (path[0].Equals(0, L"Reload"))
			{
				m_BitField.Set(eWE_Reload);
			}
			else if (path[0].Equals(0, L"SaveMII"))
			{
				m_BitField.Set(eWE_SaveMII);
			}
			else if (path[0].Equals(0, L"ApplyGroupName"))
			{
				m_BitField.Set(eWE_ApplyGroupName);
			}
			else if (path[0].Equals(0, L"ApplySubsetName"))
			{
				m_BitField.Set(eWE_ApplySubsetName);
			}
			else if (path[0].Equals(0, L"ApplySubset"))
			{
				MkEditBoxControlNode* ebPosNode = dynamic_cast<MkEditBoxControlNode*>(GetChildNode(L"SubsetPosEB"));
				MkEditBoxControlNode* ebSizeNode = dynamic_cast<MkEditBoxControlNode*>(GetChildNode(L"SubsetSizeEB"));

				if ((ebPosNode != NULL) && (ebSizeNode != NULL) &&
					_UpdateSubsetTransform(ebPosNode->GetText(), m_SubsetPosition) &&
					_UpdateSubsetTransform(ebSizeNode->GetText(), m_SubsetSize))
				{
					m_BitField.Set(eWE_ApplySubsetRect);
				}
			}
			else if (path[0].Equals(0, L"AddSeqName"))
			{
				MkEditBoxControlNode* seqNode = dynamic_cast<MkEditBoxControlNode*>(GetChildNode(L"SeqNameEB"));
				if ((seqNode != NULL) && (!seqNode->GetText().Empty()))
				{
					m_TargetSequenceStr.Clear();
					m_TargetSequenceStr.PushBack(seqNode->GetText());
					m_BitField.Set(eWE_AddSeqName);
				}
			}
			else if (path[0].Equals(0, L"ApplySeqName"))
			{
				MkEditBoxControlNode* seqNode = dynamic_cast<MkEditBoxControlNode*>(GetChildNode(L"SeqNameEB"));
				MkDropDownListControlNode* listNode = dynamic_cast<MkDropDownListControlNode*>(GetChildNode(L"SeqDDList"));
				if ((seqNode != NULL) && (!seqNode->GetText().Empty()) && (listNode != NULL))
				{
					MkHashStr itemName = seqNode->GetText();
					MkHashStr oldName = listNode->GetTargetItemKey();
					if (itemName != oldName)
					{
						m_TargetSequenceStr.Clear();
						m_TargetSequenceStr.PushBack(oldName);
						m_TargetSequenceStr.PushBack(itemName);
						m_BitField.Set(eWE_ApplySeqName);
					}
				}
			}
			else if (path[0].Equals(0, L"EditCurrSeq"))
			{
				MkDropDownListControlNode* listNode = dynamic_cast<MkDropDownListControlNode*>(GetChildNode(L"SeqDDList"));
				if ((listNode != NULL) && (!listNode->GetTargetItemKey().Empty()))
				{
					m_TargetSequenceStr.Clear();
					m_TargetSequenceStr.PushBack(listNode->GetTargetItemKey());
					m_BitField.Set(eWE_EditCurrSeq);
				}
			}
			else if (path[0].Equals(0, L"DelCurrSeq"))
			{
				MkDropDownListControlNode* listNode = dynamic_cast<MkDropDownListControlNode*>(GetChildNode(L"SeqDDList"));
				if ((listNode != NULL) && (!listNode->GetTargetItemKey().Empty()))
				{
					MkHashStr oldName = listNode->GetTargetItemKey();
					listNode->RemoveItem(oldName);

					m_TargetSequenceStr.Clear();
					m_TargetSequenceStr.PushBack(oldName);
					m_BitField.Set(eWE_DelCurrSeq);
				}
			}
			else if (path[0].Equals(0, L"ToggleCurrSeq"))
			{
				MkDropDownListControlNode* listNode = dynamic_cast<MkDropDownListControlNode*>(GetChildNode(L"SeqDDList"));
				if ((listNode != NULL) && (!listNode->GetTargetItemKey().Empty()))
				{
					m_TargetSequenceStr.Clear();
					m_TargetSequenceStr.PushBack(listNode->GetTargetItemKey());
					m_BitField.Set(eWE_ToggleCurrSeq);
				}
			}
		}
	}
	else if (eventType == ePA_SNE_DropDownItemSet)
	{
		if ((path.GetSize() == 1) && path[0].Equals(0, L"SeqDDList"))
		{
			MkHashStr buffer;
			if (argument->GetDataEx(MkListBoxControlNode::ArgKey_Item, buffer, 0))
			{
				MkEditBoxControlNode* seqNode = dynamic_cast<MkEditBoxControlNode*>(GetChildNode(L"SeqNameEB"));
				if (seqNode != NULL)
				{
					if (seqNode->GetText() != buffer.GetString())
					{
						seqNode->SetText(buffer.GetString());
					}
				}
			}
		}
	}
	else if (eventType == ePA_SNE_TextCommitted)
	{
		if ((path.GetSize() == 1))
		{
			MkStr buffer;
			if (argument->GetData(MkEditBoxControlNode::ArgKey_Text, buffer, 0) && (!buffer.Empty()))
			{
				if (path[0].Equals(0, L"GroupNameEB"))
				{
					m_BitField.Set(eWE_ApplyGroupName);
				}
				else if (path[0].Equals(0, L"SubsetNameEB"))
				{
					m_BitField.Set(eWE_ApplySubsetName);
				}
				else if (path[0].Equals(0, L"SubsetPosEB") || path[0].Equals(0, L"SubsetSizeEB"))
				{
					MkEditBoxControlNode* ebPosNode = dynamic_cast<MkEditBoxControlNode*>(GetChildNode(L"SubsetPosEB"));
					MkEditBoxControlNode* ebSizeNode = dynamic_cast<MkEditBoxControlNode*>(GetChildNode(L"SubsetSizeEB"));

					if ((ebPosNode != NULL) && (ebSizeNode != NULL) &&
						_UpdateSubsetTransform(ebPosNode->GetText(), m_SubsetPosition) &&
						_UpdateSubsetTransform(ebSizeNode->GetText(), m_SubsetSize))
					{
						m_BitField.Set(eWE_ApplySubsetRect);
					}
				}
			}
		}
	}

	MkBodyFrameControlNode::SendNodeReportTypeEvent(eventType, path, argument);
}

MkBitField32 SystemWindowBodyNode::ConsumeEvent(void)
{
	MkBitField32 result = m_BitField;
	m_BitField.Clear();
	return result;
}

bool SystemWindowBodyNode::_UpdateSubsetTransform(const MkStr& text, MkInt2& buffer)
{
	MkArray<MkStr> tokens;
	MkStr txt = text;
	txt.RemoveBlank();
	if ((txt.Tokenize(tokens, L",") == 2) &&
		tokens[0].IsDigit() &&
		tokens[1].IsDigit())
	{
		buffer.x = tokens[0].ToInteger();
		buffer.y = tokens[1].ToInteger();
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------//
