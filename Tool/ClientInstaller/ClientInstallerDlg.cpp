
// ClientInstallerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "ClientInstaller.h"
#include "ClientInstallerDlg.h"

#include "MkCore_MkPathName.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkFileDownloader.h"
#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkBaseFramework.h"

#include "MkAppManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MKDEF_CLIENT_FOLDER_NAME L"KuntaraOnline"
#define MKDEF_DOWNLOAD_URL L"http://210.207.252.151/fullpack/korea_pw"
#define MKDEF_SERVICE_FILE_NAME L"UpdateService.exe"


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
END_MESSAGE_MAP()


// CClientInstallerDlg �޽��� ó����

BOOL CClientInstallerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//---------------------------------------------------------------------------------------------//

	// ��ġ ��ġ
	MkPathName targetRootPath;

	while (true)
	{
		MkStr selDirMsg = L"������ ��ġ �� ���� ������ �˷��ּ���. ������ ���� �Ʒ�,\n[";
		selDirMsg += MKDEF_CLIENT_FOLDER_NAME;
		selDirMsg += L"] �̸����� ��ġ�˴ϴ�.";

		targetRootPath.Clear();
		if (!targetRootPath.GetDirectoryPathFromDialog(selDirMsg, m_hWnd))
		{
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return TRUE;
		}

		// �⺻ ���� �߰�
		targetRootPath += MKDEF_CLIENT_FOLDER_NAME;

		MkStr confirmMsg = L"[";
		confirmMsg += targetRootPath;
		confirmMsg += L"]\n��ġ�� ��ġ�ϰڽ��ϴ�.";

		int rlt = ::MessageBox(m_hWnd, confirmMsg.GetPtr(), MKDEF_CLIENT_FOLDER_NAME, MB_YESNO);
		if (rlt == IDNO)
			continue;

		if (rlt == IDYES)
		{
			if (!targetRootPath.MakeDirectoryPath())
			{
				::MessageBox(m_hWnd, L"��ġ �� �� ���� ��ġ�Դϴ�", MKDEF_CLIENT_FOLDER_NAME, MB_OK);
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return TRUE;
			}
			else
				break;
		}
	}

	// �־��� root directory ������� framework ����
	if (!APP_MGR.GetApplication().SetUpFramework(m_hWnd, targetRootPath.GetPtr(), false, false))
	{
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return TRUE;
	}

	//---------------------------------------------------------------------------------------------//

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_InfoEdit.SetWindowTextW(m_LastDesc.GetPtr());

	m_MainProgress.SetRange(0, 100);
	m_MainProgress.SetPos(0);

	m_SubProgress.SetRange(0, 100);
	m_SubProgress.SetPos(0);

	m_DoneBtn.EnableWindow(FALSE);
	m_DoneBtn.ShowWindow(SW_HIDE);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CClientInstallerDlg::OnPaint()
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

					if (currState == MkPatchFileDownloader::eShowSuccessResult)
					{
						MkPathName serviceFilePath = MkPathName::GetRootDirectory() + MKDEF_SERVICE_FILE_NAME;
						serviceFilePath.ExcuteWindowProcess(L"UpdateService");
					}

					m_MainState = eMS_Complete;
					break;
				}

				m_PatchState = currState;
			}

			MkStr descStr;
			switch (currState)
			{
			case MkPatchFileDownloader::eReady: break;
			case MkPatchFileDownloader::eDownloadPatchInfoFile: descStr = L"���� ���� �ٿ�"; break;
			case MkPatchFileDownloader::ePurgeDeleteList: descStr = L"���� ����"; break;
			case MkPatchFileDownloader::eFindDownloadTargets: descStr = L"�ٿ� ��� ���� ����"; break;
			case MkPatchFileDownloader::eRegisterTargetFiles:
			case MkPatchFileDownloader::eDownloadTargetFiles: descStr = L"���� �ٿ�ε�"; break;
			case MkPatchFileDownloader::eUpdateFiles: descStr = L"���� ����"; break;
			case MkPatchFileDownloader::eOptimizeChunk: descStr = L"�ý��� ����ȭ"; break;
			case MkPatchFileDownloader::eShowSuccessResult: descStr = L"�Ϸ�. ������ �޸� �غ� �Ǿ����ϴ�!"; break;
			case MkPatchFileDownloader::eShowFailedResult: descStr = L"���� ����. ��� �� ����� �غ�����."; break;
			}

			if (descStr != m_LastDesc)
			{
				m_InfoEdit.SetWindowTextW(descStr.GetPtr());
				m_LastDesc = descStr;
			}
		}
		break;

		case eMS_Complete: // ���
			break;

		case eMS_Quit:
			MkBaseFramework::Close();
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
