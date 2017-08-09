#ifndef __MINIKEY_CORE_MKBASEWINDOW_H__
#define __MINIKEY_CORE_MKBASEWINDOW_H__

//------------------------------------------------------------------------------------------------//
// win32 ������
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkGlobalDefinition.h"
#include "MkCore_MkStr.h"


class MkBaseWindow
{
public:

	// ������ �������� �ʱ�ȭ
	// clientSize�� xy�� 0 �����̸� ��ũ�����̽�(����ȭ�鿡�� �۾�ǥ������ ���ܵ� ����) ũ��� ����
	// ex>
	//	window.SetUpByWindowCreation(hI, WndProc, NULL, L"normal window", eSWP_All, MkInt2(CW_USEDEFAULT, CW_USEDEFAULT), MkInt2(640, 480));
	//	window.SetUpByWindowCreation(hI, WndProc, NULL, L"full window", eSWP_Close, MkInt2::Zero, MkInt2::Zero);
	virtual bool SetUpByWindowCreation
		(HINSTANCE hInstance, WNDPROC wndProc, HWND hParent, const MkStr& title, eSystemWindowProperty sysWinProp,
		const MkInt2& position, const MkInt2& clientSize, bool fullScreen = false, bool hide = false);

	// �ܺο��� ������ ������� �ʱ�ȭ
	virtual bool SetUpByOuterWindow(HWND hWnd);

	// ������ ��ġ ����
	virtual bool SetPosition(const MkInt2& position);

	// ������ ��ġ ��ȯ
	MkInt2 GetPosition(void) const;

	// ������ ũ�� �缳��
	virtual bool SetWindowSize(const MkInt2& size);

	// ������ ũ�� ��ȯ
	MkInt2 GetWindowSize(void) const;

	// Ŭ���̾�Ʈ ũ�⸦ ������ ������ ũ�� �缳��
	// resetStyle�� true�� ��� ��Ÿ�� �缳��
	virtual bool SetClientSize(const MkInt2& size);

	// Ŭ���̾�Ʈ ũ�� ��ȯ
	MkInt2 GetClientSize(void) const;

	// ��Ŀ�� ����
	virtual bool SetOnTop(void);

	// ��Ŀ������ ��ȯ
	bool GetOnTop(void) const;

	// ���̱� ���� ����
	virtual bool SetShowCmd(unsigned int showCmd);

	// ���̱� ���� ��ȯ
	unsigned int GetShowCmd(void) const;

	// SW_SHOWMINIMIZED ���¿��� �����찡 �����Ǿ��� �� MkTitleBarHooker�κ��� ȣ��Ǵ� �ݹ� ���
	// �ش� ��� ȣ�� ���� �ٸ� ������ ��� �Ϸ�� ����(position, size ��)
	virtual void WindowRestored(void) {}

	// ������ Ŭ���� ����
	virtual void Clear(void);

	// ������ Ÿ��Ʋ ����
	virtual void SetWindowTitle(const MkStr& title);

	// ���� �������� ������ ��Ÿ�� ��ȯ
	DWORD GetCurrentWindowStyle(void) const;

	// window mode���� ��Ÿ�� ��ȯ
	inline DWORD GetWindowModeStyle(void) const { return m_WindowModeStyle; }

	// window mode ��Ÿ�Ϸ� ��ȯ
	void ChangeToWindowModeStyle(void);

	// full mode ��Ÿ�Ϸ� ��ȯ
	void ChangeToFullModeStyle(void);

	// clientSize�� ������ ������ ũ�⸦ ��ȯ
	MkInt2 ConvertClientToWindowSize(const MkInt2& clientSize) const;

	// ������ ũ��κ��� clientSize�� ����� ��ȯ
	MkInt2 ConvertWindowToClientSize(const MkInt2& windowSize) const;

	// ������ �ڵ� ��ȯ
	inline HWND GetWindowHandle(void) const { return m_hWnd; }

	// �θ� ������ �ڵ� ��ȯ
	inline HWND GetParentWindowHandle(void) const { return m_hParent; }

	// ��ũ�����̽� ���� ũ�� ���
	static MkInt2 GetWorkspaceSize(void);

	// ������ ��ġ ���(vista �̻��� ��� aero ����)
	static MkInt2 GetWindowPosition(HWND hWnd);

	// ������ ũ�� ���(vista �̻��� ��� aero ����)
	static MkInt2 GetWindowSize(HWND hWnd);

	MkBaseWindow();
	virtual ~MkBaseWindow() { Clear(); }

protected:
	
	bool _CreateWindow(HINSTANCE hInstance, WNDPROC wndProc, HWND hParent, const MkStr& title, const MkInt2& position, const MkInt2& windowSize, bool fullScreen);

protected:

	HINSTANCE m_hInstance;
	HWND m_hWnd;
	HWND m_hParent;
	MkStr m_ClassName;
	DWORD m_WindowModeStyle;
	DWORD m_CurrentWindowStyle;
};

#endif
