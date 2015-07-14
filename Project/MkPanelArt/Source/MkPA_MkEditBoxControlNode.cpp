
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkKeywordFilter.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkFontManager.h"
#include "MkPA_MkTextNode.h"
#include "MkPA_MkHiddenEditBox.h"
#include "MkPA_MkEditBoxControlNode.h"


const MkHashStr MkEditBoxControlNode::SelectionPanelName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Selection";
const MkHashStr MkEditBoxControlNode::TextPanelName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Text";
const MkHashStr MkEditBoxControlNode::CursorPanelName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Cursor";

const MkHashStr MkEditBoxControlNode::ArgKey_Text = L"Text";

const MkHashStr MkEditBoxControlNode::ObjKey_History = L"History";
const MkHashStr MkEditBoxControlNode::ObjKey_TextString = L"TextString";

#define MKDEF_NORMAL_CURSOR_CHAR L"│"

//------------------------------------------------------------------------------------------------//

MkEditBoxControlNode* MkEditBoxControlNode::CreateChildNode(MkSceneNode* parentNode, const MkHashStr& childNodeName)
{
	MkEditBoxControlNode* node = __TSI_SceneNodeDerivedInstanceOp<MkEditBoxControlNode>::Alloc(parentNode, childNodeName);
	MK_CHECK(node != NULL, childNodeName.GetString() + L" MkEditBoxControlNode 생성 실패") {}
	return node;
}

//------------------------------------------------------------------------------------------------//

void MkEditBoxControlNode::SetSingleLineEditBox
(const MkHashStr& themeName, MkWindowThemeData::eFrameType frameType, float length, const MkStr& initMsg, bool useHistory)
{
	m_FrameType = frameType;
	float frameSize = MK_STATIC_RES.GetWindowThemeSet().GetFrameSize(themeName, m_FrameType);
	
	// background
	SetThemeName(themeName);
	SetComponentType(MkWindowThemeData::eCT_DefaultBox);
	SetClientSize(MkFloat2(GetMax<float>(length, frameSize), frameSize));
	SetFormState(MkWindowThemeFormData::eS_Default);

	// default
	_DefaultSetUp(useHistory);

	// init
	SetText(initMsg);
}

void MkEditBoxControlNode::SetText(const MkStr& msg)
{
	int selPos = static_cast<int>(msg.GetSize());
	__UpdateText(msg, selPos, selPos);

	MK_EDIT_BOX.NotifyTextChange(this);
}

void MkEditBoxControlNode::CommitText(void)
{
	MkDataNode arg;
	arg.CreateUnit(ArgKey_Text, m_Text);
	StartNodeReportTypeEvent(ePA_SNE_TextCommitted, &arg);
	
	if (m_UseHistory && (!m_Text.Empty()))
	{
		m_MessageHistory.Record(m_Text);
	}
}

void MkEditBoxControlNode::Save(MkDataNode& node) const
{
	// text/cursor/seletion panel 제외
	static MkArray<MkHashStr> exceptions(3);
	if (exceptions.Empty())
	{
		exceptions.PushBack(SelectionPanelName);
		exceptions.PushBack(TextPanelName);
		exceptions.PushBack(CursorPanelName);
	}
	_AddExceptionList(node, SystemKey_PanelExceptions, exceptions);

	// run
	MkWindowBaseNode::Save(node);
}

MKDEF_DECLARE_SCENE_CLASS_KEY_IMPLEMENTATION(MkEditBoxControlNode);

void MkEditBoxControlNode::SetObjectTemplate(MkDataNode& node)
{
	MkWindowBaseNode::SetObjectTemplate(node);

	node.CreateUnit(ObjKey_History, false);
	node.CreateUnit(ObjKey_TextString, MkStr::EMPTY);
}

void MkEditBoxControlNode::LoadObject(const MkDataNode& node)
{
	MkWindowBaseNode::LoadObject(node);

	// default
	bool useHistory = false;
	node.GetData(ObjKey_History, useHistory, 0);
	_DefaultSetUp(useHistory);

	// text
	MkStr text;
	if (node.GetData(ObjKey_TextString, text, 0))
	{
		SetText(text);
	}
}

void MkEditBoxControlNode::SaveObject(MkDataNode& node) const
{
	MkWindowBaseNode::SaveObject(node);

	node.SetData(ObjKey_History, m_UseHistory, 0);
	node.SetData(ObjKey_TextString, m_Text, 0);
}

MkEditBoxControlNode::MkEditBoxControlNode(const MkHashStr& name) : MkWindowBaseNode(name)
{
	m_UseHistory = false;
	m_SelStart = 0;
	m_SelEnd = 0;
}

void MkEditBoxControlNode::__GainInputFocus(void)
{
	SetComponentType(MkWindowThemeData::eCT_NoticeBox);

	_UpdateCursorAndSelection();
}

void MkEditBoxControlNode::__LostInputFocus(void)
{
	SetComponentType(MkWindowThemeData::eCT_DefaultBox);

	_SetPanelEnable(CursorPanelName, false);
	_SetPanelEnable(SelectionPanelName, false);
}

void MkEditBoxControlNode::__UpdateText(const MkStr& msg, int selStart, int selEnd)
{
	// text panel 확인
	if (!PanelExist(TextPanelName))
		return;

	MkPanel* textPanel = GetPanel(TextPanelName);
	MkTextNode* textNode = textPanel->GetTextNodePtr();
	if (textNode == NULL)
		return;

	// 비속어 필터링
	MkStr safeMsg;
	MK_KEYWORD_FILTER.CheckSlang(msg, safeMsg);

	// text 갱신
	if (safeMsg != m_Text)
	{
		m_Text = safeMsg;
		textNode->SetText(m_Text);
		textPanel->BuildAndUpdateTextCache();

		// report event
		MkDataNode arg;
		arg.CreateUnit(ArgKey_Text, m_Text);
		StartNodeReportTypeEvent(ePA_SNE_TextModified, &arg);
	}

	// cursor & selection 갱신
	if ((selStart != m_SelStart) || (selEnd != m_SelEnd))
	{
		// scroll position 갱신
		float scrollPos = textPanel->GetPixelScrollPosition().x;
		float textLength = static_cast<float>(textNode->GetWholePixelSize().x);
		float panelWidth = textPanel->GetPanelSize().x;

		// text가 panel 크기보다 클 경우 적절한 scroll position 필요
		if (textLength > panelWidth)
		{
			int lookPos = -1; // 커서 변화를 통한 기준 위치 탐색
			if (selStart == selEnd) // 현재 일반 커서
			{
				if ((m_SelStart != m_SelEnd) || (selEnd != m_SelEnd)) // 선택 영역이었거나 바뀌거나 일반 커서 상태에서 이동했으면
				{
					lookPos = selEnd;
				}
			}
			else // 현재 선택 영역. start/end 중 변화가 있는 쪽이 기준 위치가 됨
			{
				if (selStart != m_SelStart)
				{
					lookPos = selStart;
				}
				else if (selEnd != m_SelEnd)
				{
					lookPos = selEnd;
				}
			}

			if (lookPos >= 0) // 기준 위치 변화가 존재하면
			{
				float issuePos = _GetTextLength(textNode->GetFontType(), m_Text, 0, lookPos);

				if (issuePos < scrollPos) // 기준 위치가 scroll pos보다 작을 경우
				{
					scrollPos = issuePos;
				}
				else if (issuePos > (scrollPos + panelWidth)) // 기준 위치가 허용 범위를 넘어갔을 경우
				{
					scrollPos = issuePos - panelWidth;
				}
			}
		}
		else
		{
			scrollPos = 0.f;
		}

		// offset
		textPanel->SetPixelScrollPosition(MkFloat2(scrollPos, 0.f));

		// update cursor info
		m_SelStart = selStart;
		m_SelEnd = selEnd;

		_UpdateCursorAndSelection();
	}
}

void MkEditBoxControlNode::__StepBackMsgHistory(void)
{
	if (m_UseHistory)
	{
		SetText(m_MessageHistory.StepBack());
	}
}

void MkEditBoxControlNode::__StepForwardMsgHistory(void)
{
	if (m_UseHistory)
	{
		MkStr newMsg;
		if (m_MessageHistory.StepForward(newMsg))
		{
			SetText(newMsg);
		}
	}
}

void MkEditBoxControlNode::__ToggleNormalCursor(void)
{
	if (m_SelStart == m_SelEnd)
	{
		MkPanel* panel = GetPanel(CursorPanelName);
		if (panel != NULL)
		{
			panel->SetVisible(!panel->GetVisible());
		}
	}
}

//------------------------------------------------------------------------------------------------//

void MkEditBoxControlNode::_DefaultSetUp(bool useHistory)
{
	// history
	m_UseHistory = useHistory;
	if (m_UseHistory)
	{
		m_MessageHistory.SetUp(64);
	}

	// create panels
	MkFloat2 localPos = _GetFormMargin();
	const MkArray<MkHashStr>& editTextNode = MK_STATIC_RES.GetWindowThemeSet().GetEditTextNode(GetThemeName(), m_FrameType);

	// selection panel
	MkPanel& selPanel = CreatePanel(SelectionPanelName);
	selPanel.SetLocalPosition(localPos);
	selPanel.SetLocalDepth(-0.1f); // 배경과 겹치지 않도록 0.1f만큼 앞에 위치
	selPanel.SetSmallerSourceOp(MkPanel::eExpandSource);
	selPanel.SetBiggerSourceOp(MkPanel::eReduceSource);
	selPanel.SetPanelSize(GetClientRect().size);
	selPanel.SetVisible(false);

	const MkWindowThemeFormData* selZoneFD = MK_STATIC_RES.GetWindowThemeSet().GetFormData(GetThemeName(), MkWindowThemeData::eCT_YellowZone, MkHashStr::EMPTY);
	selPanel.SetTexture(MK_STATIC_RES.GetWindowThemeSet().GetImageFilePath(GetThemeName()),
		(selZoneFD == NULL) ? MkHashStr::EMPTY : selZoneFD->GetSubsetOrSequenceName(MkWindowThemeFormData::eS_Default, MkWindowThemeUnitData::eP_MC));

	// text panel
	MkPanel& textPanel = CreatePanel(TextPanelName);
	textPanel.SetLocalPosition(localPos);
	textPanel.SetLocalDepth(-0.2f); // selection panel과 겹치지 않도록 0.1f만큼 앞에 위치
	textPanel.SetSmallerSourceOp(MkPanel::eAttachToLeftTop);
	textPanel.SetBiggerSourceOp(MkPanel::eCutSource);
	textPanel.SetPanelSize(GetClientRect().size);
	textPanel.SetTextNode(editTextNode, false);

	// cursor panel
	MkPanel& cursorPanel = CreatePanel(CursorPanelName);
	cursorPanel.SetLocalPosition(localPos);
	cursorPanel.SetLocalDepth(-0.3f); // text panel과 겹치지 않도록 0.1f만큼 앞에 위치
	cursorPanel.SetTextNode(editTextNode, false);
	cursorPanel.GetTextNodePtr()->SetText(MKDEF_NORMAL_CURSOR_CHAR);
	cursorPanel.BuildAndUpdateTextCache();
	cursorPanel.SetVisible(false);
}

MkFloat2 MkEditBoxControlNode::_GetFormMargin(void) const
{
	MkFloat2 margin;
	const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(GetThemeName(), GetComponentType(), MkHashStr::EMPTY);
	if (formData != NULL)
	{
		margin.x = formData->GetLeftMargin();
		margin.y = formData->GetBottomMargin();
	}
	return margin;
}

float MkEditBoxControlNode::_GetTextLength(const MkHashStr& fontType, const MkStr& text, int beginPos, int endPos) const
{
	if ((beginPos >= 0) && (beginPos < endPos))
	{
		MkStr subset;
		text.GetSubStr(MkArraySection(static_cast<unsigned int>(beginPos), static_cast<unsigned int>(endPos - beginPos)), subset);
		return static_cast<float>(MK_FONT_MGR.GetTextSize(fontType, subset, false).x);
	}
	return 0.f;
}

MkPanel* MkEditBoxControlNode::_SetPanelEnable(const MkHashStr& name, bool enable)
{
	MkPanel* panel = GetPanel(name);
	if (panel != NULL)
	{
		panel->SetVisible(enable);
	}
	return panel;
}

void MkEditBoxControlNode::_UpdateCursorAndSelection(void)
{
	MkPanel* textPanel = GetPanel(TextPanelName);
	if (textPanel != NULL)
	{
		MkTextNode* textNode = textPanel->GetTextNodePtr();
		if (textNode != NULL)
		{
			const MkHashStr& fontType = textNode->GetFontType();
			float scrollPos = textPanel->GetPixelScrollPosition().x;
			float panelWidth = textPanel->GetPanelSize().x;
			MkFloat2 localPos = _GetFormMargin();

			// normal cursor
			if (m_SelStart == m_SelEnd)
			{
				MkPanel* panel = _SetPanelEnable(CursorPanelName, true);
				_SetPanelEnable(SelectionPanelName, false);

				localPos.x += _GetTextLength(fontType, m_Text, 0, m_SelStart) - scrollPos;
				localPos.x -= panel->GetTextureSize().x * 0.5f;
				panel->SetLocalPosition(localPos);
			}
			// selection
			else
			{
				_SetPanelEnable(CursorPanelName, false);
				MkPanel* panel = _SetPanelEnable(SelectionPanelName, true);

				float startPos = Clamp<float>(_GetTextLength(fontType, m_Text, 0, m_SelStart) - scrollPos, 0.f, panelWidth);
				float endPos = Clamp<float>(_GetTextLength(fontType, m_Text, 0, m_SelEnd) - scrollPos, 0.f, panelWidth);

				localPos.x += GetMin<float>(startPos, endPos);
				panel->SetLocalPosition(localPos);

				MkFloat2 size = panel->GetPanelSize();
				size.x = GetMax<float>(startPos, endPos) - GetMin<float>(startPos, endPos);
				panel->SetPanelSize(size);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------//