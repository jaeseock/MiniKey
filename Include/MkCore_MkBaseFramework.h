#ifndef __MINIKEY_CORE_MKBASEFRAMEWORK_H__
#define __MINIKEY_CORE_MKBASEFRAMEWORK_H__

//------------------------------------------------------------------------------------------------//
// base framework
//
// �ʱ�ȭ, ����, ��������� ���� ����
// �⺻ �̱��� �� ��ü ����, ���� ��������� ������ ����(logic, file loading) ����
//
// ������Ʈ ���ݿ� ���� logic-render�� ���� �������� ��ȭ�ϴ� single-thread�� ������ thread���� ó���Ǵ�
// multi-thread�� ��� ������� �������� ���θ� ���� ����(����Ʈ�� single-thread)
// - CreateLogicThreadUnit()�� ��ȯ���� NULL�� ���� single-thread��, logic thread unit ��ü�� ��� multi-thread�� �ν�
// - �������� MK_TIME_MGR.Update()�� ���� �ð� ������ MK_PAGE_MGR.__Update()�� ���� ������ ����
// - single-thread�� ��� ��� �� ��������, multi-thread�� ��� logic thread unit�� �������� ó��
//
// ������ ����
// - single-thread�� ��� �������� ������ ���� ����
// - multi-thread�� ���(logic thread unit ����) logic thread unit�� ������ ������ ������ ��
//   �ٸ� thread���� logic thread�� �������� ���� �Ǿ��� �� ���� ����
//   * ���� thread�� Update()�� �������� ������ ���� ���� (ex> 0, 1, 2, 3, 4, 5, ...)
//   * Ÿ thread�� Update()�� ������ ��ŵ ���ɼ� ���� (ex> 0, 1, 3, 5, 6, 8, ...)
//
// ex> single/multi-thread ȯ�濡 ���� application �ʱ�ȭ ����
//
//	// TestPage ����(����)
//	class TestPage : public MkBasePage
//	{
//		...
//	};
//
//	// single-thread������ TestFramework ����
//	class TestFramework : public MkBaseFramework
//	{
//	public:
//		virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
//		{
//			MK_PAGE_MGR.SetUp(new TestPage(L"TestPage"));
//			MK_PAGE_MGR.ChangePageDirectly(L"TestPage");
//			return true;
//		}
//		virtual ~TestFramework() {}
//	};
//
//	// multi-thread������ TestThreadUnit ����
//	class TestThreadUnit : public MkLogicThreadUnit
//	{
//	public:
//		virtual bool SetUp(void)
//		{
//			MK_PAGE_MGR.SetUp(new TestPage(L"TestPage"));
//			MK_PAGE_MGR.ChangePageDirectly(L"TestPage");
//			return MkLogicThreadUnit::SetUp();
//		}
//
//		TestThreadUnit(const MkHashStr& threadName) : MkLogicThreadUnit(threadName) {}
//		virtual ~TestThreadUnit() {}
//	};
//
//	// multi-thread������ TestFramework ����
//	class TestFramework : public MkBaseFramework
//	{
//	public:
//		virtual MkBaseThreadUnit* CreateLogicThreadUnit(const MkHashStr& threadName) const { return new TestThreadUnit(threadName); }
//		virtual ~TestFramework() {}
//	};
//
//	// TestApplication ����(����)
//	class TestApplication : public MkWin32Application
//	{
//	public:
//		virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }
//
//		TestApplication() : MkWin32Application() {}
//		virtual ~TestApplication() {}
//	};
//
//	// ��Ʈ�� ����Ʈ������ TestApplication ����
//	int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
//	{
//		TestApplication application;
//		application.Run(hI, L"MK Test app", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, false, false, NULL, cmdline);
//		return 0;
//	}
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkInstanceDeallocator.h"
#include "MkCore_MkAccessibilityShortcutKeySetter.h"
#include "MkCore_MkFixedFPUSetter.h"
#include "MkCore_MkThreadManager.h"
#include "MkCore_MkListeningWindow.h"
#include "MkCore_MkPathName.h"


class MkBaseThreadUnit;
class MkCheatMessage;

class MkBaseFramework
{
public:

	// logic thread unit ������ ��ȯ
	// Ȯ�� thread unit�� ����ϰ� ���� ��� ���⼭ ��� ������ ��ȯ
	// NULL�� ��ȯ�ϸ� single threadó�� ���
	virtual MkBaseThreadUnit* CreateLogicThreadUnit(const MkHashStr& threadName) const;

	// loading thread unit ������ ��ȯ
	// Ȯ�� thread unit�� ����ϰ� ���� ��� ���⼭ ��� ������ ��ȯ
	virtual MkBaseThreadUnit* CreateLoadingThreadUnit(const MkHashStr& threadName) const;

	// cheat message ������ ��ȯ
	virtual MkCheatMessage* CreateCheatMessage(void) const { return NULL; }

	// Ȯ�� �ʱ�ȭ
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg) { return true; }

	// ��� �������� �ʱ�ȭ �Ϸ� �� ���� ���� �˶�
	virtual void StartLooping(void) {}

	// Ȯ�� ����
	virtual void Update(void) {}

	// �巡�� ���� ó�� Ȯ�� ����
	virtual void ConsumeDraggedFiles(MkArray<MkPathName>& draggedFilePathList) { draggedFilePathList.Clear(); }

	// WM_SETCURSOR message ó��
	virtual void ConsumeSetCursorMsg(void) {}

	// Ȯ�� ����
	virtual void Clear(void) {}

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	bool __Start
		(HINSTANCE hInstance, const wchar_t* title, const wchar_t* rootPath, bool useLog, eSystemWindowProperty sysWinProp,
		int x, int y, int clientWidth, int clientHeight, bool fullScreen, bool dragAccept, WNDPROC wndProc, const char* arg);

	void __Run(void);

	void __End(void);

	// ������ ���ν���
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	MkBaseFramework() {}
	virtual ~MkBaseFramework() {}

protected:

	void _TurnOnLowFragmentationHeap(void);

protected:

	MkAccessibilityShortcutKeySetter m_ASKSetter;
	MkFixedFPUSetter m_FPUSetter;

	MkInstanceDeallocator m_InstanceDeallocator;
	MkThreadManager m_ThreadManager;
	MkListeningWindow m_MainWindow;

	bool m_UseLogicThreadUnit;
};

//------------------------------------------------------------------------------------------------//

#endif
