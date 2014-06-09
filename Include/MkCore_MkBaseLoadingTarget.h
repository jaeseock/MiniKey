#ifndef __MINIKEY_CORE_MKBASELOADINGTARGET_H__
#define __MINIKEY_CORE_MKBASELOADINGTARGET_H__

//------------------------------------------------------------------------------------------------//
// background loading을 위한 인터페이스
//
// + MkByteArray를 로딩하기 위한 MkByteArrayLoadingTarget 포함
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkArray.h"
#include "MkCore_MkLockable.h"
#include "MkCore_MkSharedPointer.h"


class MkStr;
class MkPathName;

class MkBaseLoadingTarget
{
public:

	// 데이터 상태
	enum eDataState
	{
		eWaiting = 0, // 로딩 대기중
		eLoading, // 로딩 진행 중

		eError, // 에러 발생. 종료
		eStop, // 명령에 의해 강제종료됨. 종료
		eComplete // 로딩 완료. 종료
	};

	// 외부 종료 명령
	inline void StopLoading(void) { m_Interrupt = true; }

	// 파일 시스템으로부터 읽어낸 byteArray로 객체 성격에 맞는 초기화를 실행하고 성공여부 반환
	// (NOTE) loading thread에 의해 호출 되기 때문에 thread 문제가 없음이 보장 되어야 함
	virtual bool CustomSetUp(const MkByteArray& byteArray, const MkPathName& filePath, const MkStr& argument) { return true; }

	// 데이터 상태 반환
	inline eDataState GetDataState(void) { return m_DataState; }

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	// 종료 명령 반환
	inline bool __GetInterrupt(void) { return m_Interrupt; }

	inline void __SetDataState(eDataState state) { m_DataState = state; }

	MkBaseLoadingTarget()
	{
		m_DataState = eWaiting;
		m_Interrupt = false;
	}

	virtual ~MkBaseLoadingTarget() {}

private:

	MkLockable<eDataState> m_DataState; // 현재 상태
	MkLockable<bool> m_Interrupt; // 종료 명령 signal
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
