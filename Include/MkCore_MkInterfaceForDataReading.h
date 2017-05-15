#ifndef __MINIKEY_CORE_MKINTERFACEFORDATAREADING_H__
#define __MINIKEY_CORE_MKINTERFACEFORDATAREADING_H__


//------------------------------------------------------------------------------------------------//
// MkByteArray���� MkInterfaceForDataWriting�� ��ϵ� �����͸� �����س��� ���� interface
// ��Ͻ��� ���� ����, ���� �ڷ������� Read()�� ȣ��
//
// ex>
//	// MkByteArray(srcArray) -> data
//	bool boolOut;
//	int intOut;
//	unsigned int uintOut;
//	__int64 dintOut;
//	unsigned __int64 duintOut;
//	float floatOut;
//	MkInt2 ptOut;
//	MkVec2 v2Out;
//	MkVec3 v3Out;
//	MkStr strOut;
//
//	MkInterfaceForDataReading drInterface;
//	drInterface.SetUp(srcArray, 0);
//
//	drInterface.Read(boolOut);
//	drInterface.Read(intOut);
//	drInterface.Read(uintOut);
//	drInterface.Read(dintOut);
//	drInterface.Read(duintOut);
//	drInterface.Read(floatOut);
//	drInterface.Read(ptOut);
//	drInterface.Read(v2Out);
//	drInterface.Read(v3Out);
//	drInterface.Read(strOut);
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"
#include "MkCore_MkArray.h"


class MkVec2;
class MkVec3;
class MkStr;

class MkInterfaceForDataReading
{
public:

	// �ʱ�ȭ
	// sourcePtr�� MkInterfaceForDataWriting�� ��ϵ� �����͸� ������ �ִ� ��ȿ�� byte array
	// ��ȯ���� ���� �ʱ�ȭ ����
	bool SetUp(const MkByteArray& srcArray, unsigned int beginPosition = 0);

	// Ÿ�Կ� ���� �� �б�
	// ��ȯ���� ���� ���� ����
	// (NOTE) �б� ������ false�� ��ȯ�Ǿ����� ���� �б�� ��������� ������ �� ����
	bool Read(bool& buffer);
	bool Read(int& buffer);
	bool Read(unsigned int& buffer);
	bool Read(__int64& buffer);
	bool Read(unsigned __int64& buffer);
	bool Read(float& buffer);
	bool Read(MkInt2& buffer);
	bool Read(MkVec2& buffer);
	bool Read(MkVec3& buffer);
	bool Read(MkStr& buffer);

	// ȣȯ���� ���� �ܺ� ���ۿ� �б�
	bool Read(unsigned char* buffer, unsigned int size);

	// ������ġ ���޿���
	bool CheckEndOfBlock(void) const;

	// ���� Ž�� ��ġ �̵�
	void SetCurrentPosition(unsigned int newPosition);

	// ���� Ž�� ��ġ ��ȯ
	unsigned int GetCurrentPosition(void) const;

	MkInterfaceForDataReading();

protected:

	MkByteArray* m_SourcePtr;
	unsigned int m_BeginPosition;
	unsigned int m_CurrentPosition;
	unsigned int m_EndPosition;
};

//------------------------------------------------------------------------------------------------//

#endif
