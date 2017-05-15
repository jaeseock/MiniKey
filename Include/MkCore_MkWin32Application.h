#ifndef __MINIKEY_CORE_MKWIN32APPLICATION_H__
#define __MINIKEY_CORE_MKWIN32APPLICATION_H__

//------------------------------------------------------------------------------------------------//
// win32 application
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkGlobalDefinition.h"


class MkBaseFramework;
class MkCmdLine;

class MkWin32Application
{
public:

	// 프레임워크 생성 반환
	virtual MkBaseFramework* CreateFramework(void) const;

	// application 실행
	// 윈도우 생성 규칙
	// - fullScreen이 false일 경우
	//   * clientWidth, clientHeight가 0보다 클 경우 해당 크기만큼 윈도우 생성
	//   * clientWidth, clientHeight가 0일 경우 작업영역 크기만큼 윈도우 생성
	// - normal window application일 때 fullScreen이 true일 경우
	//   * 의미 없음. 상동
	// - rendering window application일 때 fullScreen이 true일 경우
	//   * clientWidth, clientHeight가 0보다 클 경우 장치에서 허락하는 크기면 윈도우 생성
	//   * clientWidth, clientHeight가 0일 경우 바탕화면 크기로 윈도우 생성
	//
	// argDefault : 기본 argument
	// argBonus : 기본 argument에 추가되는 argument
	//
	// argument로 어플간 중복 실행을 막을 수 있다
	// - #DMK = [name] : 어플 실행시 [name]이름으로 mutex를 선언
	// - #BME = [name] : 어플 실행시 [name]이름으로 선언된 mutex가 있을 경우 실행되지 않음
	// - #AME = [name] : #BME 키워드를 지우는 역할. 즉 "#DMK=A; #BME=A; #BME=B; #AME=A"는 "#DMK=A; #BME=B"와 동일
	// ex> A, B 서로 다른 두 어플이 있을 경우,
	//	- "#DMK=A; #BME=A" -> A 어플은 오직 하나만 실행 가능
	//	- "#DMK=A; #BME=A; #BME=B" -> A 어플은 실행 전 A, B 어플 중 하나라도 동작하고 있을 경우 실행 불가
	//	- "#DMK=A; #BME=A; #BME=B; #AME=A" -> "#DMK=A; #BME=B"이 되므로 A 어플은 실행 전 B 어플이 동작하고 있을 경우 실행 불가
	void Run(
		HINSTANCE hInstance,
		const wchar_t* title = L"MiniKey",
		const wchar_t* rootPath = L"..\\FileRoot",
		bool useLog = true,
		eSystemWindowProperty sysWinProp = eSWP_All,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int clientWidth = 640,
		int clientHeight = 480,
		bool fullScreen = false,
		bool dragAccept = false,
		WNDPROC wndProc = NULL,
		const char* arg1 = NULL,
		const char* arg2 = NULL
		);

	virtual ~MkWin32Application() {};

protected:

	bool _CheckExcution(MkCmdLine& cmdLine, HANDLE& myMutexHandle, const wchar_t* myTitle);
};

//------------------------------------------------------------------------------------------------//

#endif
