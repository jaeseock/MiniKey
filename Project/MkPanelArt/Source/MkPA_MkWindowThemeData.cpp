
#include "MkCore_MkCheck.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkDevPanel.h"

#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkWindowThemeData.h"


const MkHashStr MkWindowThemeData::DefaultThemeName(L"Default");

const static MkHashStr KEY_IMAGE_INDEX = L"ImageIndex";


const MkHashStr MkWindowThemeData::ComponentTypeName[eCT_RegularMax] =
{
	MK_VALUE_TO_STRING(eCT_ShadowBox),
	MK_VALUE_TO_STRING(eCT_DefaultBox),
	MK_VALUE_TO_STRING(eCT_NoticeBox),
	MK_VALUE_TO_STRING(eCT_StaticBox),
	MK_VALUE_TO_STRING(eCT_GuideBox),
	MK_VALUE_TO_STRING(eCT_FlatBox),
	MK_VALUE_TO_STRING(eCT_Title),
	MK_VALUE_TO_STRING(eCT_NormalBtn),
	MK_VALUE_TO_STRING(eCT_OKBtn),
	MK_VALUE_TO_STRING(eCT_CancelBtn),
	MK_VALUE_TO_STRING(eCT_CheckBoxBtn),
	MK_VALUE_TO_STRING(eCT_FlatBtn),
	MK_VALUE_TO_STRING(eCT_GuideBtn),
	MK_VALUE_TO_STRING(eCT_GuideLine),
	MK_VALUE_TO_STRING(eCT_YellowZone),
	MK_VALUE_TO_STRING(eCT_BlueZone),
	MK_VALUE_TO_STRING(eCT_RedOutlineZone),
	MK_VALUE_TO_STRING(eCT_DarkZone),
	MK_VALUE_TO_STRING(eCT_YellowSelBtn),
	MK_VALUE_TO_STRING(eCT_BlueSelBtn),
	MK_VALUE_TO_STRING(eCT_RedOutlineSelBtn),
	MK_VALUE_TO_STRING(eCT_RedLEDBtnSmall),
	MK_VALUE_TO_STRING(eCT_RedLEDBtnMedium),
	MK_VALUE_TO_STRING(eCT_RedLEDBtnLarge),
	MK_VALUE_TO_STRING(eCT_GreenLEDBtnSmall),
	MK_VALUE_TO_STRING(eCT_GreenLEDBtnMedium),
	MK_VALUE_TO_STRING(eCT_GreenLEDBtnLarge),
	MK_VALUE_TO_STRING(eCT_BlueLEDBtnSmall),
	MK_VALUE_TO_STRING(eCT_BlueLEDBtnMedium),
	MK_VALUE_TO_STRING(eCT_BlueLEDBtnLarge),
	MK_VALUE_TO_STRING(eCT_NoticeIconSmall),
	MK_VALUE_TO_STRING(eCT_NoticeIconMedium),
	MK_VALUE_TO_STRING(eCT_NoticeIconLarge),
	MK_VALUE_TO_STRING(eCT_InformationIconSmall),
	MK_VALUE_TO_STRING(eCT_InformationIconMedium),
	MK_VALUE_TO_STRING(eCT_InformationIconLarge),
	MK_VALUE_TO_STRING(eCT_WarningIconSmall),
	MK_VALUE_TO_STRING(eCT_WarningIconMedium),
	MK_VALUE_TO_STRING(eCT_WarningIconLarge),
	MK_VALUE_TO_STRING(eCT_WindowIconSmall),
	MK_VALUE_TO_STRING(eCT_WindowIconMedium),
	MK_VALUE_TO_STRING(eCT_WindowIconLarge),
	MK_VALUE_TO_STRING(eCT_EditModeIconSmall),
	MK_VALUE_TO_STRING(eCT_EditModeIconMedium),
	MK_VALUE_TO_STRING(eCT_EditModeIconLarge),
	MK_VALUE_TO_STRING(eCT_LeftArrowIconSmall),
	MK_VALUE_TO_STRING(eCT_LeftArrowIconMedium),
	MK_VALUE_TO_STRING(eCT_LeftArrowIconLarge),
	MK_VALUE_TO_STRING(eCT_RightArrowIconSmall),
	MK_VALUE_TO_STRING(eCT_RightArrowIconMedium),
	MK_VALUE_TO_STRING(eCT_RightArrowIconLarge),
	MK_VALUE_TO_STRING(eCT_UpArrowIconSmall),
	MK_VALUE_TO_STRING(eCT_UpArrowIconMedium),
	MK_VALUE_TO_STRING(eCT_UpArrowIconLarge),
	MK_VALUE_TO_STRING(eCT_DownArrowIconSmall),
	MK_VALUE_TO_STRING(eCT_DownArrowIconMedium),
	MK_VALUE_TO_STRING(eCT_DownArrowIconLarge),
	MK_VALUE_TO_STRING(eCT_CheckMarkIconSmall),
	MK_VALUE_TO_STRING(eCT_CheckMarkIconMedium),
	MK_VALUE_TO_STRING(eCT_CheckMarkIconLarge)
};

const MkWindowThemeFormData::eFormType MkWindowThemeData::ComponentFormType[eCT_RegularMax] =
{
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_ShadowBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_DefaultBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_NoticeBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_StaticBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_GuideBox
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_FlatBox
	MkWindowThemeFormData::eFT_DualUnit, // eCT_Title
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_NormalBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_OKBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_CancelBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_CheckBoxBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_FlatBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_GuideBtn
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_GuideLine
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_YellowZone
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_BlueZone
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_RedOutlineZone
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_DarkZone
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_YellowSelBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_BlueSelBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_RedOutlineSelBtn
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_RedLEDBtnSmall
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_RedLEDBtnMedium
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_RedLEDBtnLarge
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_GreenLEDBtnSmall
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_GreenLEDBtnMedium
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_GreenLEDBtnLarge
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_BlueLEDBtnSmall
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_BlueLEDBtnMedium
	MkWindowThemeFormData::eFT_QuadUnit, // eCT_BlueLEDBtnLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_NoticeIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_NoticeIconMedium
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_NoticeIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_InformationIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_InformationIconMedium
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_InformationIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_WarningIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_WarningIconMedium
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_WarningIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_WindowIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_WindowIconMedium
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_WindowIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_EditModeIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_EditModeIconMedium
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_EditModeIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_LeftArrowIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_LeftArrowIconMedium
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_LeftArrowIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_RightArrowIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_RightArrowIconMedium
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_RightArrowIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_UpArrowIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_UpArrowIconMedium
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_UpArrowIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_DownArrowIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_DownArrowIconMedium
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_DownArrowIconLarge
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_CheckMarkIconSmall
	MkWindowThemeFormData::eFT_SingleUnit, // eCT_CheckMarkIconMedium
	MkWindowThemeFormData::eFT_SingleUnit // eCT_CheckMarkIconLarge
};

const MkHashStr MkWindowThemeData::CustomFormName(MK_VALUE_TO_STRING(eCT_CustomForm));

const MkHashStr MkWindowThemeData::FrameTypeName[eFT_Max] =
{
	MK_VALUE_TO_STRING(eFT_Small),
	MK_VALUE_TO_STRING(eFT_Medium),
	MK_VALUE_TO_STRING(eFT_Large)
};

//------------------------------------------------------------------------------------------------//

bool MkWindowThemeData::SetUp(const MkDataNode& node)
{
	bool notDefaultTheme = (node.GetNodeName() != DefaultThemeName);

	// setting : image path. duty
	if (!_LoadThemeImagePath(node))
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

			// edit text node
			MkArray<MkHashStr> editTextNode;
			if (notDefaultTheme)
			{
				frameNode.GetDataEx(L"EditTextNode", editTextNode);
			}
			else
			{
				MK_CHECK(frameNode.GetDataEx(L"EditTextNode", editTextNode) && (!editTextNode.Empty()), node.GetNodeName().GetString() + L" theme node에 EditTextNode 값이 없음")
					return false;
			}
			
			MK_CHECK(MK_STATIC_RES.TextNodeExist(editTextNode), node.GetNodeName().GetString() + L" theme node에서 존재하지 않는 edit text node 지정")
				return false;

			m_EditTextNode[frameType] = editTextNode;
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
			const MkDataNode& formDataNode = *node.GetChildNode(currTargetName);
			MK_CHECK(fd.SetUp(&m_ImageFilePath[_LoadFormImageIndex(formDataNode)], formDataNode), node.GetNodeName().GetString() + L" theme node의 " + currTargetName.GetString() + L" component 구성 실패")
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
		unsigned int parentImageIndex = _LoadFormImageIndex(customFormNode);
		m_CustomForms = new MkHashMap<MkHashStr, MkWindowThemeFormData>;

		MkArray<MkHashStr> formKeyList;
		customFormNode.GetChildNodeList(formKeyList);
		MK_INDEXING_LOOP(formKeyList, i)
		{
			const MkHashStr& cfName = formKeyList[i];
			MkWindowThemeFormData& fd = m_CustomForms->Create(cfName);
			const MkDataNode& formDataNode = *customFormNode.GetChildNode(cfName);
			MK_CHECK(fd.SetUp(&m_ImageFilePath[_LoadFormImageIndex(formDataNode, parentImageIndex)], formDataNode), node.GetNodeName().GetString() + L" theme node의 " + CustomFormName.GetString() + L" component, " + cfName.GetString() + L" 구성 실패")
				return false;
		}
	}

	// 설정이 끝났으니 사용 될 때까지 texture 해제
	MK_INDEXING_LOOP(m_ImageFilePath, i)
	{
		MK_BITMAP_POOL.UnloadBitmapTexture(m_ImageFilePath[i]);
	}

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
	case eFT_Medium:
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
	case eFT_Medium:
	case eFT_Large:
		return m_CaptionTextNode[frameType];
	}
	return MkHashStr::EMPTY_ARRAY;
}

const MkArray<MkHashStr>& MkWindowThemeData::GetEditTextNode(eFrameType frameType) const
{
	switch (frameType)
	{
	case eFT_Small:
	case eFT_Medium:
	case eFT_Large:
		return m_EditTextNode[frameType];
	}
	return MkHashStr::EMPTY_ARRAY;
}

const MkWindowThemeFormData* MkWindowThemeData::GetFormData(eComponentType compType, const MkHashStr& formName) const
{
	return ((compType == eCT_CustomForm) && (m_CustomForms != NULL)) ?
		(m_CustomForms->Exist(formName) ? &(*m_CustomForms)[formName] : NULL) :
		(m_RegularComponents.Exist(compType) ? &m_RegularComponents[compType] : NULL);
}

MkWindowThemeData::eComponentType MkWindowThemeData::ConvertComponentNameToType(const MkHashStr& componentName)
{
	static MkHashMap<MkHashStr, eComponentType> table;
	if (table.Empty())
	{
		for (int i=0; i<eCT_RegularMax; ++i)
		{
			table.Create(ComponentTypeName[i], static_cast<eComponentType>(i));
		}
		table.Rehash();
	}
	return table.Exist(componentName) ? table[componentName] : eCT_None;
}

MkWindowThemeData::eFrameType MkWindowThemeData::ConvertFrameNameToType(const MkHashStr& frameName)
{
	static MkHashMap<MkHashStr, eFrameType> table;
	if (table.Empty())
	{
		for (int i=0; i<eFT_Max; ++i)
		{
			table.Create(FrameTypeName[i], static_cast<eFrameType>(i));
		}
		table.Rehash();
	}
	return table.Exist(frameName) ? table[frameName] : eFT_None;
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

	case eFT_Medium:
		{
			switch (LEDType)
			{
			case eLED_Red: return eCT_RedLEDBtnMedium;
			case eLED_Green: return eCT_GreenLEDBtnMedium;
			case eLED_Blue: return eCT_BlueLEDBtnMedium;
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
			case eIT_CheckMark: return eCT_CheckMarkIconSmall;
			}
		}
		break;

	case eFT_Medium:
		{
			switch (iconType)
			{
			case eIT_Default: return eCT_WindowIconMedium;
			case eIT_Notice: return eCT_NoticeIconMedium;
			case eIT_Information: return eCT_InformationIconMedium;
			case eIT_Warning: return eCT_WarningIconMedium;
			case eIT_EditMode: return eCT_EditModeIconMedium;
			case eIT_LeftArrow: return eCT_LeftArrowIconMedium;
			case eIT_RightArrow: return eCT_RightArrowIconMedium;
			case eIT_UpArrow: return eCT_UpArrowIconMedium;
			case eIT_DownArrow: return eCT_DownArrowIconMedium;
			case eIT_CheckMark: return eCT_CheckMarkIconMedium;
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
			case eIT_CheckMark: return eCT_CheckMarkIconLarge;
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
	m_FrameSize[eFT_Medium] = 0.f;
	m_FrameSize[eFT_Large] = 0.f;
}

//------------------------------------------------------------------------------------------------//

bool MkWindowThemeData::_LoadThemeImagePath(const MkDataNode& node)
{
	MkArray<MkHashStr> paths;
	MK_CHECK(node.GetDataEx(L"ImagePath", paths) && (!paths.Empty()), node.GetNodeName().GetString() + L" theme node에 ImagePath 경로가 없음")
		return false;

	MK_INDEXING_LOOP(paths, i)
	{
		const MkHashStr& imagePath = paths[i];
		MkBaseTexture* texture = MK_BITMAP_POOL.GetBitmapTexture(imagePath);
		MK_CHECK(texture != NULL, L"잘못된 이미지 경로 : " + imagePath.GetString())
			return false;

		m_ImageFilePath.PushBack(imagePath);
	}
	return true;
}

unsigned int MkWindowThemeData::_LoadFormImageIndex(const MkDataNode& node, unsigned int parentImageIndex)
{
	unsigned int index = 0xffffffff;
	return (node.GetData(KEY_IMAGE_INDEX, index, 0) && m_ImageFilePath.IsValidIndex(index)) ? index : parentImageIndex;
}

//------------------------------------------------------------------------------------------------//
