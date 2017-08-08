
#pragma once

#include "cwebbrowser2.h"
#include "CImgBtn.h"
#include "CImgProgress.h"

#include "MkCore_MkPatchFileDownloader.h"
#include "afxwin.h"


class CKuntaraLauncherDlg : public CDialog
{
public:
	CKuntaraLauncherDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	enum { IDD = IDD_KUNTARALAUNCHER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	enum eMainState
	{
		eMS_FirstFrame = 0,
		eMS_CheckInput,
		eMS_StartPatch,
		eMS_UpdatePatch,
		eMS_Complete,

		eMS_Quit
	};

protected:
	HICON m_hIcon;

	CWebBrowser2 m_WebBrowser;

	CImgProgress m_MainProgress;
	CImgProgress m_SubProgress;

	CImgBtn m_StartBtn;
	CImgBtn m_CloseBtn;

	CEdit m_DescEdit;
	MkStr m_LastDesc;

	eMainState m_MainState;

	MkStr m_PatchURL;
	MkPatchFileDownloader m_Patcher;
	MkPatchFileDownloader::ePatchState m_PatchState;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
public:
	virtual BOOL DestroyWindow();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnBnClickedQuit();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
