
// SimpleWebInstallerDlg.h: 헤더 파일
//

#pragma once

#include "MkCore_MkPatchFileDownloader.h"


// CSimpleWebInstallerDlg 대화 상자
class CSimpleWebInstallerDlg : public CDialogEx
{
// 생성입니다.
public:
	CSimpleWebInstallerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLEWEBINSTALLER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	enum eMainState
	{
		eMS_FirstFrame = 0,
		eMS_StartPatch,
		eMS_UpdatePatch,
		eMS_Complete,

		eMS_Quit
	};

	CProgressCtrl m_MainProgress;
	CProgressCtrl m_SubProgress;

	eMainState m_MainState;

	MkStr m_PatchURL;
	MkPatchFileDownloader m_Patcher;
	MkPatchFileDownloader::ePatchState m_PatchState;


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	virtual BOOL DestroyWindow();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
