
// SimpleWebInstallerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "SimpleWebInstaller.h"
#include "SimpleWebInstallerDlg.h"
#include "afxdialogex.h"

#include "MkCore_MkPathName.h"
#include "MkCore_MkFileDownloader.h"
#include "MkCore_MkPatchFileGenerator.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkDataNode.h"
#include "MkCore_MkTailData.h"

#include "MkAppManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSimpleWebInstallerDlg 대화 상자

CSimpleWebInstallerDlg::CSimpleWebInstallerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMPLEWEBINSTALLER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_MainState = eMS_FirstFrame;
	m_PatchState = MkPatchFileDownloader::eReady;
}

void CSimpleWebInstallerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_MainProgress);
	DDX_Control(pDX, IDC_PROGRESS2, m_SubProgress);
}

BEGIN_MESSAGE_MAP(CSimpleWebInstallerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CSimpleWebInstallerDlg 메시지 처리기

BOOL CSimpleWebInstallerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	MkStr appTitle = L"Web Installer";
	bool useKorean = true;
	MkStr clientFolderName;

	bool params = false;
	MkByteArray tailDataBuffer;
	if (MkTailData::GetTailData(tailDataBuffer))
	{
		MkDataNode tailDataNode;
		if (tailDataNode.Load(tailDataBuffer))
		{
			if (tailDataNode.GetData(L"AppTitle", appTitle, 0))
			{
				SetWindowText(appTitle.GetPtr());
			}

			tailDataNode.GetData(L"UseKorean", useKorean, 0);
			tailDataNode.GetData(L"ClientFolderName", clientFolderName, 0);
			tailDataNode.GetData(L"DownloadURL", m_PatchURL, 0);

			params = true;
		}
	}

	// for debugging
	appTitle = L"씰 유니버스(DEV) Web installer";
	useKorean = true;
	clientFolderName = L"Playwith\\SealUniverse_DEV";
	m_PatchURL = L"http://intpatch-sealm.playwith.co.kr/su_dev/full";

	if (clientFolderName.Empty() || m_PatchURL.Empty())
	{
		::MessageBox(m_hWnd, (useKorean) ? L"인스톨 정보가 없습니다." : L"No informations.", appTitle.GetPtr(), MB_OK);
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return TRUE;
	}	

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	//---------------------------------------------------------------------------------------------//

	MkPathName targetRootPath;

	while (true)
	{
		MkStr selDirMsg = (useKorean) ?
			(L"게임이 설치될 상위 폴더를 정해주세요.\n폴더 아래 [" + clientFolderName + L"] 위치에 구성됩니다.") : 
			(L"Please select folder for Tool Client. The folder [" + clientFolderName + L"] will be saved in the folder you select.");

		targetRootPath.Clear();
		if (!targetRootPath.GetDirectoryPathFromDialog(selDirMsg, m_hWnd))
		{
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return TRUE;
		}

		targetRootPath += clientFolderName;

		MkStr confirmMsg = (useKorean) ? L"다음 경로에 구성합니다.\n[" : L"Save in folder\n[";
		confirmMsg += targetRootPath;
		confirmMsg += L"]";

		int rlt = ::MessageBox(m_hWnd, confirmMsg.GetPtr(), appTitle.GetPtr(), MB_YESNO);
		if (rlt == IDNO)
			continue;

		if (rlt == IDYES)
		{
			if (!targetRootPath.MakeDirectoryPath())
			{
				::MessageBox(m_hWnd, (useKorean) ? L"잘못된 폴더 경로입니다." : L"This folder cannot be installed.", appTitle.GetPtr(), MB_OK);
				AfxGetMainWnd()->PostMessage(WM_CLOSE);
				return TRUE;
			}
			else
				break;
		}
	}

	if (!APP_MGR.GetApplication().SetUpFramework(m_hWnd, targetRootPath.GetPtr(), true, false))
	{
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return TRUE;
	}

	m_MainProgress.SetRange(0, 100);
	m_MainProgress.SetPos(0);

	m_SubProgress.SetRange(0, 100);
	m_SubProgress.SetPos(0);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CSimpleWebInstallerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CSimpleWebInstallerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CSimpleWebInstallerDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	switch (m_MainState)
	{
	case eMS_FirstFrame:
	{
		HWND frontWnd = ::GetForegroundWindow();
		if ((frontWnd != NULL) && (frontWnd != m_hWnd))
		{
			DWORD frontThreadID = ::GetWindowThreadProcessId(frontWnd, NULL);
			DWORD myThreadID = ::GetCurrentThreadId();
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

		m_MainState = eMS_StartPatch;
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

				m_MainState = eMS_Complete;

				if (currState == MkPatchFileDownloader::eShowSuccessResult)
				{
					MkPathName runPath = m_Patcher.GetRunFilePath();
					runPath.OpenFileInVerb();

					//if (MKDEF_AUTO_CLOSE)
					{
						OnCancel();
					}
				}
				break;
			}

			m_PatchState = currState;
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
	return TRUE;
}

LRESULT CSimpleWebInstallerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (MkWin32Application::MsgProc(m_hWnd, message, wParam, lParam))
		return 0;

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CSimpleWebInstallerDlg::DestroyWindow()
{
	APP_MGR.GetApplication().Close();
	BOOL ok = CDialog::DestroyWindow();
	APP_MGR.GetApplication().Destroy();

	return ok;
}

void CSimpleWebInstallerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (nID == SC_CLOSE)
	{
		switch (m_MainState)
		{
		case eMS_FirstFrame:
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
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
