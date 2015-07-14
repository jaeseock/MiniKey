#ifndef __MINIKEY_CORE_MKLISTENINGWINDOW_H__
#define __MINIKEY_CORE_MKLISTENINGWINDOW_H__

//------------------------------------------------------------------------------------------------//
// ����� ���� MkWindowUpdateListener���� ������ �����ϴ� ������
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkWindowUpdateListener.h"
#include "MkCore_MkBaseWindow.h"


class MkListeningWindow : public MkBaseWindow, public MkWindowUpdateListener
{
public:

	//------------------------------------------------------------------------------------------------//
	// listener ����
	//------------------------------------------------------------------------------------------------//
	
	// listener �߰��� ���̵� ��ȯ
	virtual unsigned int RegisterWindowUpdateListener(MkWindowUpdateListener* reciever);

	// id�� �ش��ϴ� listener ����
	virtual void RemoveWindowUpdateListener(unsigned int index);

	// ��� listener ����
	virtual void ClearAllWindowUpdateListener(void);

	//------------------------------------------------------------------------------------------------//
	// ����� ���� ����
	//------------------------------------------------------------------------------------------------//

	// ������ �������� �ʱ�ȭ
	// clientSize�� xy�� 0 �����̸� ��ũ�����̽�(����ȭ�鿡�� �۾�ǥ������ ���ܵ� ����) ũ��� ����
	virtual bool SetUpByWindowCreation
		(HINSTANCE hInstance, WNDPROC wndProc, HWND hParent, const MkStr& title, eSystemWindowProperty sysWinProp,
		const MkInt2& position, const MkInt2& clientSize, bool fullScreen = false);

	// �ܺο��� ������ ������� �ʱ�ȭ
	virtual bool SetUpByOuterWindow(HWND hWnd);

	// ������ ��ġ ����
	virtual bool SetPosition(const MkInt2& position);

	// ������ ũ�� �缳��
	virtual bool SetWindowSize(const MkInt2& size);

	// ��Ŀ�� ����
	virtual bool SetOnTop(void);

	// ���̱� ���� ����
	virtual bool SetShowCmd(unsigned int showCmd);

	// SW_SHOWMINIMIZED ���¿��� �����찡 �����Ǿ��� �� MkTitleBarHooker�κ��� ȣ��Ǵ� �ݹ� ���
	// �ش� ��� ȣ�� ���� �ٸ� ������ ��� �Ϸ�� ����(position, size ��)
	virtual void WindowRestored(void);

	//------------------------------------------------------------------------------------------------//

	// ����
	virtual void Clear(void);

public:

	MkListeningWindow() : MkBaseWindow(), MkWindowUpdateListener() {}
	virtual ~MkListeningWindow() { Clear(); }

	//------------------------------------------------------------------------------------------------//

protected:

	// ������ ���� ��� ���ù�
	MkArray<MkWindowUpdateListener*> m_Listener;
};

#endif
