
// KuntaraLauncherDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "KuntaraLauncher.h"
#include "KuntaraLauncherDlg.h"

#include "MkCore_MkPathName.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkFileDownloader.h"
#include "MkCore_MkCmdLine.h"
#include "MkCore_MkLayeredWindow.h"
#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkPatchStarter.h"
#include "MkCore_MkBaseFramework.h"

#include "MkAppManager.h"
#include "GDIPlusManager.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static MkLayeredWindow g_LayeredWindow;


CKuntaraLauncherDlg::CKuntaraLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKuntaraLauncherDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_MainState = eMS_FirstFrame;
	m_PatchState = MkPatchFileDownloader::eReady;
}

void CKuntaraLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EXPLORER1, m_WebBrowser);
	DDX_Control(pDX, IDC_PROGRESS1, m_MainProgress);
	DDX_Control(pDX, IDC_PROGRESS2, m_SubProgress);
	DDX_Control(pDX, IDOK, m_StartBtn);
	DDX_Control(pDX, IDC_BUTTON1, m_CloseBtn);
	DDX_Control(pDX, IDC_EDIT1, m_DescEdit);
}

BEGIN_MESSAGE_MAP(CKuntaraLauncherDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
ON_WM_NCHITTEST()
ON_BN_CLICKED(IDC_BUTTON1, &CKuntaraLauncherDlg::OnBnClickedQuit)
ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


BOOL CKuntaraLauncherDlg::OnInitDialog()
{
	g_LayeredWindow.SetUp(L"KuntaraLauncherIntroImage", IDB_PNG12);

	CDialog::OnInitDialog();

	if (!APP_MGR.GetApplication().SetUpFramework(m_hWnd, L"", true, false))
	{
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return TRUE;
	}

	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	//if (GP_MGR.AddImage(0, L"launcher\\launcher_bg.png"))
	if (GP_MGR.AddImage(0, IDB_PNG3))
	{
		MkInt2 bgSize = GP_MGR.GetImageSize(0);
		MoveWindow(0, 0, bgSize.x, bgSize.y, 0);
	}

	// browser
	m_WebBrowser.MoveWindow(2, 52, 846, 394, 0);

	// border(�����¿� 2 px)�� �ý��������� �����Ϸ��� �� �̹��� 850*398���� �����¿� 2 px�� �� �ְ� �� ��ŭ rgn�� ����
	//m_WebBrowser.MoveWindow(-2, 50, 854, 342, 0);
	//HRGN hRgn = ::CreateRectRgn(2, 2, 852, 340);
	//m_WebBrowser.SetWindowRgn(hRgn, 0);

	CComVariant noticeURL(L"https://gameinfo.kuntara.co.kr/launcher/");
	m_WebBrowser.Navigate2(&noticeURL, NULL, NULL, NULL, NULL);

	// progress : main
	//m_MainProgress.SetProgressInfo(10, 119, 493, L"launcher\\gage_f.png", L"launcher\\gage_b.png");
	m_MainProgress.SetProgressInfo(10, 119, 514, IDB_PNG2, IDB_PNG1);
	m_MainProgress.SetRange(0, 100);
	m_MainProgress.SetPos(0);

	// progress : sub
	//m_SubProgress.SetProgressInfo(11, 119, 514, L"launcher\\gage_f.png", L"launcher\\gage_b.png");
	m_SubProgress.SetProgressInfo(11, 119, 493, IDB_PNG2, IDB_PNG1);
	m_SubProgress.SetRange(0, 100);
	m_SubProgress.SetPos(0);

	// btn : start
	//m_StartBtn.SetButtonInfo(20, 641, 461, L"launcher\\start_e.png", L"launcher\\start_f.png", L"launcher\\start_p.png", L"launcher\\start_d.png");
	m_StartBtn.SetButtonInfo(20, 641, 461, IDB_PNG9, IDB_PNG10, IDB_PNG11, IDB_PNG8);
	m_StartBtn.EnableWindow(FALSE);

	// btn : close
	//m_CloseBtn.SetButtonInfo(21, 641, 508, L"launcher\\quit_e.png", L"launcher\\quit_f.png", L"launcher\\quit_p.png", L"launcher\\quit_d.png");
	m_CloseBtn.SetButtonInfo(21, 641, 508, IDB_PNG5, IDB_PNG6, IDB_PNG7, IDB_PNG4);

	// desc
	//static CFont currFont;
	//currFont.CreatePointFont(10, L"����");

	m_DescEdit.SetWindowTextW(L"");
	m_DescEdit.MoveWindow(130, 475, 200, 16, 0);
	//m_DescEdit.SetFont(&currFont);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CKuntaraLauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CKuntaraLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CKuntaraLauncherDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	switch (m_MainState)
	{
	case eMS_FirstFrame:
		{
			g_LayeredWindow.Clear();
			MK_DEV_PANEL.__ResetAlignmentPosition();

			// ������ foreground�� ����
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
			const MkCmdLine& cmdLine = MkWin32Application::GetCmdLine();

			// patch url �˻�
			MkStr patchUrlKey = MKDEF_PATCH_DOWNLOAD_URL_KEY;
			if (cmdLine.HasPair(patchUrlKey))
			{
				m_PatchURL = cmdLine.GetValue(patchUrlKey, 0);
			}
			else
			{
				//m_PatchURL = L"http://210.207.252.151/kuntara/korea"; // temp
			}

			if (m_PatchURL.Empty())
			{
				::MessageBox(NULL, L"No URL.", L"KuntaraLauncher", MB_OK);
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
			m_Patcher.CheckAndDownloadPatchFile(m_PatchURL);
			m_MainState = eMS_UpdatePatch;
		}
		break;

	case eMS_UpdatePatch:
		{
			m_Patcher.Update();

			MkPatchFileDownloader::ePatchState currState = m_Patcher.GetCurrentState();
			if (currState == m_PatchState)
			{
				switch (currState)
				{
				case MkPatchFileDownloader::eRegisterTargetFiles:
				case MkPatchFileDownloader::eUpdateFiles:
				case MkPatchFileDownloader::eOptimizeChunk:
					m_SubProgress.SetPos(m_Patcher.GetCurrentProgress());
					break;
				}

				int offset = 0;
				float blockSize = 0.f;
				switch (currState)
				{
				case MkPatchFileDownloader::eRegisterTargetFiles:
					offset = 20;
					blockSize = 50.f;
					break;
				case MkPatchFileDownloader::eUpdateFiles:
					offset = 70;
					blockSize = 10.f;
					break;
				case MkPatchFileDownloader::eOptimizeChunk:
					offset = 80;
					blockSize = 20.f;
					break;
				}
				if (blockSize > 0)
				{
					float ratio = static_cast<float>(m_Patcher.GetCurrentProgress()) / static_cast<float>(m_Patcher.GetMaxProgress());
					int value = static_cast<int>(ratio * blockSize);
					m_MainProgress.SetPos(offset + value);
				}
			}
			else
			{
				switch (currState)
				{
				case MkPatchFileDownloader::ePurgeDeleteList: m_MainProgress.SetPos(5); break;
				case MkPatchFileDownloader::eFindDownloadTargets: m_MainProgress.SetPos(10); break;
				case MkPatchFileDownloader::eRegisterTargetFiles: m_MainProgress.SetPos(20); break;
				case MkPatchFileDownloader::eUpdateFiles: m_MainProgress.SetPos(70); break;
				case MkPatchFileDownloader::eOptimizeChunk: m_MainProgress.SetPos(80); break;

				case MkPatchFileDownloader::eShowSuccessResult:
					m_StartBtn.EnableWindow(TRUE);
					m_MainProgress.SetPos(100);
					m_MainState = eMS_Complete;
					break;

				case MkPatchFileDownloader::eShowFailedResult:
					m_MainProgress.SetPos(100);
					m_MainState = eMS_Complete;
					break;
				}

				if (currState != MkPatchFileDownloader::eDownloadTargetFiles)
				{
					if (m_Patcher.GetMaxProgress() == 0)
					{
						m_SubProgress.SetRange(0, 1);
						m_SubProgress.SetPos(1);
					}
					else
					{
						m_SubProgress.SetRange(0, m_Patcher.GetMaxProgress());
						m_SubProgress.SetPos(0);
					}

					m_PatchState = currState;
				}
			}

			MkStr descStr;
			switch (currState)
			{
			case MkPatchFileDownloader::eReady: break;

			case MkPatchFileDownloader::eDownloadPatchInfoFile:
				descStr = L"���� ���� �ٿ�";
				break;

			case MkPatchFileDownloader::ePurgeDeleteList:
				descStr = L"���� ����";
				break;

			case MkPatchFileDownloader::eFindDownloadTargets:
				descStr = L"�ٿ� ��� ���� ����";
				break;

			case MkPatchFileDownloader::eRegisterTargetFiles:
			case MkPatchFileDownloader::eDownloadTargetFiles:
				//descStr = L"��ġ ���� �ٿ�ε� (" + MkStr(m_Patcher.GetCurrentProgress()) + L" / " + MkStr(m_Patcher.GetMaxProgress()) + L")";
				descStr = L"��ġ ���� �ٿ�ε�";
				break;

			case MkPatchFileDownloader::eUpdateFiles:
				//descStr = L"��ġ ���� ���� (" + MkStr(m_Patcher.GetCurrentProgress()) + L" / " + MkStr(m_Patcher.GetMaxProgress()) + L")";
				descStr = L"��ġ ���� ����";
				break;

			case MkPatchFileDownloader::eOptimizeChunk:
				//descStr = L"���� �ý��� ����ȭ (" + MkStr(m_Patcher.GetCurrentProgress()) + L" / " + MkStr(m_Patcher.GetMaxProgress()) + L")";
				descStr = L"���� �ý��� ����ȭ";
				break;

			case MkPatchFileDownloader::eShowSuccessResult:
				descStr = L"�Ϸ�";
				break;

			case MkPatchFileDownloader::eShowFailedResult:
				descStr = L"���� ����. ��� �� �絵�� �غ�����.";
				break;
			}

			if (descStr != m_LastDesc)
			{
				RECT rc;
				m_DescEdit.GetWindowRect(&rc);
				ScreenToClient(&rc);
				GP_MGR.Draw(this, 0, MkInt2(rc.left, rc.top), MkInt2(rc.right - rc.left, rc.bottom - rc.top));

				m_DescEdit.SetWindowTextW(descStr.GetPtr());
				m_LastDesc = descStr;

				MK_DEV_PANEL.MsgToFreeboard(0, descStr);
			}
		}
		break;

	case eMS_Complete:
		// ���
		break;

	case eMS_Quit:
		MkBaseFramework::Close();
		break;
	}
	
	APP_MGR.GetApplication().Update();

	UpdateDialogControls(this, FALSE);
	return TRUE;
}

LRESULT CKuntaraLauncherDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (MkWin32Application::MsgProc(m_hWnd, message, wParam, lParam))
		return 0;

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CKuntaraLauncherDlg::DestroyWindow()
{
	APP_MGR.GetApplication().Close();

	BOOL ok = CDialog::DestroyWindow();

	APP_MGR.GetApplication().Destroy();

	return ok;
}

BOOL CKuntaraLauncherDlg::OnEraseBkgnd(CDC* pDC)
{
	GP_MGR.Draw(this, 0);
	return TRUE;
}

LRESULT CKuntaraLauncherDlg::OnNcHitTest(CPoint point)
{
	UINT hit = CDialog::OnNcHitTest(point);
	return (hit == HTCLIENT) ? HTCAPTION : hit;
}

void CKuntaraLauncherDlg::OnOK()
{
	CDialog::OnOK();

	// launcher�� app�� �����ϹǷ� ���� ���� ���� ó��
	const MkCmdLine& cmdLine = MkWin32Application::GetCmdLine();
	MkCmdLine newCmd = cmdLine;
	newCmd.AddPair(L"#AME", L"_MkLauncher");
	MkStr newArg;
	newCmd.GetFullStr(newArg);
	MkPathName runPath = m_Patcher.GetRunFilePath();
	runPath.OpenFileInVerb(newArg);
}

void CKuntaraLauncherDlg::OnBnClickedQuit()
{
	switch (m_MainState)
	{
	case eMS_FirstFrame:
	case eMS_CheckInput:
	case eMS_StartPatch:
	case eMS_Complete:
	case eMS_Quit:
		MkBaseFramework::Close();
		break;

	case eMS_UpdatePatch:
		MK_FILE_DOWNLOADER.StopDownload();
		break;
	}
}

HBRUSH CKuntaraLauncherDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_EDIT1)
	{
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(140, 140, 140));
		pDC->SelectStockObject(NULL_BRUSH);
		return NULL;
	}
	return hbr;
}
