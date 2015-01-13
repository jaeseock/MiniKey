
#include "MkCore_MkColor.h"
#include "MkCore_MkMat3.h"
#include "MkCore_MkQuaternion.h"
#include "MkCore_MkDataTypeDefinition.h"
#include "MkCore_MkHashStr.h"


//------------------------------------------------------------------------------------------------//

// MkVec2
const MkVec2 MkVec2::Zero(0.f, 0.f);
const MkVec2 MkVec2::AxisX(1.f, 0.f);
const MkVec2 MkVec2::AxisY(0.f, 1.f);
const MkVec2 MkVec2::One(1.f, 1.f);

// MkVec3
const MkVec3 MkVec3::Zero(0.f, 0.f, 0.f);
const MkVec3 MkVec3::AxisX(1.f, 0.f, 0.f);
const MkVec3 MkVec3::AxisY(0.f, 1.f, 0.f);
const MkVec3 MkVec3::AxisZ(0.f, 0.f, 1.f);
const MkVec3 MkVec3::One(1.f, 1.f, 1.f);

// MkMat3
const MkMat3 MkMat3::Identity;

// MkColor
const float MkColor::_DivFactor(1.f / 255.f);
const MkColor MkColor::Empty(0.f, 0.f, 0.f, 0.f);
const MkColor MkColor::Black(0.f, 0.f, 0.f, 1.f);
const MkColor MkColor::White(1.f, 1.f, 1.f, 1.f);
const MkColor MkColor::LightGray(0.67f, 0.67f, 0.67f, 1.f);
const MkColor MkColor::DarkGray(0.33f, 0.33f, 0.33f, 1.33f);
const MkColor MkColor::Red(1.f, 0.f, 0.f, 1.f);
const MkColor MkColor::Green(0.f, 1.f, 0.f, 1.f);
const MkColor MkColor::Blue(0.f, 0.f, 1.f, 1.f);
const MkColor MkColor::Yellow(1.f, 1.f, 0.f, 1.f);
const MkColor MkColor::Cian(0.f, 1.f, 1.f, 1.f);
const MkColor MkColor::Pink(1.f, 0.f, 1.f, 1.f);
const MkColor MkColor::Violet(1.f, 0.4f, 0.8f, 1.f);
const MkColor MkColor::Orange(1.f, 0.6f, 0.2f, 1.f);

// quaternion
const MkQuaternion MkQuaternion::Zero(0.f, 0.f, 0.f, 0.f);
const MkQuaternion MkQuaternion::Identity(1.f, 0.f, 0.f, 0.f);

// MkStr
const MkStr MkStr::EMPTY(L"");
const MkStr MkStr::TAB(L"\t");
const MkStr MkStr::LF(L"\n");
const MkStr MkStr::CR(L"\r");
const MkStr MkStr::SPACE(L" ");
const MkStr MkStr::CRLF(L"\r\n");

// MkHashStr
const MkHashStr MkHashStr::EMPTY(L"");

// MkPrimitiveDataType
ePrimitiveDataType MkPrimitiveDataType::GetEnum(const MkStr& tag)
{
	if (!tag.Empty())
	{
		switch (tag.GetAt(0))
		{
		case L'b':
			{
				if (tag == m_TypeTag[ePDT_Bool]) return ePDT_Bool;
			}
			break;
		case L'i':
			{
				if (tag == m_TypeTag[ePDT_Int]) return ePDT_Int;
				else if (tag == m_TypeTag[ePDT_Int2]) return ePDT_Int2;
			}
			break;
		case L'u':
			{
				if (tag == m_TypeTag[ePDT_UnsignedInt]) return ePDT_UnsignedInt;
			}
			break;
		case L'f':
			{
				if (tag == m_TypeTag[ePDT_Float]) return ePDT_Float;
			}
			break;
		case L'v':
			{
				if (tag == m_TypeTag[ePDT_Vec3]) return ePDT_Vec3;
				else if (tag == m_TypeTag[ePDT_Vec2]) return ePDT_Vec2;
			}
			break;
		case L's':
			{
				if (tag == m_TypeTag[ePDT_Str]) return ePDT_Str;
			}
			break;
		}
	}
	return ePDT_Undefined;
}

const MkStr MkPrimitiveDataType::m_UndefinedTag(L"undefined");
const MkStr MkPrimitiveDataType::m_TypeTag[ePDT_MaxCount] =
	{
		L"bool",
		L"int",
		L"uint",
		L"float",
		L"int2",
		L"vec2",
		L"vec3",
		L"str"
	};

//------------------------------------------------------------------------------------------------//
