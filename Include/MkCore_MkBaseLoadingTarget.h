#ifndef __MINIKEY_CORE_MKBASELOADINGTARGET_H__
#define __MINIKEY_CORE_MKBASELOADINGTARGET_H__

//------------------------------------------------------------------------------------------------//
// background loading�� ���� �������̽�
//
// + MkByteArray�� �ε��ϱ� ���� MkByteArrayLoadingTarget ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"
#include "MkCore_MkLockable.h"
#include "MkCore_MkSharedPointer.h"


class MkStr;
class MkPathName;

class MkBaseLoadingTarget
{
public:

	// ������ ����
	enum eDataState
	{
		eWaiting = 0, // �ε� �����
		eLoading, // �ε� ���� ��

		eError, // ���� �߻�. ����
		eStop, // ��ɿ� ���� ���������. ����
		eComplete // �ε� �Ϸ�. ����
	};

	// �ܺ� ���� ���
	inline void StopLoading(void) { m_Interrupt = true; }

	// ���� �ý������κ��� �о byteArray�� ��ü ���ݿ� �´� �ʱ�ȭ�� �����ϰ� �������� ��ȯ
	// (NOTE) loading thread�� ���� ȣ�� �Ǳ� ������ thread ������ ������ ���� �Ǿ�� ��
	virtual bool CustomSetUp(const MkByteArray& byteArray, const MkPathName& filePath, const MkStr& argument) { return true; }

	// ������ ���� ��ȯ
	inline eDataState GetDataState(void) { return m_DataState; }

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	// ���� ��� ��ȯ
	inline bool __GetInterrupt(void) { return m_Interrupt; }

	inline void __SetDataState(eDataState state) { m_DataState = state; }

	MkBaseLoadingTarget()
	{
		m_DataState = eWaiting;
		m_Interrupt = false;
	}

	virtual ~MkBaseLoadingTarget() {}

private:

	MkLockable<eDataState> m_DataState; // ���� ����
	MkLockable<bool> m_Interrupt; // ���� ��� signal
};

// smart ptr
typedef MkSharedPointer<MkBaseLoadingTarget> MkLoadingTargetPtr;


//------------------------------------------------------------------------------------------------//
// MkByteArrayLoadingTarget
//------------------------------------------------------------------------------------------------//

class MkByteArrayLoadingTarget : public MkBaseLoadingTarget
{
public:

	virtual bool CustomSetUp(const MkByteArray& byteArray, const MkPathName& filePath, const MkStr& argument)
	{
		if (!MkBaseLoadingTarget::CustomSetUp(byteArray, filePath, argument))
			return false;

		m_ByteArray = byteArray;
		return true;
	}

	inline const MkByteArray& GetByteArray(void) const { return m_ByteArray; }

	MkByteArrayLoadingTarget() : MkBaseLoadingTarget() {}
	virtual ~MkByteArrayLoadingTarget() { m_ByteArray.Clear(); }

private:

	MkByteArray m_ByteArray;
};

//------------------------------------------------------------------------------------------------//

#endif
