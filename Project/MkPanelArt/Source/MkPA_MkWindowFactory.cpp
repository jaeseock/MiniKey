
#include "MkCore_MkCheck.h"
//#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkWindowDispositioner.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowTagNode.h"
#include "MkPA_MkTitleBarControlNode.h"
#include "MkPA_MkBodyFrameControlNode.h"
#include "MkPA_MkWindowFactory.h"

const MkHashStr MkWindowFactory::OneAndOnlyTagName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"OAOTag";
const MkHashStr MkWindowFactory::OkButtonName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Ok";
const MkHashStr MkWindowFactory::CancelButtonName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"Cancel";
const MkHashStr MkWindowFactory::BodyFrameName = MkStr(MKDEF_PA_WIN_CONTROL_PREFIX) + L"BodyFrame";

//------------------------------------------------------------------------------------------------//

void MkWindowFactory::SetThemeName(const MkHashStr& themeName)
{
	m_ThemeName = MK_STATIC_RES.GetWindowThemeSet().IsValidTheme(themeName) ? themeName : MkWindowThemeData::DefaultThemeName;
}

MkWindowTagNode* MkWindowFactory::CreateTextTagNode(const MkHashStr& name, const MkStr& message, MkFloat2& textRegion) const
{
	MkArray<MkHashStr> textName;
	return _CreateTextTagNode(name, message, textName, textRegion);
}

MkWindowTagNode* MkWindowFactory::CreateTextTagNode(const MkHashStr& name, const MkArray<MkHashStr>& message, MkFloat2& textRegion) const
{
	return _CreateTextTagNode(name, MkStr::EMPTY, message, textRegion);
}

MkWindowBaseNode* MkWindowFactory::CreateComponentNode(const MkHashStr& name, MkWindowThemeData::eComponentType componentType) const
{
	MkWindowBaseNode* node = MkWindowBaseNode::CreateChildNode(NULL, name);
	if (node != NULL)
	{
		node->SetThemeName(m_ThemeName);
		node->SetComponentType(componentType);
		node->SetFormState(MkWindowThemeFormData::eS_Default);
		node->SetLocalDepth(-1.f); // 자동으로 1.f만큼 앞에 위치시킴
	}
	return node;
}

MkWindowBaseNode* MkWindowFactory::CreateButtonTypeNode(const MkHashStr& name, MkWindowThemeData::eComponentType componentType, const MkStr& message) const
{
	MkWindowBaseNode* node = NULL;
	const MkWindowThemeFormData* formData = MK_STATIC_RES.GetWindowThemeSet().GetFormData(m_ThemeName, componentType, MkHashStr::EMPTY);
	if ((formData != NULL) && formData->IsButtonFormType())
	{
		// btn node
		node = CreateComponentNode(name, componentType);
		if (node != NULL)
		{
			// tag
			MkFloat2 textRegion;
			MkWindowTagNode* tagNode = CreateTextTagNode(OneAndOnlyTagName, message, textRegion);
			if (tagNode != NULL)
			{
				node->AttachChildNode(tagNode);
				tagNode->SetAlignmentPosition(eRAP_MiddleCenter);
			}

			// client size
			MkFloat2 clientSize = m_MinClientSizeForButton;
			clientSize.CompareAndKeepMax(textRegion); // btn 크기가 text 영역을 포괄하도록 갱신
			node->SetClientSize(clientSize);
		}
	}
	return node;
}

MkWindowBaseNode* MkWindowFactory::CreateNormalButtonNode(const MkHashStr& name, const MkStr& message) const
{
	return CreateButtonTypeNode(name, MkWindowThemeData::eCT_NormalBtn, message);
}

MkWindowBaseNode* MkWindowFactory::CreateOkButtonNode(void) const
{
	return CreateButtonTypeNode(OkButtonName, MkWindowThemeData::eCT_OKBtn, L"OK");
}

MkWindowBaseNode* MkWindowFactory::CreateOkButtonNode(const MkStr& message) const
{
	return CreateButtonTypeNode(OkButtonName, MkWindowThemeData::eCT_OKBtn, message);
}

MkWindowBaseNode* MkWindowFactory::CreateCancelButtonNode(void) const
{
	return CreateButtonTypeNode(CancelButtonName, MkWindowThemeData::eCT_CancelBtn, L"Cancel");
}

MkWindowBaseNode* MkWindowFactory::CreateCancelButtonNode(const MkStr& message) const
{
	return CreateButtonTypeNode(CancelButtonName, MkWindowThemeData::eCT_CancelBtn, message);
}

MkTitleBarControlNode* MkWindowFactory::CreateMessageBox
(const MkHashStr& name, const MkStr& title, const MkStr& desc, MkWindowBaseNode* callBackWindow, eMsgBoxType boxType, eMsgBoxButton btnType) const
{
	MkArray<MkHashStr> emptyArray;
	return _CreateMessageBox(name, title, emptyArray, desc, emptyArray, callBackWindow, boxType, btnType);
	
}

MkTitleBarControlNode* MkWindowFactory::CreateMessageBox
(const MkHashStr& name, const MkArray<MkHashStr>& title, const MkArray<MkHashStr>& desc, MkWindowBaseNode* callBackWindow, eMsgBoxType boxType, eMsgBoxButton btnType) const
{
	return _CreateMessageBox(name, MkStr::EMPTY, title, MkStr::EMPTY, desc, callBackWindow, boxType, btnType);
}

bool MkWindowFactory::IsOkButton(const MkArray<MkHashStr>& callerPath, const MkHashStr& msgBoxName)
{
	return _IsMsgBoxButton(callerPath, msgBoxName, OkButtonName);
}

bool MkWindowFactory::IsCancelButton(const MkArray<MkHashStr>& callerPath, const MkHashStr& msgBoxName)
{
	return _IsMsgBoxButton(callerPath, msgBoxName, CancelButtonName);
}

MkWindowFactory::MkWindowFactory()
{
	m_ThemeName = MkWindowThemeData::DefaultThemeName;
	m_FrameType = MkWindowThemeData::eFT_Small;

	float frameSize = MK_STATIC_RES.GetWindowThemeSet().GetFrameSize(m_ThemeName, m_FrameType);
	m_MinClientSizeForButton = MkFloat2(80.f, frameSize);
}

//------------------------------------------------------------------------------------------------//

MkWindowTagNode* MkWindowFactory::_CreateTextTagNode(const MkHashStr& name, const MkStr& message, const MkArray<MkHashStr>& textName, MkFloat2& textRegion) const
{
	MkWindowTagNode* node = MkWindowTagNode::CreateChildNode(NULL, name);
	if (node != NULL)
	{
		node->SetLocalDepth(-0.1f); // parent와 겹치는 것을 피하기 위해 0.1f만큼 앞에 위치
		
		if (textName.Empty())
		{
			node->SetTextName(MK_STATIC_RES.GetWindowThemeSet().GetCaptionTextNode(m_ThemeName, m_FrameType), message); // message 삽입
		}
		else
		{
			node->SetTextName(textName); // text name 삽입
		}

		// text 영역의 크기를 반환
		const MkInt2& iTextSize = node->GetTagTextPtr()->GetWholePixelSize();
		textRegion = MkFloat2(static_cast<float>(iTextSize.x), static_cast<float>(iTextSize.y));
	}
	return node;
}

MkTitleBarControlNode* MkWindowFactory::_CreateMessageBox
(const MkHashStr& name, const MkStr& titleStr, const MkArray<MkHashStr>& titleTextName, const MkStr& descStr, const MkArray<MkHashStr>& descTextName,
 MkWindowBaseNode* callBackWindow, eMsgBoxType boxType, eMsgBoxButton btnType) const
{
	// icon type
	MkWindowThemeData::eIconType iconType = MkWindowThemeData::eIT_None;
	MkWindowThemeData::eComponentType bodyType = MkWindowThemeData::eCT_None;
	switch (boxType)
	{
	case eMBT_Default:
		iconType = MkWindowThemeData::eIT_Default;
		bodyType = MkWindowThemeData::eCT_DefaultBox;
		break;
	case eMBT_Notice:
		iconType = MkWindowThemeData::eIT_Notice;
		bodyType = MkWindowThemeData::eCT_DefaultBox;
		break;
	case eMBT_Information:
		iconType = MkWindowThemeData::eIT_Information;
		bodyType = MkWindowThemeData::eCT_DefaultBox;
		break;
	case eMBT_Warning:
		iconType = MkWindowThemeData::eIT_Warning;
		bodyType = MkWindowThemeData::eCT_NoticeBox;
		break;
	case eMBT_EditMode:
		iconType = MkWindowThemeData::eIT_EditMode;
		bodyType = MkWindowThemeData::eCT_DefaultBox;
		break;
	}

	if ((iconType == MkWindowThemeData::eIT_None) || (bodyType == MkWindowThemeData::eCT_None))
		return NULL;

	// create title
	MkTitleBarControlNode* titleNode = MkTitleBarControlNode::CreateChildNode(NULL, name);
	if (titleNode != NULL)
	{
		do
		{
			titleNode->SetTitleBar(m_ThemeName, m_FrameType, 10.f, false); // length는 의미 없음. 이후 body frame에서 재설정 할 것임
			titleNode->SetIcon(iconType);

			if (titleTextName.Empty())
			{
				titleNode->SetCaption(titleStr);
			}
			else
			{
				titleNode->SetCaption(titleTextName);
			}
			
			// create body frame
			MkBodyFrameControlNode* bodyFrame = MkBodyFrameControlNode::CreateChildNode(titleNode, BodyFrameName);
			if (bodyFrame == NULL)
				break;

			// body frame dispositioner
			MkWindowDispositioner windowDispositioner;
			windowDispositioner.AddNewLine(); // title에 해당하는 빈 영역
			windowDispositioner.AddDummyToCurrentLine(titleNode->CalculateWindowSize());

			// create text tag
			MkFloat2 textRegion;
			MkWindowTagNode* tagNode = descTextName.Empty() ?
				CreateTextTagNode(OneAndOnlyTagName, descStr, textRegion) : CreateTextTagNode(OneAndOnlyTagName, descTextName, textRegion);
			
			if (tagNode == NULL)
				break;

			bodyFrame->AttachChildNode(tagNode);

			windowDispositioner.AddNewLine(MkWindowDispositioner::eLHA_Center); // text tag는 중앙 정렬
			windowDispositioner.AddRectToCurrentLine(OneAndOnlyTagName, textRegion);

			// call back target path
			MkArray<MkHashStr> callbackTarget;
			if (callBackWindow != NULL)
			{
				callBackWindow->GetWindowPath(callbackTarget);
			}

			// create ok btn
			MkWindowBaseNode* okBtn = CreateOkButtonNode();
			if (okBtn == NULL)
				break;

			bodyFrame->AttachChildNode(okBtn);

			if (!callbackTarget.Empty())
			{
				okBtn->SetCallBackTargetWindowPath(callbackTarget);
			}

			windowDispositioner.AddNewLine(MkWindowDispositioner::eLHA_Center); // button set도 중앙 정렬
			windowDispositioner.AddRectToCurrentLine(okBtn->GetNodeName(), okBtn->CalculateWindowSize());

			// create cancel btn
			MkWindowBaseNode* cancelBtn = NULL;
			if (btnType == eMBB_OkCancel)
			{
				cancelBtn = CreateCancelButtonNode();
				if (cancelBtn == NULL)
					break;
				
				bodyFrame->AttachChildNode(cancelBtn);

				if (!callbackTarget.Empty())
				{
					cancelBtn->SetCallBackTargetWindowPath(callbackTarget);
				}

				windowDispositioner.AddRectToCurrentLine(cancelBtn->GetNodeName(), cancelBtn->CalculateWindowSize());
			}

			// dispositioner 계산
			windowDispositioner.Calculate();

			// body frame size
			MkFloat2 clientSize = windowDispositioner.GetRegion();
			bodyFrame->SetBodyFrame(m_ThemeName, bodyType, true, MkBodyFrameControlNode::eHT_IncludeParentAtTop, clientSize);

			// 하위 node 위치 반영
			windowDispositioner.ApplyPositionToNode(tagNode);
			windowDispositioner.ApplyPositionToNode(okBtn);
			windowDispositioner.ApplyPositionToNode(cancelBtn);

			return titleNode;
		}
		while (false);

		MK_DELETE(titleNode);
	}
	return NULL;
}

bool MkWindowFactory::_IsMsgBoxButton(const MkArray<MkHashStr>& callerPath, const MkHashStr& msgBoxName, const MkHashStr& buttonName)
{
	return ((callerPath.GetSize() == 3) && // title -> body -> button
		(callerPath[0] == msgBoxName) &&
		(callerPath[1] == BodyFrameName) &&
		(callerPath[2] == buttonName));
}

//------------------------------------------------------------------------------------------------//