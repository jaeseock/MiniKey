
// ClientInstallerDlg.h : 헤더 파일
//

#pragma once

#include "MkCore_MkPatchFileDownloader.h"
#include "afxwin.h"
#include "afxcmn.h"


// CClientInstallerDlg 대화 상자
class CClientInstallerDlg : public CDialog
{
// 생성입니다.
public:
	CClientInstallerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CLIENTINSTALLER_DIALOG };

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

// 구현입니다.
protected:

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedCancel();

protected:

	HICON m_hIcon;

	CEdit m_InfoEdit;
	CProgressCtrl m_MainProgress;
	CProgressCtrl m_SubProgress;
	CButton m_CancelBtn;
	CButton m_DoneBtn;

	MkStr m_LastDesc;

	eMainState m_MainState;

	MkStr m_PatchURL;
	MkPatchFileDownloader m_Patcher;
	MkPatchFileDownloader::ePatchState m_PatchState;
};
