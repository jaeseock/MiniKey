#ifndef __MINIKEY_CORE_MKSYSTEMENVIRONMENT_H__
#define __MINIKEY_CORE_MKSYSTEMENVIRONMENT_H__


//------------------------------------------------------------------------------------------------//
// global instance - system environment
//
// application 구동시의 system, OS 등 관련 정보 저장
// 실행파일 버전 정보 정의
//
// 초기화 이후 변경 없고 getter만 존재하므로 thread-safe는 고려할 필요 없음
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkType2.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkVersionTag.h"

#define MK_SYS_ENV MkSystemEnvironment::Instance()


class MkSystemEnvironment : public MkSingletonPattern<MkSystemEnvironment>
{
public:

	//------------------------------------------------------------------------------------------------//

	// 빌드 모드
	enum eBuildMode
	{
		eDebug = 0,
		eRelease,
		eShipping
	};

	// cpu 연산방식
	enum eProcessorArchitecture
	{
		eEtcPA = 0, // itanium or unknown
		eX86,
		eX64
	};

	//------------------------------------------------------------------------------------------------//

	// 어플리케이션 버전 정보 반환
	inline const MkVersionTag& GetApplicationVersionTag(void) { return m_ApplicationVersion; }

	// 어플리케이션 빌드 모드 반환
	inline eBuildMode GetBuildMode(void) const { return m_BuildMode; }

	// cpu 연산방식 반환
	inline eProcessorArchitecture GetProcessorArchitecture(void) const { return m_ProcessorArchitecture; }

	// core 수 반환
	inline unsigned int GetNumberOfProcessors(void) const { return m_NumberOfProcessors; }

	// Windows 버전 정보 반환
	// http://msdn.microsoft.com/en-us/library/ms724834(v=vs.85).aspx
	inline const MkStr& GetWindowsVersion(void) const { return m_WindowsVersion; }

	// Windows가 XP 혹은 그보다 최신 버전인지 여부 반환
	inline bool CheckWindowsIsXpOrHigher(void) const { return m_WindowsIsXpOrHigher; }

	// Windows가 Vista 혹은 그보다 최신 버전인지 여부 반환
	inline bool CheckWindowsIsVistaOrHigher(void) const { return m_WindowsIsVistaOrHigher; }

	// 바탕화면 해상도 반환
	inline const MkInt2& GetBackgroundResolution(void) const { return m_BackgroundResolution; }

	// 현재 날자를 얻음 (년.월.일)
	static MkStr GetCurrentSystemDate(void);

	// 현재 시간을 얻음 (시:분:초)
	static MkStr GetCurrentSystemTime(void);

	// 현재 시스템 로그온된 유저 이름을 얻음
	inline const MkStr& GetCurrentLogOnUserName(void) const { return m_CurrentUserName; }

	// 시스템 정보 출력
	void __PrintSystemInformationToLog(void) const;

	//------------------------------------------------------------------------------------------------//

	MkSystemEnvironment();
	virtual ~MkSystemEnvironment() {}

protected:

	void _Initialize(void);

	//------------------------------------------------------------------------------------------------//

protected:

	eBuildMode m_BuildMode;

	eProcessorArchitecture m_ProcessorArchitecture;

	unsigned int m_NumberOfProcessors;

	MkStr m_WindowsVersion;
	bool m_WindowsIsXpOrHigher;
	bool m_WindowsIsVistaOrHigher;

	MkInt2 m_BackgroundResolution;

	MkStr m_CurrentUserName;

	MkVersionTag m_ApplicationVersion;
	MkVersionTag m_MiniKeyVersion;
};

//------------------------------------------------------------------------------------------------//

#endif
