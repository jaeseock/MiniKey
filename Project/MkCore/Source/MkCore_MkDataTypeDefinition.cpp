
#include "MkCore_MkBitField32.h"
#include "MkCore_MkRect.h"
#include "MkCore_MkColor.h"
#include "MkCore_MkMat3.h"
#include "MkCore_MkQuaternion.h"
#include "MkCore_MkDataTypeDefinition.h"
#include "MkCore_MkPathName.h"


//------------------------------------------------------------------------------------------------//

// MkBitField32
const MkBitField32 MkBitField32::EMPTY;
const MkBitField32 MkBitField32::FULL(0xffffffff);

// MkType2
const MkShort2 MkShort2::Zero(0, 0);
const MkUShort2 MkUShort2::Zero(0, 0);
const MkInt2 MkInt2::Zero(0, 0);
const MkUInt2 MkUInt2::Zero(0, 0);
const MkLong2 MkLong2::Zero(0, 0);
const MkULong2 MkULong2::Zero(0, 0);
const MkInt642 MkInt642::Zero(0, 0);
const MkUInt642 MkUInt642::Zero(0, 0);
const MkFloat2 MkFloat2::Zero(0.f, 0.f);
const MkDouble2 MkDouble2::Zero(0., 0.);

// MkRect
const MkIntRect MkIntRect::EMPTY(MkInt2::Zero, MkInt2::Zero);
const MkInt64Rect MkInt64Rect::EMPTY(MkInt642::Zero, MkInt642::Zero);
const MkFloatRect MkFloatRect::EMPTY(MkFloat2::Zero, MkFloat2::Zero);
const MkDoubleRect MkDoubleRect::EMPTY(MkDouble2::Zero, MkDouble2::Zero);

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
const MkStr MkStr::TAG_CR(L"<CR>");
const MkStr MkStr::TAG_LF(L"<LF>");
const MkStr MkStr::TAG_CRLF(L"<CR><LF>");

// MkHashStr
const MkHashStr MkHashStr::EMPTY(MkStr::EMPTY);
const MkArray<MkHashStr> MkHashStr::EMPTY_ARRAY(0);

// MkPathName
const MkHashStr MkPathName::KeyFileCount(L"#FC");
const MkHashStr MkPathName::KeyDirCount(L"#DC");
const MkHashStr MkPathName::KeyFileSize(L"#SZ");
const MkHashStr MkPathName::KeyWrittenTime(L"#WT");
const MkHashStr MkPathName::KeyMD5(L"#MD5");

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
				else if (tag == m_TypeTag[ePDT_ByteArray]) return ePDT_ByteArray;
			}
			break;
		case L'i':
			{
				if (tag == m_TypeTag[ePDT_Int]) return ePDT_Int;
				else if (tag == m_TypeTag[ePDT_Int2]) return ePDT_Int2;
			}
			break;
		case L'd':
			{
				if (tag == m_TypeTag[ePDT_DInt]) return ePDT_DInt;
				else if (tag == m_TypeTag[ePDT_DUnsignedInt]) return ePDT_DUnsignedInt;
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
		L"dint",
		L"duint",
		L"float",
		L"int2",
		L"vec2",
		L"vec3",
		L"str",
		L"bar"
	};

//------------------------------------------------------------------------------------------------//
