
#include "stdafx.h"
#include "ClientInstaller.h"
#include "ClientInstallerDlg.h"

//#include "../../Lib/MkCore/Include/MkCore_MkPathName.h"
//#include "../../Lib/MkCore/Include/MkCore_MkDevPanel.h"
//#include "../../Lib/MkCore/Include/MkCore_MkFileDownloader.h"
//#include "../../Lib/MkCore/Include/MkCore_MkPatchFileGenerator.h"
//#include "../../Lib/MkCore/Include/MkCore_MkBaseFramework.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkFileDownloader.h"
#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkBaseFramework.h"

#include "MkAppManager.h"
#include "GDIPlusManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// test
//#define MKDEF_CLIENT_FOLDER_NAME L"KuntaraOnline"
//#define MKDEF_DOWNLOAD_URL L"http://210.207.252.151/fullpack/korea_pw"
//#define MKDEF_AUTO_CLOSE false

// cdn(pw)
//#define MKDEF_CLIENT_FOLDER_NAME L"KuntaraOnline"
//#define MKDEF_DOWNLOAD_URL L"http://full.kuntara.co.kr/pw"
//#define MKDEF_AUTO_CLOSE false

// cdn(og)
//#define MKDEF_CLIENT_FOLDER_NAME L"Ongate Kuntara Online"
//#define MKDEF_DOWNLOAD_URL L"http://full.kuntara.co.kr/og"
//#define MKDEF_AUTO_CLOSE false

// cdn(test)
#define MKDEF_CLIENT_FOLDER_NAME L"TeraInstall"
//#define MKDEF_DOWNLOAD_URL L"http://dltera.playwith.in.th/main"
//#define MKDEF_DOWNLOAD_URL L"http://aka-fulltera.playwith.in.th/main"
#define MKDEF_DOWNLOAD_URL L"http://dl.lytogame.com/newclient/tera"
#define MKDEF_AUTO_CLOSE true

//#define MKDEF_SERVICE_FILE_NAME L"UpdateService.exe"

static CFont sFont;


CClientInstallerDlg::CClientInstallerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientInstallerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_MainState = eMS_FirstFrame;
	m_PatchState = MkPatchFileDownloader::eReady;
}

void CClientInstallerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_InfoEdit);
	DDX_Control(pDX, IDC_PROGRESS1, m_MainProgress);
	DDX_Control(pDX, IDC_PROGRESS2, m_SubProgress);
	DDX_Control(pDX, IDCANCEL, m_CancelBtn);
	DDX_Control(pDX, IDOK, m_DoneBtn);
}

BEGIN_MESSAGE_MAP(CClientInstallerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CClientInstallerDlg::OnBnClickedCancel)
	ON_WM_ERASEBKGND()
	ON_WM_NCHITTEST()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


BOOL CClientInstallerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowTextW(L"TERA Installer");
	ModifyStyle(0, WS_SYSMENU, 0);

	//---------------------------------------------------------------------------------------------//

	MkPathName targetRootPath;

	while (true)
	{
		//MkStr selDirMsg = L"กรุณาเลือก Folder ที่จะดาวน์โหลดไฟล์ติดตั้ง ซึ่ง Folder [TeraInstall] จะถูกบันทึกลงในด้านล่างของ Folder ที่เลือก ";
		//MkStr selDirMsg = L"Please select folder for download Tera Client.\nThe folder [TeraInstall] will be saved in the folder you select.";
		MkStr selDirMsg = L"Silahkan pilih folder untuk donwload Tera Client. Folder [TeraInstall] akan tersimpan di dalam folder yang kamu pilih.";
		
		targetRootPath.Clear();
		if (!targetRootPath.GetDirectoryPathFromDialog(selDirMsg, m_hWnd))
		{
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return TRUE;
		}

		targetRootPath += MKDEF_CLIENT_FOLDER_NAME;

		//MkStr confirmMsg = L"บันทึกที่\n[";
		//MkStr confirmMsg = L"Save in folder [";
		MkStr confirmMsg = L"Simpan di dalam folder [";
		confirmMsg += targetRootPath;
		confirmMsg += L"]";

		int rlt = ::MessageBox(m_hWnd, confirmMsg.GetPtr(), MKDEF_CLIENT_FOLDER_NAME, MB_YESNO);
		if (rlt == IDNO)
			continue;

		if (rlt == IDYES)
		{
			if (!targetRootPath.MakeDirectoryPath())
			{
				//::MessageBox(m_hWnd, L"ไม่สามารถติดตั้งได้ที่ตำแหน่งนี้ ", MKDEF_CLIENT_FOLDER_NAME, MB_OK);
				//::MessageBox(m_hWnd, L"This folder cannot be installed.", MKDEF_CLIENT_FOLDER_NAME, MB_OK);
				::MessageBox(m_hWnd, L"Tidak bisa install di folder ini.", MKDEF_CLIENT_FOLDER_NAME, MB_OK);
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return TRUE;
			}
			else
				break;
		}
	}

	if (!APP_MGR.GetApplication().SetUpFramework(m_hWnd, targetRootPath.GetPtr(), false, false))
	{
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return TRUE;
	}

	//---------------------------------------------------------------------------------------------//
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	if (GP_MGR.AddImage(0, IDB_PNG9))
	{
		MkInt2 bgSize = GP_MGR.GetImageSize(0);
		MoveWindow(0, 0, bgSize.x, bgSize.y, 0);
	}

	m_MainProgress.SetProgressInfo(10, 58, 360, IDB_PNG7, IDB_PNG8);
	m_MainProgress.SetRange(0, 100);
	m_MainProgress.SetPos(0);

	m_SubProgress.SetProgressInfo(11, 58, 325, IDB_PNG7, IDB_PNG8);
	m_SubProgress.SetRange(0, 100);
	m_SubProgress.SetPos(0);

	m_CancelBtn.SetButtonInfo(20, 446, 388, IDB_PNG1, IDB_PNG2, IDB_PNG3, IDB_PNG1);

	m_DoneBtn.SetButtonInfo(21, 446, 388, IDB_PNG4, IDB_PNG5, IDB_PNG6, IDB_PNG4);
	m_DoneBtn.EnableWindow(FALSE);
	m_DoneBtn.ShowWindow(SW_HIDE);

	m_InfoEdit.SetWindowTextW(m_LastDesc.GetPtr());
	m_InfoEdit.MoveWindow(72, 395, 350, 16, 0);

	LOGFONT logFont;
	m_InfoEdit.GetFont()->GetLogFont(&logFont);
	logFont.lfHeight = 16;
	sFont.CreateFontIndirect(&logFont);
	m_InfoEdit.SetFont(&sFont);

	return TRUE;
}


void CClientInstallerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CClientInstallerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CClientInstallerDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	switch (m_MainState)
	{
	case eMS_FirstFrame:
		{
			MK_DEV_PANEL.__ResetAlignmentPosition();

			HWND frontWnd = ::GetForegroundWindow();
			if ((frontWnd != NULL) && (frontWnd != m_hWnd))
			{
				DWORD frontThreadID = ::GetWindowThreadProcessId(frontWnd, NULL);
				DWORD myThreadID= ::GetCurrentThreadId();
				if (myThreadID != frontThreadID)
				{
					if (::AttachThreadInput(myThreadID, frontThreadID, TRUE))
					{
						::SetForegroundWindow(m_hWnd);
						::BringWindowToTop(m_hWnd);
						::AttachThreadInput(myThreadID, frontThreadID, FALSE);
					}
				}
			}

			m_MainState = eMS_CheckInput;
		}
		break;

	case eMS_CheckInput:
		{
			m_PatchURL = MKDEF_DOWNLOAD_URL;

			if (m_PatchURL.Empty())
			{
				::MessageBox(NULL, L"No URL.", L"ClientInstaller", MB_OK);

				m_CancelBtn.EnableWindow(FALSE);
				m_CancelBtn.ShowWindow(SW_HIDE);

				m_DoneBtn.EnableWindow(TRUE);
				m_DoneBtn.ShowWindow(SW_SHOWNORMAL);

				m_MainState = eMS_Quit;
			}
			else
			{
				MK_DEV_PANEL.InsertEmptyLine();
				MK_DEV_PANEL.MsgToLog(L"> Download URL :");
				MK_DEV_PANEL.MsgToLog(L"  " + m_PatchURL);
				MK_DEV_PANEL.InsertEmptyLine();

				m_MainState = eMS_StartPatch;
			}
		}
		break;

	case eMS_StartPatch:
		{
			m_Patcher.CheckAndDownloadPatchFile(m_PatchURL, false);
			m_MainState = eMS_UpdatePatch;
		}
		break;

	case eMS_UpdatePatch:
		{
			m_Patcher.Update();

			MkPatchFileDownloader::ePatchState currState = m_Patcher.GetCurrentState();
			if (currState == m_PatchState)
			{
				// main progress
				switch (currState)
				{
				case MkPatchFileDownloader::eUpdateFiles:
					{
						float ratio = static_cast<float>(m_Patcher.GetCurrentProgress()) / static_cast<float>(m_Patcher.GetMaxProgress());
						m_MainProgress.SetPos(80 + static_cast<int>(ratio * 20.f));
					}
					break;
				}

				// sub progress
				switch (currState)
				{
				case MkPatchFileDownloader::eDownloadTargetFiles:
					m_SubProgress.SetPos(static_cast<int>(m_Patcher.GetDownloadProgress() * 100.f));
					break;
				case MkPatchFileDownloader::eUpdateFiles:
					m_SubProgress.SetPos(m_Patcher.GetCurrentProgress());
					break;
				}
			}
			else
			{
				switch (currState)
				{
				case MkPatchFileDownloader::ePurgeDeleteList:
					m_MainProgress.SetPos(3);
					m_SubProgress.SetRange(0, 1);
					m_SubProgress.SetPos(1);
					break;

				case MkPatchFileDownloader::eFindDownloadTargets:
					m_MainProgress.SetPos(6);
					break;

				case MkPatchFileDownloader::eRegisterTargetFiles:
					{
						float ratio = static_cast<float>(m_Patcher.GetCurrentProgress()) / static_cast<float>(m_Patcher.GetMaxProgress());
						m_MainProgress.SetPos(10 + static_cast<int>(ratio * 70.f));
					}
					m_SubProgress.SetRange(0, 100);
					m_SubProgress.SetPos(0);
					break;

				case MkPatchFileDownloader::eDownloadTargetFiles:
					m_SubProgress.SetPos(static_cast<int>(m_Patcher.GetDownloadProgress() * 100.f));
					break;

				case MkPatchFileDownloader::eUpdateFiles:
					m_SubProgress.SetRange(0, m_Patcher.GetMaxProgress());
					m_SubProgress.SetPos(0);
					break;

				case MkPatchFileDownloader::eOptimizeChunk:
					m_MainProgress.SetPos(100);
					m_SubProgress.SetRange(0, 1);
					m_SubProgress.SetPos(1);
					break;

				case MkPatchFileDownloader::eShowSuccessResult:
				case MkPatchFileDownloader::eShowFailedResult:
					m_MainProgress.SetPos(100);
					m_SubProgress.SetRange(0, 1);
					m_SubProgress.SetPos(1);
					
					m_CancelBtn.EnableWindow(FALSE);
					m_CancelBtn.ShowWindow(SW_HIDE);

					m_DoneBtn.EnableWindow(TRUE);
					m_DoneBtn.ShowWindow(SW_SHOWNORMAL);

					m_MainState = eMS_Complete;

					if (currState == MkPatchFileDownloader::eShowSuccessResult)
					{
						MkPathName runPath = m_Patcher.GetRunFilePath();
						runPath.OpenFileInVerb();

						if (MKDEF_AUTO_CLOSE)
						{
							OnCancel();
						}
					}
					break;
				}

				m_PatchState = currState;
			}

			MkStr descStr;
			switch (currState)
			{
			case MkPatchFileDownloader::eRegisterTargetFiles:
			case MkPatchFileDownloader::eDownloadTargetFiles:
				//descStr = L"กำลังดาวน์โหลดไฟล์ ";
				m_Patcher.GetCurrentDownloadState(descStr);
				break;
			case MkPatchFileDownloader::eUpdateFiles:
				//descStr = L"ปรับใช้ไฟล์แล้ว ";
				//descStr = L"Updating patch file.";
				descStr = L"Memperbarui patch file.";
				break;
			case MkPatchFileDownloader::eShowFailedResult:
				//descStr = L"หยุดดำเนินการชั่วคราว กรุณาลองดำเนินการใหม่ในอีกสักครู่ ";
				//descStr = L"Stop running. Please try again later.";
				descStr = L"Proses tidak berjalan. Silahkan coba lagi nanti.";
				break;
			}

			if (descStr != m_LastDesc)
			{
				//RECT rc;
				//m_InfoEdit.GetWindowRect(&rc);
				//ScreenToClient(&rc);
				//GP_MGR.Draw(this, 0, MkInt2(rc.left, rc.top), MkInt2(rc.right - rc.left, rc.bottom - rc.top));

				m_InfoEdit.SetWindowTextW(descStr.GetPtr());
				m_LastDesc = descStr;
			}
		}
		break;

		case eMS_Complete:
			break;

		case eMS_Quit:
			OnCancel();
			break;
	}
	
	APP_MGR.GetApplication().Update();

	UpdateDialogControls(this, FALSE);
	return TRUE;
}

LRESULT CClientInstallerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (MkWin32Application::MsgProc(m_hWnd, message, wParam, lParam))
		return 0;

	return CDialog::WindowProc(message, wParam, lParam);
}


BOOL CClientInstallerDlg::DestroyWindow()
{
	APP_MGR.GetApplication().Close();
	BOOL ok = CDialog::DestroyWindow();
	APP_MGR.GetApplication().Destroy();

	return ok;
}

void CClientInstallerDlg::OnBnClickedCancel()
{
	switch (m_MainState)
	{
	case eMS_FirstFrame:
	case eMS_CheckInput:
	case eMS_StartPatch:
	case eMS_Complete:
	case eMS_Quit:
		OnCancel();
		break;

	case eMS_UpdatePatch:
		MK_FILE_DOWNLOADER.StopDownload();
		break;
	}
}

BOOL CClientInstallerDlg::OnEraseBkgnd(CDC* pDC)
{
	GP_MGR.Draw(this, 0);
	return TRUE;
}

LRESULT CClientInstallerDlg::OnNcHitTest(CPoint point)
{
	UINT hit = CDialog::OnNcHitTest(point);
	return (hit == HTCLIENT) ? HTCAPTION : hit;
}

HBRUSH CClientInstallerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_EDIT1)
	{
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(140, 140, 140));
		pDC->SelectStockObject(BLACK_BRUSH);
		return NULL;
	}
	return hbr;
}
