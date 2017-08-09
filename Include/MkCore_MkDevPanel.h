#ifndef __MINIKEY_CORE_MKDEVPANEL_H__
#define __MINIKEY_CORE_MKDEVPANEL_H__


//------------------------------------------------------------------------------------------------//
// global instance - development panel
//
// thread-safe
//
// 선정의된 system index (0 ~ MKDEF_BOARD_LINE_COUNT)
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

	// 초기화
	// rootWindow : 어플리케이션 루트 윈도우
	// sRect::eAlignmentPosition : rootWindow에 대한 snapping 위치
	// cheatInterface : 치트 처리 인터페이스. NULL일경우 기본 인터페이스(MkCheatMessage) 사용, MkDevPanel 종료시 자체적으로 등록된 객체 삭제
	bool SetUp(MkListeningWindow& rootWindow, eRectAlignmentPosition alignmentPosition = eRAP_RMostTop, MkCheatMessage* cheatInterface = NULL, bool hide = false);

	// freeboard의 index 라인에 msg 출력
	void MsgToFreeboard(unsigned int index, const MkStr& msg);

	// log box에 msg 출력
	// addTime이 true이면 출력 시간 기입
	void MsgToLog(const MkStr& msg, bool addTime = false);

	// log box에 빈 줄 하나를 삽입
	inline void InsertEmptyLine(void) { MsgToLog(MkStr::EMPTY, false); }

	// log box 비움
	// 윈도우상 text만 초기화 하는 것으로 로그 파일에는 영향이 없음
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
