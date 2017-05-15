#ifndef __MINIKEY_CORE_MKDATATYPEDEFINITION_H__
#define __MINIKEY_CORE_MKDATATYPEDEFINITION_H__


//------------------------------------------------------------------------------------------------//
// MiniKey primitive 자료형 선언
// data type을 다루는 모든 인터페이스류에서 지원 보장
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"
#include "MkCore_MkVec2.h"
#include "MkCore_MkVec3.h"
#include "MkCore_MkStr.h"


enum ePrimitiveDataType
{
	ePDT_Undefined = -1,
	ePDT_IndexBegin = 0,

	ePDT_Bool = ePDT_IndexBegin,
	ePDT_Int,
	ePDT_UnsignedInt,
	ePDT_DInt, // __int64
	ePDT_DUnsignedInt, // unsigned __int64
	ePDT_Float,
	ePDT_Int2,
	ePDT_Vec2,
	ePDT_Vec3,
	ePDT_Str,

	ePDT_IndexEnd,
	ePDT_MaxCount = ePDT_IndexEnd
};


// primitive data type 관련 상수 참조자
class MkPrimitiveDataType
{
public:

	// 종류 검사
	static bool IsValid(ePrimitiveDataType type) { return ((type >= ePDT_IndexBegin) && (type < ePDT_IndexEnd)); }

	// 타입별 enum 반환
	// ex>
	//	ePrimitiveDataType eb = MkPrimitiveDataType::GetEnum<bool>();
	//	ePrimitiveDataType ei = MkPrimitiveDataType::GetEnum<int>();
	template <class TE>	static ePrimitiveDataType GetEnum(void) { return ePDT_Undefined; }
	template <>	static ePrimitiveDataType GetEnum<bool>(void) { return ePDT_Bool; }
	template <>	static ePrimitiveDataType GetEnum<int>(void) { return ePDT_Int; }
	template <>	static ePrimitiveDataType GetEnum<unsigned int>(void) { return ePDT_UnsignedInt; }
	template <>	static ePrimitiveDataType GetEnum<__int64>(void) { return ePDT_DInt; }
	template <>	static ePrimitiveDataType GetEnum<unsigned __int64>(void) { return ePDT_DUnsignedInt; }
	template <>	static ePrimitiveDataType GetEnum<float>(void) { return ePDT_Float; }
	template <>	static ePrimitiveDataType GetEnum<MkInt2>(void) { return ePDT_Int2; }
	template <>	static ePrimitiveDataType GetEnum<MkVec2>(void) { return ePDT_Vec2; }
	template <>	static ePrimitiveDataType GetEnum<MkVec3>(void) { return ePDT_Vec3; }
	template <>	static ePrimitiveDataType GetEnum<MkStr>(void) { return ePDT_Str; }

	// tag -> enum
	// ex>
	//	ePrimitiveDataType eb = MkPrimitiveDataType::GetEnum(L"bool");
	//	ePrimitiveDataType ei = MkPrimitiveDataType::GetEnum(L"vec2");
	static ePrimitiveDataType GetEnum(const MkStr& tag);

	// 타입별 tag 반환
	// ex>
	//	const MkStr& tf = MkPrimitiveDataType::GetTag<float>();
	//	const MkStr& ts = MkPrimitiveDataType::GetTag<MkStr>();
	template <class TE>	static const MkStr& GetTag(void) { return m_UndefinedTag; }
	template <>	static const MkStr& GetTag<bool>(void) { return m_TypeTag[ePDT_Bool]; }
	template <>	static const MkStr& GetTag<int>(void) { return m_TypeTag[ePDT_Int]; }
	template <>	static const MkStr& GetTag<unsigned int>(void) { return m_TypeTag[ePDT_UnsignedInt]; }
	template <>	static const MkStr& GetTag<__int64>(void) { return m_TypeTag[ePDT_DInt]; }
	template <>	static const MkStr& GetTag<unsigned __int64>(void) { return m_TypeTag[ePDT_DUnsignedInt]; }
	template <>	static const MkStr& GetTag<float>(void) { return m_TypeTag[ePDT_Float]; }
	template <>	static const MkStr& GetTag<MkInt2>(void) { return m_TypeTag[ePDT_Int2]; }
	template <>	static const MkStr& GetTag<MkVec2>(void) { return m_TypeTag[ePDT_Vec2]; }
	template <>	static const MkStr& GetTag<MkVec3>(void) { return m_TypeTag[ePDT_Vec3]; }
	template <>	static const MkStr& GetTag<MkStr>(void) { return m_TypeTag[ePDT_Str]; }

	// enum -> tag
	// ex>
	//	const MkStr& tf = MkPrimitiveDataType::GetTag(ePDT_Float);
	//	const MkStr& ts = MkPrimitiveDataType::GetTag(ePDT_Str);
	static const MkStr& GetTag(ePrimitiveDataType type) { return IsValid(type) ? m_TypeTag[type] : m_UndefinedTag; }

protected:

	static const MkStr m_UndefinedTag;
	static const MkStr m_TypeTag[ePDT_MaxCount];
};

//------------------------------------------------------------------------------------------------//

#endif
