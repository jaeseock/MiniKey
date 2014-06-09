#ifndef __MINIKEY_CORE_MKINTERFACEFORDATAWRITING_H__
#define __MINIKEY_CORE_MKINTERFACEFORDATAWRITING_H__


//------------------------------------------------------------------------------------------------//
// data type�� MkByteArray�� ���� ���� interface
// ȿ������ ���� �̸� �Է��� ������� ������ ���Ҵٰ� Flush() ȣ��� �Ѳ����� ���
// ��ϵ� �����ʹ� MkInterfaceForDataReading�� ����� ������
//
// �ټ��� �� ��Ͻ� �ӵ� ����� ���� SetInputSize() / UpdateInputSize()�� �ʿ� ������ �̸� Ȯ���� �� ����
// SetInputSize() ȣ���� ��� ����� �� Write() ���� �� UpdateInputSize() ȣ��
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
//	dwInterface.Write(MkStr(L"�Ʊ�ٸ� ���ٸ�"));
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

	// �ش� data type�� ���� �Է� �� �� ����
	bool SetInputSize(ePrimitiveDataType type, unsigned int size);

	// �Է� �� �� ���� ���Ḧ �����ϰ� �ʿ� ���� Ȯ��
	void UpdateInputSize(void);

	// Ÿ�Կ� ���� �� ����
	void Write(bool source);
	void Write(int source);
	void Write(unsigned int source);
	void Write(float source);
	void Write(const MkInt2& source);
	void Write(const MkVec2& source);
	void Write(const MkVec3& source);
	void Write(const MkStr& source);

	// flush�Ͽ� destBuffer�� beginPosition ��ġ���� ����(beginPosition�� MKDEF_MAX_ARRAY_SIZE�� ���� ������ ��ġ�� �ǹ�)
	// destBuffer�� �뷮�� ���ڶ�� Ȯ��
	// ���� �����Ͱ� �����ϸ� �������� �κ� ��ŭ ���(overwrite)
	// ��ȯ���� flush �� ������ ũ��
	unsigned int Flush(MkByteArray& destBuffer, unsigned int beginPosition = MKDEF_MAX_ARRAY_SIZE);

	// ����
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

	// �ܼ��ϰ� ����(dynamic casting ȸ��)
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
