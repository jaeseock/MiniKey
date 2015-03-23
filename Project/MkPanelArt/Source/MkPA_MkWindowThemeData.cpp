
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowThemeData.h"


const MkHashStr MkWindowThemeData::DefaultThemeName(L"Default");


const MkHashStr MkWindowThemeData::ComponentTypeName[eCT_RegularMax] =
{
	MK_VALUE_TO_STRING(eCT_ShadowBox),
	MK_VALUE_TO_STRING(eCT_DefaultBox),
	MK_VALUE_TO_STRING(eCT_NoticeBox),
	MK_VALUE_TO_STRING(eCT_StaticBox),
	MK_VALUE_TO_STRING(eCT_GuideBox),
	MK_VALUE_TO_STRING(eCT_Title),
	MK_VALUE_TO_STRING(eCT_NormalBtn),
	MK_VALUE_TO_STRING(eCT_OKBtn),
	MK_VALUE_TO_STRING(eCT_CancelBtn),
	MK_VALUE_TO_STRING(eCT_FlatBtn),
	MK_VALUE_TO_STRING(eCT_GuideBtn),
	MK_VALUE_TO_STRING(eCT_GuideLine),
	MK_VALUE_TO_STRING(eCT_YellowZone),
	MK_VALUE_TO_STRING(eCT_BlueZone),
	MK_VALUE_TO_STRING(eCT_RedOutlineZone),
	MK_VALUE_TO_STRING(eCT_YellowSelBtn),
	MK_VALUE_TO_STRING(eCT_BlueSelBtn),
	MK_VALUE_TO_STRING(eCT_RedOutlineSelBtn),
	MK_VALUE_TO_STRING(eCT_RedLEDBtnSmall),
	MK_VALUE_TO_STRING(eCT_RedLEDBtnLarge),
	MK_VALUE_TO_STRING(eCT_GreenLEDBtnSmall),
	MK_VALUE_TO_STRING(eCT_GreenLEDBtnLarge),
	MK_VALUE_TO_STRING(eCT_BlueLEDBtnSmall),
	MK_VALUE_TO_STRING(eCT_BlueLEDBtnLarge),
	MK_VALUE_TO_STRING(eCT_NoticeIconSmall),
	MK_VALUE_TO_STRING(eCT_NoticeIconLarge),
	MK_VALUE_TO_STRING(eCT_InformationIconSmall),
	MK_VALUE_TO_STRING(eCT_InformationIconLarge),
	MK_VALUE_TO_STRING(eCT_WarningIconSmall),
	MK_VALUE_TO_STRING(eCT_WarningIconLarge),
	MK_VALUE_TO_STRING(eCT_WindowIconSmall),
	MK_VALUE_TO_STRING(eCT_WindowIconLarge),
	MK_VALUE_TO_STRING(eCT_EditModeIconSmall),
	MK_VALUE_TO_STRING(eCT_EditModeIconLarge),
	MK_VALUE_TO_STRING(eCT_LeftArrowIconSmall),
	MK_VALUE_TO_STRING(eCT_LeftArrowIconLarge),
	MK_VALUE_TO_STRING(eCT_RightArrowIconSmall),
	MK_VALUE_TO_STRING(eCT_RightArrowIconLarge),
	MK_VALUE_TO_STRING(eCT_UpArrowIconSmall),
	MK_VALUE_TO_STRING(eCT_UpArrowIconLarge),
	MK_VALUE_TO_STRING(eCT_DownArrowIconSmall),
	MK_VALUE_TO_STRING(eCT_DownArrowIconLarge),
	MK_VALUE_TO_STRING(eCT_CheckArrowIconSmall),
	MK_VALUE_TO_STRING(eCT_CheckArrowIconLarge)
};

const MkWindowThemeFormData::eFormType MkWindowThemeData::ComponentFormType[eCT_RegularMax] =
{
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_ShadowBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_DefaultBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_NoticeBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_StaticBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_GuideBox
	MkWindowThemeFormData::eFT_DualUnit, // eCT_Title
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_NormalBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_OKBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_CancelBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_FlatBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_GuideBtn
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_GuideLine
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_YellowZone
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_BlueZone
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_RedOutlineZone
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_YellowSelBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_BlueSelBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_RedOutlineSelBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_RedLEDBtnSmall
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_RedLEDBtnLarge
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_GreenLEDBtnSmall
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_GreenLEDBtnLarge
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_BlueLEDBtnSmall
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_BlueLEDBtnLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_NoticeIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_NoticeIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_InformationIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_InformationIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_WarningIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_WarningIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_WindowIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_WindowIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_EditModeIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_EditModeIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_LeftArrowIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_LeftArrowIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_RightArrowIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_RightArrowIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_UpArrowIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_UpArrowIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_DownArrowIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_DownArrowIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_CheckArrowIconSmall
	MkWindowThemeFormData::eFT_SingleUnit // eCT_CheckArrowIconLarge
};

const MkHashStr MkWindowThemeData::CustomFormName(MK_VALUE_TO_STRING(eCT_CustomForm));

const MkHashStr MkWindowThemeData::FrameTypeName[eFT_Max] =
{
	MK_VALUE_TO_STRING(eFT_Small),
	MK_VALUE_TO_STRING(eFT_Large)
};

//------------------------------------------------------------------------------------------------//

bool MkWindowThemeData::SetUp(const MkDataNode& node)
{
	bool notDefaultTheme = (node.GetNodeName() != DefaultThemeName);

	// setting : image path. duty
	MK_CHECK(node.GetDataEx(L"ImagePath", m_ImageFilePath, 0) && (!m_ImageFilePath.Empty()), node.GetNodeName().GetString() + L" theme node에 ImagePath 경로가 없음")
		return false;

	MkBaseTexture* texture = MK_BITMAP_POOL.GetBitmapTexture(m_ImageFilePath);
	MK_CHECK(texture != NULL, L"잘못된 이미지 경로 : " + m_ImageFilePath.GetString())
		return false;

	// setting:: frame. default theme만 duty
	for (unsigned int i=0; i<eFT_Max; ++i)
	{
		const MkHashStr& currTargetName = FrameTypeName[i];
		bool hasNode = node.ChildExist(currTargetName);

		// default theme는 모든 frame setting을 다 가지고 있어야 함
		MK_CHECK(notDefaultTheme || hasNode, node.GetNodeName().GetString() + L" theme node에 " + currTargetName.GetString() + L" setting이 없음")
			return false;

		if (hasNode)
		{
			eFrameType frameType = static_cast<eFrameType>(i);
			const MkDataNode& frameNode = *node.GetChildNode(currTargetName);

			// frame size
			float frameSize = 0.f;
			if (notDefaultTheme)
			{
				frameNode.GetData(L"FrameSize", frameSize, 0);
			}
			else
			{
				MK_CHECK(frameNode.GetData(L"FrameSize", frameSize, 0), node.GetNodeName().GetString() + L" theme node에 FrameSize 값이 없음")
					return false;
			}
			m_FrameSize[frameType] = GetMax<float>(frameSize, 0.f);

			// caption text node
			MkArray<MkHashStr> captionTextNode;
			if (notDefaultTheme)
			{
				frameNode.GetDataEx(L"CaptionTextNode", captionTextNode);
			}
			else
			{
				MK_CHECK(frameNode.GetDataEx(L"CaptionTextNode", captionTextNode) && (!captionTextNode.Empty()), node.GetNodeName().GetString() + L" theme node에 CaptionTextNode 값이 없음")
					return false;
			}
			
			MK_CHECK(MK_STATIC_RES.TextNodeExist(captionTextNode), node.GetNodeName().GetString() + L" theme node에서 존재하지 않는 caption text node 지정")
				return false;

			m_CaptionTextNode[frameType] = captionTextNode;
		}
	}
	
	// regular component 등록
	for (unsigned int i=0; i<eCT_RegularMax; ++i)
	{
		const MkHashStr& currTargetName = ComponentTypeName[i];
		bool hasNode = node.ChildExist(currTargetName);

		// default theme는 모든 component를 다 가지고 있어야 함
		MK_CHECK(notDefaultTheme || hasNode, node.GetNodeName().GetString() + L" theme node에 " + currTargetName.GetString() + L" component가 없음")
			return false;

		if (hasNode)
		{
			eComponentType compType = static_cast<eComponentType>(i);
			MkWindowThemeFormData& fd = m_RegularComponents.Create(compType);
			MK_CHECK(fd.SetUp(&m_ImageFilePath, texture, *node.GetChildNode(currTargetName)), node.GetNodeName().GetString() + L" theme node의 " + currTargetName.GetString() + L" component 구성 실패")
				return false;

			// setup이 완료되었으면 form type 점검
			MK_CHECK(fd.GetFormType() == ComponentFormType[i], node.GetNodeName().GetString() + L" theme node의 " + currTargetName.GetString() + L" component form type이 잘못되었음")
				return false;
		}
	}

	// custom form 등록
	if (node.ChildExist(CustomFormName))
	{
		const MkDataNode& customFormNode = *node.GetChildNode(CustomFormName);
		m_CustomForms = new MkHashMap<MkHashStr, MkWindowThemeFormData>;

		MkArray<MkHashStr> formKeyList;
		customFormNode.GetChildNodeList(formKeyList);
		MK_INDEXING_LOOP(formKeyList, i)
		{
			const MkHashStr& cfName = formKeyList[i];
			MkWindowThemeFormData& fd = m_CustomForms->Create(cfName);
			MK_CHECK(fd.SetUp(&m_ImageFilePath, texture, *customFormNode.GetChildNode(cfName)), node.GetNodeName().GetString() + L" theme node의 " + CustomFormName.GetString() + L" component, " + cfName.GetString() + L" 구성 실패")
				return false;
		}
	}

	// 설정이 끝났으니 사용 될 때까지 texture 해제
	MK_BITMAP_POOL.UnloadBitmapTexture(m_ImageFilePath);

	MkStr compMsg = (m_RegularComponents.GetSize() == eCT_RegularMax) ? L"FULL" : MkStr(m_RegularComponents.GetSize());
	MK_DEV_PANEL.MsgToLog(L"   - theme name : " + node.GetNodeName().GetString() + L" (" + compMsg + L" component)", false);
	return true;
}

void MkWindowThemeData::Clear(void)
{
	m_ImageFilePath.Clear();
	m_RegularComponents.Clear();
	MK_DELETE(m_CustomForms);
}

float MkWindowThemeData::GetFrameSize(eFrameType frameType) const
{
	switch (frameType)
	{
	case eFT_Small:
	case eFT_Large:
		return m_FrameSize[frameType];
	}
	return 0.f;
}

const MkArray<MkHashStr>& MkWindowThemeData::GetCaptionTextNode(eFrameType frameType) const
{
	switch (frameType)
	{
	case eFT_Small:
	case eFT_Large:
		return m_CaptionTextNode[frameType];
	}
	return MkHashStr::EMPTY_ARRAY;
}

const MkWindowThemeFormData* MkWindowThemeData::GetFormData(eComponentType compType, const MkHashStr& formName) const
{
	return ((compType == eCT_CustomForm) && (m_CustomForms != NULL)) ?
		(m_CustomForms->Exist(formName) ? &(*m_CustomForms)[formName] : NULL) :
		(m_RegularComponents.Exist(compType) ? &m_RegularComponents[compType] : NULL);
}

MkWindowThemeData::eComponentType MkWindowThemeData::GetLEDButtonComponent(eFrameType frameType, eLEDType LEDType)
{
	switch (frameType)
	{
	case eFT_Small:
		{
			switch (LEDType)
			{
			case eLED_Red: return eCT_RedLEDBtnSmall;
			case eLED_Green: return eCT_GreenLEDBtnSmall;
			case eLED_Blue: return eCT_BlueLEDBtnSmall;
			}
		}
		break;

	case eFT_Large:
		{
			switch (LEDType)
			{
			case eLED_Red: return eCT_RedLEDBtnLarge;
			case eLED_Green: return eCT_GreenLEDBtnLarge;
			case eLED_Blue: return eCT_BlueLEDBtnLarge;
			}
		}
		break;
	}
	return eCT_None;
}

MkWindowThemeData::eComponentType MkWindowThemeData::GetSystemIconComponent(eFrameType frameType, eIconType iconType)
{
	switch (frameType)
	{
	case eFT_Small:
		{
			switch (iconType)
			{
			case eIT_Default: return eCT_WindowIconSmall;
			case eIT_Notice: return eCT_NoticeIconSmall;
			case eIT_Information: return eCT_InformationIconSmall;
			case eIT_Warning: return eCT_WarningIconSmall;
			case eIT_EditMode: return eCT_EditModeIconSmall;
			case eIT_LeftArrow: return eCT_LeftArrowIconSmall;
			case eIT_RightArrow: return eCT_RightArrowIconSmall;
			case eIT_UpArrow: return eCT_UpArrowIconSmall;
			case eIT_DownArrow: return eCT_DownArrowIconSmall;
			case eIT_CheckArrow: return eCT_CheckArrowIconSmall;
			}
		}
		break;

	case eFT_Large:
		{
			switch (iconType)
			{
			case eIT_Default: return eCT_WindowIconLarge;
			case eIT_Notice: return eCT_NoticeIconLarge;
			case eIT_Information: return eCT_InformationIconLarge;
			case eIT_Warning: return eCT_WarningIconLarge;
			case eIT_EditMode: return eCT_EditModeIconLarge;
			case eIT_LeftArrow: return eCT_LeftArrowIconLarge;
			case eIT_RightArrow: return eCT_RightArrowIconLarge;
			case eIT_UpArrow: return eCT_UpArrowIconLarge;
			case eIT_DownArrow: return eCT_DownArrowIconLarge;
			case eIT_CheckArrow: return eCT_CheckArrowIconLarge;
			}
		}
		break;
	}
	return eCT_None;
}

MkWindowThemeData::MkWindowThemeData()
{
	m_CustomForms = NULL;
	m_FrameSize[eFT_Small] = 0.f;
	m_FrameSize[eFT_Large] = 0.f;
}

//------------------------------------------------------------------------------------------------//
