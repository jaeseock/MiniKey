#ifndef __MINIKEY_CORE_MKLOGMANAGER_H__
#define __MINIKEY_CORE_MKLOGMANAGER_H__


//------------------------------------------------------------------------------------------------//
// global instance - log manager
//
// 기본적으로 실행파일 이름으로 시스템 로그를 하나 보유
//
// 각 페이지를 개별적으로 외부 경로로 송출 할 수 있음
// (NOTE) 발송 클라이언트 계정이 로그를 남기려는 경로에 대한 쓰기권한이 있어야 함
//
// 크래쉬 발생시 크래쉬 정보 로그를 생성해 전송
// - CreateCrashPage()로 생성하고 MsgToCrashPage()로 기록한 후 최종적으로 SendCrashLogTo()로 전송 실행
//   ex> MK_LOG_MGR.SetCrashTargetAddress(L"\\\\192.168.1.10\\project\\Log");
//       ...
//       MK_LOG_MGR.SendCrashLogTo();
// - username과 생성시간이 로그 파일 이름에 추가되어 생성 일자를 폴더명으로 갖는 위치로 복사됨
//
// thread-safe
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkMap.h"
#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkLockable.h"

#define MK_LOG_MGR MkLogManager::Instance()


class MkLogManager : public MkSwitchedSingletonPattern<MkLogManager>
{
public:

	//------------------------------------------------------------------------------------------------//
	// management
	//------------------------------------------------------------------------------------------------//

	// 새 로그 페이지 생성 (producer)
	// (in) pageName : 생성될 로그 페이지 이름
	// (in) filePath : 로그파일이 생성될 절대, 혹은 root directory 기반 상대경로
	//   별도 설정되지 않았을 경우 root directory에 (pageName).txt로 생성됨
	// return : 생성 여부
	bool CreateNewPage(const MkStr& pageName, const MkPathName& filePath = L"");

	// 해당 페이지에 메세지 기록 (consumer)
	// (in) pageName : 대상 로그 페이지 이름
	// (in) message : 기록될 메세지
	// (in) addTime : 기록 시간 표시 여부
	void Msg(const MkStr& pageName, const MkStr& message, bool addTime = false);

	// 디폴트 시스템 로그 페이지에 메세지 기록 (consumer)
	// (in) message : 기록될 메세지
	// (in) addTime : 기록 시간 표시 여부
	void Msg(const MkStr& message, bool addTime = false);

	// 해당 페이지를 targetAddress로 전송 (consumer)
	// (in) pageName : 대상 로그 페이지 이름
	// (in) targetAddress : 전송될 경로
	void SendLogTo(const MkStr& pageName, const MkPathName& targetAddress);

	// 로그 파일 열기 (consumer)
	// (in) pageName : 대상 로그 페이지 이름
	void OpenLogFile(const MkStr& pageName);

	// 디폴트 시스템 로그 파일 열기 (consumer)
	void OpenSystemLogFile(void);

	//------------------------------------------------------------------------------------------------//
	// crashing operation
	//------------------------------------------------------------------------------------------------//

	// 크래쉬 발생시 정보를 전송할 주소 지정
	inline void SetCrashTargetAddress(const MkPathName& targetAddress) { m_CrashTargetAddress = targetAddress; }

	// 크래쉬 발생시 사전 지정된 로그 페이지를 생성하고 message 첨부 (producer)
	void CreateCrashPage(const MkStr& message);

	// 크래쉬 페이지에 메세지 기록 (consumer)
	void MsgToCrashPage(const MkStr& message);

	// 크래쉬 로그를 지정된 주소로 전송 (consumer)
	void SendCrashLogTo(void);

	//
	MkLogManager();
	MkLogManager(bool enable);
	virtual ~MkLogManager() {}

	//------------------------------------------------------------------------------------------------//

protected:

	MkPathName _GetTargetPage(const MkStr& pageName);

	MkStr _MakeCurrentTimeStr(void) const;

	//------------------------------------------------------------------------------------------------//

protected:

	// 크래쉬 페이지. 크래쉬 발생시 설정
	MkLockable<MkStr> m_CrashPageName;

	// 크래쉬 발생시 전송 주소
	MkLockable<MkPathName> m_CrashTargetAddress;

	// 파일경로 테이블
	MkLockable< MkMap<MkStr, MkPathName> > m_PageTable;

	// critical section
	MkCriticalSection m_CS_File;
};

#endif
