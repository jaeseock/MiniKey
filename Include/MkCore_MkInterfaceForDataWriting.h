#ifndef __MINIKEY_CORE_MKINTERFACEFORDATAWRITING_H__
#define __MINIKEY_CORE_MKINTERFACEFORDATAWRITING_H__


//------------------------------------------------------------------------------------------------//
// data type을 MkByteArray에 쓰기 위한 interface
// 효율성을 위해 미리 입력을 순서대로 보관해 놓았다가 Flush() 호출시 한꺼번에 기록
// 기록된 데이터는 MkInterfaceForDataReading를 사용해 추출함
//
// 다수의 값 기록시 속도 향상을 위해 SetInputSize() / UpdateInputSize()로 필요 공간을 미리 확보할 수 있음
// SetInputSize() 호출이 모두 종료된 후 Write() 실행 전 UpdateInputSize() 호출
//------------------------------------------------------------------------------------------------//
// ex>
//	// data -> MkByteArray(destBuffer)
//	MkInterfaceForDataWriting dwInterface;
//	dwInterface.SetInputSize(ePDT_Bool, 1);
//	dwInterface.SetInputSize(ePDT_Int, 1);
//	dwInterface.SetInputSize(ePDT_UnsignedInt, 1);
//	dwInterface.SetInputSize(ePDT_Float, 1);
//	dwInterface.SetInputSize(ePDT_Int2, 1);
//	dwInterface.SetInputSize(ePDT_Vec2, 1);
//	dwInterface.SetInputSize(ePDT_Vec3, 1);
//	dwInterface.SetInputSize(ePDT_Str, 1);
//	dwInterface.UpdateInputSize();
//
//	dwInterface.Write(true);
//	dwInterface.Write(static_cast<int>(1234));
//	dwInterface.Write(static_cast<unsigned int>(5678));
//	dwInterface.Write(12.345f);
//	dwInterface.Write(MkInt2(-20, 30));
//	dwInterface.Write(MkVec2(12.f, 34.f));
//	dwInterface.Write(MkVec3(0.12f, 0.34f, 0.56f));
//	dwInterface.Write(MkStr(L"아기다리 고기다리"));
//
//	MkByteArray destBuffer;
//	unsigned int inputSize = dwInterface.Flush(destBuffer);
//	dwInterface.Clear();
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkDataTypeDefinition.h"
#include "MkCore_MkArray.h"


class MkInterfaceForDataWriting
{
public:

	// 해당 data type에 대한 입력 값 수 예약
	bool SetInputSize(ePrimitiveDataType type, unsigned int size);

	// 입력 값 수 예약 종료를 선언하고 필요 공간 확보
	void UpdateInputSize(void);

	// 타입에 따른 값 쓰기
	void Write(bool source);
	void Write(int source);
	void Write(unsigned int source);
	void Write(float source);
	void Write(const MkInt2& source);
	void Write(const MkVec2& source);
	void Write(const MkVec3& source);
	void Write(const MkStr& source);

	// flush하여 destBuffer의 beginPosition 위치부터 쓰기(beginPosition이 MKDEF_MAX_ARRAY_SIZE면 가장 마지막 위치를 의미)
	// destBuffer의 용량이 모자라면 확장
	// 기존 데이터가 존재하면 겹쳐지는 부분 만큼 덮어씀(overwrite)
	// 반환값은 flush 된 데이터 크기
	unsigned int Flush(MkByteArray& destBuffer, unsigned int beginPosition = MKDEF_MAX_ARRAY_SIZE);

	// 해제
	void Clear(void);

	MkInterfaceForDataWriting() {}
	~MkInterfaceForDataWriting() { Clear(); }

protected:

	typedef struct _InputUnit
	{
		ePrimitiveDataType type;
		unsigned int index;
	}
	InputUnit;

	void _RegisterInput(ePrimitiveDataType type, unsigned int index);
	void _RecordUnit(const unsigned char* srcPtr, unsigned int size) const;

protected:

	// 단순하게 저장(dynamic casting 회피)
	MkArray<bool> m_BoolUnits;
	MkArray<int> m_IntUnits;
	MkArray<unsigned int> m_UIntUnits;
	MkArray<float> m_FloatUnits;
	MkArray<MkInt2> m_Int2Units;
	MkArray<MkVec2> m_Vec2Units;
	MkArray<MkVec3> m_Vec3Units;
	MkArray<MkStr> m_StrUnits;

	MkArray<InputUnit> m_InputList;
};

//------------------------------------------------------------------------------------------------//

#endif
