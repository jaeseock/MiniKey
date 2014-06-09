#ifndef __MINIKEY_CORE_MKTITLEBARHOOKER_H__
#define __MINIKEY_CORE_MKTITLEBARHOOKER_H__

//------------------------------------------------------------------------------------------------//
// MkBaseWindow�� ������ Ÿ��Ʋ�� Ŭ���� �߻��ϴ� �̺�Ʈ ��ŷ
// : ��Ƽ������� �ٸ� �����忡�� �����츦 �����ϴ� �Ͱ� ����� ��� ��ǥ
//
// - �̱۽������ ����/������������ Ÿ��Ʋ�� Ŭ���� ���μ��� �������� ȸ��
// - 3d �������� ��� ���������찡 ���� ������ ���� �巡�׽� �̵������� Ŭ����Ǵ� ���� ����
// - �ִ�ȭ ��ư�� ���� ��� Ÿ��Ʋ�� ����Ŭ���� ����
//
// (NOTE) thread-safe ���� ����
//------------------------------------------------------------------------------------------------//

#include <Windows.h>
#include "MkCore_MkType2.h"


class MkBaseWindow;

class MkTitleBarHooker
{
public:

	// ��� ������ ����
	// maxSizeIsWorkspace�� true�� ��� maximize�� Ŭ���̾�Ʈ ������ ����ȭ���� �ƴ� ��ũ�����̽� ũ��� ������
	void SetUp(MkBaseWindow* targetWindow, bool maxSizeIsWorkspace = false);

	// �ʱ�ȭ
	void Clear(void);

	// ��� �������� WndProc ���ο��� ȣ��
	// ��ȯ���� Ż�⿩��(return 0)
	bool CheckWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// �� ������ ȣ��
	void Update(void);

	MkTitleBarHooker() { Clear(); }
	~MkTitleBarHooker() { Clear(); }

protected:

	bool _ToggleShowMode(void);

protected:

	MkBaseWindow* m_TargetWindow;
	bool m_MaximizeEnable;
	bool m_MaxSizeIsWorkspace;

	bool m_Minimized;
	bool m_OnDrag;
	MkInt2 m_CursorStartPosition;
	MkInt2 m_WindowStartPosition;
};

//------------------------------------------------------------------------------------------------//

#endif
