#ifndef __MINIKEY_CORE_MKCHECK_H__
#define __MINIKEY_CORE_MKCHECK_H__

//------------------------------------------------------------------------------------------------//
// 조건 검증
//
// "MK_CHECK(condition, message) action" 형식으로 condition이 false일 시 해당 action을 취함
// 빌드 모드에 따라 다르게 동작
// - debug : 메세지 박스로 정지여부 질문. yes면 멈추고 no면 로그 남기고 action 실행
// - release : 메세지 박스로 정지여부 질문. yes면 강제 예외 발생, no면 로그 남기고 action 실행
// - shipping : action만 실행
//
// 매크로 부분이 복잡한 이유는 코드에서 "MK_CHECK(index != 0, L"index is " + MkStr(rlt)) {...}" 식으로
// message를 문자열 조합으로 사용하는 경우가 많은데 이 연산은 많은 비용이 드는 작업이므로 condition이
// false인 경우만 실행되도록 하기 위해서임
//
// (NOTE) assert()와는 달리 release, shipping 모드에서 무시되지 않음
//------------------------------------------------------------------------------------------------//
// ex>
//	MK_CHECK(ptr != NULL, L"pointer is NULL")
//	{
//		// if condition is false,
//		m_Available = false;
//	}
//
//	MK_CHECK(count == 0, L"count is not zero, " + MKStr(count))
//		return false;
//------------------------------------------------------------------------------------------------//


#ifdef _DEBUG

#define MK_CHECK(condition, message) if (!(condition)) if (MkCheck::ExceptionForDebug(message))

#else
#ifdef MK_SHIPPING

#define MK_CHECK(condition, message) if (!(condition))

#else

#define MK_CHECK(condition, message) if (!(condition)) if (MkCheck::ExceptionForRelease(__FUNCTION__, __LINE__, message))

#endif // MK_SHIPPING
#endif // _DEBUG

//------------------------------------------------------------------------------------------------//

class MkCheck
{
public:

	static bool ExceptionForDebug(const wchar_t* message);

	static bool ExceptionForRelease(const char* function, long lineNum, const wchar_t* message);
};

//------------------------------------------------------------------------------------------------//

#endif
