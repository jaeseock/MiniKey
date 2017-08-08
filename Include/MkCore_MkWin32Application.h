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

	//------------------------------------------------------------------------------------------------//
	// ������ ������ application ����
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
	// argAddition : �⺻ argument�� �߰��Ǵ� argument
	//
	// argument�� ���ð� �ߺ� ������ ���� �� �ִ�
	// - #DMK = [name] : ���� ����� [name]�̸����� mutex�� ����
	// - #BME = [name] : ���� ����� [name]�̸����� ����� mutex�� ���� ��� ������� ����
	// - #AME = [name] : #BME Ű���带 ����� ����. �� "#DMK=A; #BME=A; #BME=B; #AME=A"�� "#DMK=A; #BME=B"�� ����
	// ex> A, B ���� �ٸ� �� ������ ���� ���,
	//	- "#DMK=A; #BME=A" -> A ������ ���� �ϳ��� ���� ����
	//	- "#DMK=A; #BME=A; #BME=B" -> A ������ ���� �� A, B ���� �� �ϳ��� �����ϰ� ���� ��� ���� �Ұ�
	//	- "#DMK=A; #BME=A; #BME=B; #AME=A" -> "#DMK=A; #BME=B"�� �ǹǷ� A ������ ���� �� B ������ �����ϰ� ���� ��� ���� �Ұ�
	//------------------------------------------------------------------------------------------------//

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
		const wchar_t* argAddition = NULL
		);

	//------------------------------------------------------------------------------------------------//
	// �ܺ� ������� application ����
	// argument ������ ��
	//------------------------------------------------------------------------------------------------//

	bool Initialize(
		const wchar_t* title,
		const wchar_t* argAddition = NULL
		);

	bool SetUpFramework(
		HWND hWnd,
		const wchar_t* rootPath = L"..\\FileRoot",
		bool useLog = true,
		bool dragAccept = false
		);

	void Update(void); // �� �����Ӹ��� ȣ�� ���
	void Close(void); // destroy window ����
	void Destroy(void); // destroy window ����

	static bool MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//------------------------------------------------------------------------------------------------//

	static const MkCmdLine& GetCmdLine(void);

	MkWin32Application();
	virtual ~MkWin32Application();

protected:

	virtual bool _CheckExcution(MkCmdLine& cmdLine, HANDLE& myMutexHandle, const wchar_t* myTitle);

protected:

	HANDLE m_MutexHandle;
	MkBaseFramework* m_Framework;
};

//------------------------------------------------------------------------------------------------//

#endif
