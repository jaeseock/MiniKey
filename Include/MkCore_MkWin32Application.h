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

	// �����ӿ�ũ ���� ��ȯ
	virtual MkBaseFramework* CreateFramework(void) const;

	// application ����
	// ������ ���� ��Ģ
	// - fullScreen�� false�� ���
	//   * clientWidth, clientHeight�� 0���� Ŭ ��� �ش� ũ�⸸ŭ ������ ����
	//   * clientWidth, clientHeight�� 0�� ��� �۾����� ũ�⸸ŭ ������ ����
	// - normal window application�� �� fullScreen�� true�� ���
	//   * �ǹ� ����. ��
	// - rendering window application�� �� fullScreen�� true�� ���
	//   * clientWidth, clientHeight�� 0���� Ŭ ��� ��ġ���� ����ϴ� ũ��� ������ ����
	//   * clientWidth, clientHeight�� 0�� ��� ����ȭ�� ũ��� ������ ����
	//
	// argDefault : �⺻ argument
	// argBonus : �⺻ argument�� �߰��Ǵ� argument
	//
	// argument�� ���ð� �ߺ� ������ ���� �� �ִ�
	// - #DMK = [name] : ���� ����� [name]�̸����� mutex�� ����
	// - #BME = [name] : ���� ����� [name]�̸����� ����� mutex�� ���� ��� ������� ����
	// - #AME = [name] : #BME Ű���带 ����� ����. �� "#DMK=A; #BME=A; #BME=B; #AME=A"�� "#DMK=A; #BME=B"�� ����
	// ex> A, B ���� �ٸ� �� ������ ���� ���,
	//	- "#DMK=A; #BME=A" -> A ������ ���� �ϳ��� ���� ����
	//	- "#DMK=A; #BME=A; #BME=B" -> A ������ ���� �� A, B ���� �� �ϳ��� �����ϰ� ���� ��� ���� �Ұ�
	//	- "#DMK=A; #BME=A; #BME=B; #AME=A" -> "#DMK=A; #BME=B"�� �ǹǷ� A ������ ���� �� B ������ �����ϰ� ���� ��� ���� �Ұ�
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
