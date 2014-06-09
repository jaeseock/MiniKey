#ifndef __MINIKEY_CORE_MKWIN32APPLICATION_H__
#define __MINIKEY_CORE_MKWIN32APPLICATION_H__

//------------------------------------------------------------------------------------------------//
// win32 application
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkGlobalDefinition.h"


class MkBaseFramework;

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
		const char* arg = NULL
		);

	virtual ~MkWin32Application() {};
};

//------------------------------------------------------------------------------------------------//

#endif
