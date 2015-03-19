
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowThemeData.h"


const MkHashStr MkWindowThemeData::DefaultThemeName(L"Default");


const MkHashStr MkWindowThemeData::ComponentTypeName[MkWindowThemeData::eCT_RegularMax] =
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

const MkWindowThemeFormData::eFormType MkWindowThemeData::ComponentFormType[MkWindowThemeData::eCT_RegularMax] =
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

//------------------------------------------------------------------------------------------------//

bool MkWindowThemeData::SetUp(const MkDataNode& node)
{
	bool notDefaultTheme = (node.GetNodeName() != DefaultThemeName);

	// setting : image path. duty
	MK_CHECK(node.GetDataEx(L"ImagePath", m_ImageFilePath, 0) && (!m_ImageFilePath.Empty()), node.GetNodeName().GetString() + L" theme node�� ImagePath ��ΰ� ����")
		return false;

	MkBaseTexture* texture = MK_BITMAP_POOL.GetBitmapTexture(m_ImageFilePath);
	MK_CHECK(texture != NULL, L"�߸��� �̹��� ��� : " + m_ImageFilePath.GetString())
		return false;

	// setting : frame size. default theme�� duty
	MkArray<float> frameSize;
	if (notDefaultTheme)
	{
		node.GetData(L"FrameSize", frameSize);
	}
	else
	{
		MK_CHECK(node.GetData(L"FrameSize", frameSize) && (frameSize.GetSize() == eFT_Max), node.GetNodeName().GetString() + L" theme node�� FrameSize ���� ����")
			return false;
	}
	if (frameSize.GetSize() == eFT_Max)
	{
		m_FrameSize[eFT_Small] = GetMax<float>(frameSize[eFT_Small], 0.f);
		m_FrameSize[eFT_Large] = GetMax<float>(frameSize[eFT_Large], 0.f);
	}

	// setting : caption text node. default theme�� duty
	MkArray<MkHashStr> captionTextNode;
	if (notDefaultTheme)
	{
		node.GetDataEx(L"CaptionTextNode", captionTextNode);
	}
	else
	{
		MK_CHECK(node.GetDataEx(L"CaptionTextNode", captionTextNode) && (captionTextNode.GetSize() == eFT_Max), node.GetNodeName().GetString() + L" theme node�� CaptionTextNode ���� ����")
			return false;
	}

	if (captionTextNode.GetSize() == eFT_Max)
	{
		MK_CHECK(MK_STATIC_RES.TextNodeExist(captionTextNode[eFT_Small]), node.GetNodeName().GetString() + L" theme node���� �������� �ʴ� caption text node ���� : " + captionTextNode[eFT_Small].GetString())
			return false;
		MK_CHECK(MK_STATIC_RES.TextNodeExist(captionTextNode[eFT_Large]), node.GetNodeName().GetString() + L" theme node���� �������� �ʴ� caption text node ���� : " + captionTextNode[eFT_Large].GetString())
			return false;

		m_CaptionTextNode[eFT_Small] = captionTextNode[eFT_Small];
		m_CaptionTextNode[eFT_Large] = captionTextNode[eFT_Large];
	}

	// regular component ���
	for (unsigned int i=0; i<eCT_RegularMax; ++i)
	{
		const MkHashStr& componentName = ComponentTypeName[i];
		bool hasComponent = node.ChildExist(componentName);

		// default theme�� ��� component�� �� ������ �־�� ��
		MK_CHECK(notDefaultTheme || hasComponent, node.GetNodeName().GetString() + L" theme node�� " + componentName.GetString() + L" component�� ����")
			return false;

		if (hasComponent)
		{
			eComponentType compType = static_cast<eComponentType>(i);
			MkWindowThemeFormData& fd = m_RegularComponents.Create(compType);
			MK_CHECK(fd.SetUp(&m_ImageFilePath, texture, *node.GetChildNode(componentName)), node.GetNodeName().GetString() + L" theme node�� " + componentName.GetString() + L" component ���� ����")
				return false;

			// setup�� �Ϸ�Ǿ����� form type ����
			MK_CHECK(fd.GetFormType() == ComponentFormType[i], node.GetNodeName().GetString() + L" theme node�� " + componentName.GetString() + L" component form type�� �߸��Ǿ���")
				return false;
		}
	}

	// custom form ���
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
			MK_CHECK(fd.SetUp(&m_ImageFilePath, texture, *customFormNode.GetChildNode(cfName)), node.GetNodeName().GetString() + L" theme node�� " + CustomFormName.GetString() + L" component, " + cfName.GetString() + L" ���� ����")
				return false;
		}
	}

	// ������ �������� ��� �� ������ texture ����
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

const MkHashStr& MkWindowThemeData::GetCaptionTextNode(eFrameType frameType) const
{
	switch (frameType)
	{
	case eFT_Small:
	case eFT_Large:
		return m_CaptionTextNode[frameType];
	}
	return MkHashStr::EMPTY;
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
