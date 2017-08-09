#ifndef __MINIKEY_CORE_MKDEVPANEL_H__
#define __MINIKEY_CORE_MKDEVPANEL_H__


//------------------------------------------------------------------------------------------------//
// global instance - development panel
//
// thread-safe
//
// �����ǵ� system index (0 ~ MKDEF_BOARD_LINE_COUNT)
#define MKDEF_PREDEFINED_SYSTEM_INDEX_FILEIO 0 // MkLoadingThreadUnit
#define MKDEF_PREDEFINED_SYSTEM_INDEX_DISPLAYMODE 1 // MkRenderingDevice
#define MKDEF_PREDEFINED_SYSTEM_INDEX_TEXPOOL 2 // MkTextureAllocator
// - 3 ~ MKDEF_BOARD_LINE_COUNT : reserved
//------------------------------------------------------------------------------------------------//


#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkLockable.h"
#include "MkCore_MkStr.h"
#include "MkCore_MkSnappingWindow.h"
#include "MkCore_MkMessageHistory.h"
#include "MkCore_MkEventQueuePattern.h"


#define MK_DEV_PANEL MkDevPanel::Instance()

class MkCheatMessage;

class MkDevPanel : public MkSwitchedSingletonPattern<MkDevPanel>
{
public:

	//------------------------------------------------------------------------------------------------//
	// usage
	//------------------------------------------------------------------------------------------------//

	// �ʱ�ȭ
	// rootWindow : ���ø����̼� ��Ʈ ������
	// sRect::eAlignmentPosition : rootWindow�� ���� snapping ��ġ
	// cheatInterface : ġƮ ó�� �������̽�. NULL�ϰ�� �⺻ �������̽�(MkCheatMessage) ���, MkDevPanel ����� ��ü������ ��ϵ� ��ü ����
	bool SetUp(MkListeningWindow& rootWindow, eRectAlignmentPosition alignmentPosition = eRAP_RMostTop, MkCheatMessage* cheatInterface = NULL, bool hide = false);

	// freeboard�� index ���ο� msg ���
	void MsgToFreeboard(unsigned int index, const MkStr& msg);

	// log box�� msg ���
	// addTime�� true�̸� ��� �ð� ����
	void MsgToLog(const MkStr& msg, bool addTime = false);

	// log box�� �� �� �ϳ��� ����
	inline void InsertEmptyLine(void) { MsgToLog(MkStr::EMPTY, false); }

	// log box ���
	// ������� text�� �ʱ�ȭ �ϴ� ������ �α� ���Ͽ��� ������ ����
	void ClearLogWindow(void);

	// visible
	void SetVisible(bool enable);

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	void __Update(void);

	void __SetAlignmentPosition(eRectAlignmentPosition alignmentPosition);
	void __ResetAlignmentPosition(void);

	void __QuitApplication(void);

	void __StepBackMsgHistory(void);
	void __StepForwardMsgHistory(void);

	void __MsgToSystemBoard(unsigned int index, const MkStr& msg);
	void __MsgToDrawingBoard(unsigned int index, const MkStr& msg);

	void __CheckWndProc(UINT msg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK __DevPanelWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK __InputSubProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	MkDevPanel();
	MkDevPanel(bool enable);
	virtual ~MkDevPanel();

	//------------------------------------------------------------------------------------------------//

protected:

	enum eBoardType
	{
		eFreeboard = 0,
		eSystem,
		eProfiler,
		eThreadProf,
		eDrawing,

		eTypeCount
	};

	enum eEventType
	{
		eVisible = 0, // arg0 : on(1)/off(0)
		eMsgToFreeboard, // arg0 : line index, arg1 : msg
		eMsgToSystem, // arg0 : line index, arg1 : msg
		eMsgToDrawing, // arg0 : line index, arg1 : msg
		eMsgToLog, // arg0 : 0/1(addtime false/true), arg1 : msg
		eClearLog,
		eChangeSnapPosition, // arg0 : eRectAlignmentPosition
		eResetSnapPosition,
		eQuit
	};

	typedef MkEventUnitPack2<eEventType, unsigned int, MkStr> _PanelEvent;

	//------------------------------------------------------------------------------------------------//

	void _SetCheatInterface(MkCheatMessage* cheatInterface);

	HWND _CreateControl
		(HWND hWnd, HINSTANCE hInstance, const wchar_t* type, unsigned int id, const wchar_t* caption, DWORD style, const MkIntRect& rect);

	void _SetVisible(bool enable);

	void _SetBoardMsg(eBoardType boardType, unsigned int index, const MkStr& msg);
	void _SetBoardType(eBoardType boardType);

	void _UpdateProfiling(void);

	void _PutMsgToLogBox(const MkStr& msg, bool addTime);
	void _ClearLogBox(void);

	void _ConsumeInputMsg(void);

	//------------------------------------------------------------------------------------------------//

protected:

	MkSnappingWindow m_Window;

	HFONT m_Font;

	// time state control
	HWND m_TimeStateHWND;

	// board constrol
	MkArray<HWND> m_BoardLineHWND;
	MkArray< MkLockable<MkStr> > m_BoardLineMsg[eTypeCount];
	MkLockable<eBoardType> m_CurrentBoardType;

	// log control
	HWND m_LogHWND;
	MkCriticalSection m_LogCS;
	MkStr m_LogMsg;
	int m_LogLineCount;
	
	// input control
	HWND m_InputHWND;
	MkCheatMessage* m_CheatMsg;
	MkMessageHistory m_MessageHistory;

	// event
	MkEventQueuePattern<_PanelEvent> m_EventQueue;
};

#endif
