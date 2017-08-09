#ifndef __MINIKEY_CORE_MKSNAPPINGWINDOW_H__
#define __MINIKEY_CORE_MKSNAPPINGWINDOW_H__

//------------------------------------------------------------------------------------------------//
// ���� �����츦 �������� ���ĵǴ� ������
//
// - �ʱ�ȭ(SetUpBy...)�� �θ� �����찡 �����ϰ� ������ġ�� ���õǾ� ������ �ڵ����� ���ĵ�
// - �θ� �����쿡�� �̵�, ũ�⺯��, �ּ�/�ִ�ȭ, ���� ���� �̺�Ʈ �߻� �� �����ĵ�
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkListeningWindow.h"


class MkSnappingWindow : public MkListeningWindow
{
public:

	//------------------------------------------------------------------------------------------------//
	// �ʱ�ȭ
	//------------------------------------------------------------------------------------------------//

	// ������ �������� �ʱ�ȭ
	// clientSize�� xy�� 0 �����̸� ��ũ�����̽�(����ȭ�鿡�� �۾�ǥ������ ���ܵ� ����) ũ��� ����
	virtual bool SetUpByWindowCreation
		(HINSTANCE hInstance, WNDPROC wndProc, HWND hParent, const MkStr& title, eSystemWindowProperty sysWinProp,
		const MkInt2& position, const MkInt2& clientSize, bool fullScreen = false, bool hide = false);

	// �ܺο��� ������ ������� �ʱ�ȭ
	virtual bool SetUpByOuterWindow(HWND hWnd);

	//------------------------------------------------------------------------------------------------//
	// ���ù�
	//------------------------------------------------------------------------------------------------//
	
	// ��ġ ����
	virtual void ListenPositionUpdate(const MkIntRect& newRect);

	// ũ�� ����
	virtual void ListenWindowSizeUpdate(const MkIntRect& newRect);

	// ��ġ, ũ�� ����
	virtual void ListenWindowRectUpdate(const MkIntRect& newRect);

	// ���̱� ���� ����
	virtual void ListenShowStateUpdate(unsigned int newShowCmd, const MkIntRect& newRect);

	// minimize ���� ����
	virtual void ListenWindowRestored(const MkIntRect& newRect);

	//------------------------------------------------------------------------------------------------//
	// snapping
	//------------------------------------------------------------------------------------------------//

	// ���� ��ġ ����
	void SetAlignmentPosition(eRectAlignmentPosition alignmentPosition);

	// ���� ��ġ ��ȯ
	inline eRectAlignmentPosition GetAlignmentPosition(void) { return m_AlignmentPosition; }

	// �߰� ��� �ȼ� �� ����
	inline void SetBorder(const MkInt2& size) { m_Border = size; }

	// �߰� ��� �ȼ� �� ��ȯ
	inline MkInt2 GetBorder(void) const { return m_Border; }

	// pivotRect�� �������� ����
	void SnapWindow(const MkIntRect& pivotRect);

public:

	MkSnappingWindow();
	virtual ~MkSnappingWindow() {}

protected:

	void _SnapToParent(void);

protected:

	eRectAlignmentPosition m_AlignmentPosition;
	MkInt2 m_Border;
};

#endif
