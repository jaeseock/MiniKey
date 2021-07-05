
#include "stdafx.h"
#include "MMD Viewer.h"
#include "MMD ViewerDlg.h"

#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkClipboard.h"
#include "MkCore_MkDataNodeToMemoryConverter.h"
#include "MkCore_MkMemoryToDataTextConverter.h"
#include "MkCore_MkTagDefinitionForDataNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//------------------------------------------------------------------------------------------------//

CMMDViewerDlg::CMMDViewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMMDViewerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMMDViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON2, m_BT_ReloadCurrentFile);
	DDX_Control(pDX, IDC_RADIO1, m_RB_BinaryFile);
	DDX_Control(pDX, IDC_RADIO2, m_RB_TextFile);
	DDX_Control(pDX, IDC_RADIO3, m_RB_ExcelFile);
	DDX_Control(pDX, IDC_RADIO4, m_RB_JsonFile);
	DDX_Control(pDX, IDC_BUTTON3, m_BT_OverwriteCurrentFile);
	DDX_Control(pDX, IDC_BUTTON4, m_BT_SaveCurrentFileAs);
	DDX_Control(pDX, IDC_CHECK1, m_CB_AutoBackup);
	DDX_Control(pDX, IDC_BUTTON6, m_BT_OpenCurrentFolder);
	DDX_Control(pDX, IDC_EDIT1, m_EB_HierarchyView);
	DDX_Control(pDX, IDC_TREE1, m_TC_NodeView);
}

LRESULT CMMDViewerDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	m_MkApplication.Update();

	MkPathName filePath;
	if (m_TC_NodeView.GetDragFilePath(filePath))
	{
		_LoadTargetFile(filePath);
	}

	UpdateDialogControls(this, FALSE);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CMMDViewerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CMMDViewerDlg::OnBnClickedLoadFile)
	ON_BN_CLICKED(IDC_BUTTON2, &CMMDViewerDlg::OnBnClickedReloadCurrentFile)
	ON_BN_CLICKED(IDC_BUTTON3, &CMMDViewerDlg::OnBnClickedOverwriteCurrentFile)
	ON_BN_CLICKED(IDC_BUTTON4, &CMMDViewerDlg::OnBnClickedSaveCurrentFileAs)
	ON_BN_CLICKED(IDC_BUTTON6, &CMMDViewerDlg::OnBnClickedOpenCurrentFolder)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CMMDViewerDlg::OnNMRClick)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CMMDViewerDlg::OnTvnSelchangedTree)
END_MESSAGE_MAP()


// CMMDViewerDlg 메시지 처리기

BOOL CMMDViewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// minikey
	if ((!m_MkApplication.Initialize(L"MMD_Viewer", L"#HDP=")) ||
		(!m_MkApplication.SetUpFramework(m_hWnd, L"", true, false)))
	{
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
		return TRUE;
	}

	// 크기 재조정
	RECT rect;
	GetClientRect(&rect);
	_UpdateSizeInfo(SIZE_RESTORED, rect.right, rect.bottom);

	// 컨트롤 초기화
	m_CB_AutoBackup.SetCheck(TRUE);

	// 트리 & 아이콘
	m_TC_NodeView.SetBkColor(RGB(0, 0, 0));
	m_TC_NodeView.SetItemHeight(20);
	m_TreeImage.Create(16, 16, ILC_COLOR32, 0, 0);
	m_TC_NodeView.SetImageList(&m_TreeImage, LVSIL_NORMAL);

	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON1)); // 0. normal node
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON2)); // 1. template node
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON3)); // 2. data type tag
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON4)); // 3. unit - bool
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON5)); // 4. unit - int
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON6)); // 5. unit - uint
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON7)); // 6. unit - dint
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON8)); // 7. unit - duint
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON9)); // 8. unit - float
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON10)); // 9. unit - int2
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON11)); // 10. unit - vec2
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON12)); // 11. unit - vec3
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON13)); // 12. unit - str
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON14)); // 13. unit - bar
	m_TreeImage.Add(AfxGetApp()->LoadIcon(IDI_ICON15)); // 14. array element

	// 툴팁
	m_ToolTip.Create(this);
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON1), L"파일을 읽어들여 작업을 시작합니다.");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON2), L"동일 경로의 파일을 다시 읽어들입니다.");
	m_ToolTip.AddTool(GetDlgItem(IDC_RADIO1), L"파일 저장시 이진 포맷으로 저장합니다.");
	m_ToolTip.AddTool(GetDlgItem(IDC_RADIO2), L"파일 저장시 텍스트 포맷으로 저장합니다.");
	m_ToolTip.AddTool(GetDlgItem(IDC_RADIO2), L"파일 저장시 엑셀 포맷으로 저장합니다.");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON3), L"기존 파일 경로에 덮어씁니다.");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON4), L"새 경로에 파일을 저장합니다.");
	m_ToolTip.AddTool(GetDlgItem(IDC_CHECK1), L"같은 이름의 파일이 존재하면 자동으로 백업 파일을 만듭니다.");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON6), L"현재 작업중인 폴더를 엽니다.");
	m_ToolTip.SetMaxTipWidth(300);
	m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 10000); // 10 secs

	// 초기화
	_ClearCurrentNode();
	_UpdateAppTitle();
	_UpdateControlAvailable();

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMMDViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

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
		CDialog::OnPaint();
	}
}

HCURSOR CMMDViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMMDViewerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (MkWin32Application::MsgProc(m_hWnd, message, wParam, lParam))
		return 0;

	return CDialog::WindowProc(message, wParam, lParam);
}

void CMMDViewerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	_UpdateSizeInfo(nType, cx, cy);
}

BOOL CMMDViewerDlg::DestroyWindow()
{
	m_MkApplication.Close();
	BOOL ok = CDialog::DestroyWindow();
	m_MkApplication.Destroy();
	return ok;
}

BOOL CMMDViewerDlg::PreTranslateMessage(MSG* pMsg)
{
	m_ToolTip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CMMDViewerDlg::OnBnClickedLoadFile()
{
	MkPathName filePath;
	if (filePath.GetSingleFilePathFromDialog(m_hWnd))
	{
		_LoadTargetFile(filePath);
	}
}

void CMMDViewerDlg::OnBnClickedReloadCurrentFile()
{
	if ((!m_CurrPath.Empty()) && m_CurrPath.CheckAvailable())
	{
		MkPathName filePath = m_CurrPath;
		_LoadTargetFile(filePath);
	}
}

void CMMDViewerDlg::OnBnClickedOverwriteCurrentFile()
{
	if (!m_CurrPath.Empty())
	{
		MkPathName filePath = m_CurrPath;
		_SaveTargetFile(m_DataNode, filePath, true);
	}
}

void CMMDViewerDlg::OnBnClickedSaveCurrentFileAs()
{
	MkPathName filePath;
	if (filePath.GetSaveFilePathFromDialog(m_hWnd))
	{
		_SaveTargetFile(m_DataNode, filePath, true);
	}
}

void CMMDViewerDlg::OnBnClickedOpenCurrentFolder()
{
	if (!m_CurrPath.Empty())
	{
		m_CurrPath.GetPath().OpenDirectoryInExplorer();
	}
}

void CMMDViewerDlg::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	POINT screenPt;
	::GetCursorPos(&screenPt);
	POINT clientPt = screenPt;
	m_TC_NodeView.ScreenToClient(&clientPt);
	
	CRect rect;
	m_TC_NodeView.GetClientRect(&rect);
	if (rect.PtInRect(clientPt) == FALSE)
		return;
	
	UINT flags = 0;
	HTREEITEM currItem = m_TC_NodeView.HitTest(clientPt, &flags);
	if (currItem == FALSE)
		return;

	m_TC_NodeView.SelectItem(currItem);
	DWORD_PTR itemData = m_TC_NodeView.GetItemData(currItem);

	// node
	if (itemData != NULL)
	{
		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenuW(MF_STRING, 1, L"텍스트 폼을 클립보드로 복사");
		menu.AppendMenuW(MF_STRING, 2, L"다른 이름으로 저장");

		int menuID = menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, screenPt.x, screenPt.y, this);
		switch (menuID)
		{
		case 1:
			{
				const MkDataNode* node = reinterpret_cast<const MkDataNode*>(itemData);
				_CopyToClipboard(*node, MkStr::EMPTY);
			}
			break;

		case 2:
			{
				MkPathName filePath;
				if (filePath.GetSaveFilePathFromDialog(m_hWnd))
				{
					const MkDataNode* node = reinterpret_cast<const MkDataNode*>(itemData);
					_SaveTargetFile(*node, filePath, false);
				}
			}
			break;
		}

		menu.DestroyMenu();
	}
	// unit
	else
	{
		HTREEITEM parentItem = m_TC_NodeView.GetNextItem(currItem, TVGN_PARENT);
		if (parentItem != NULL)
		{
			itemData = m_TC_NodeView.GetItemData(parentItem);
			if (itemData != NULL)
			{
				CMenu menu;
				menu.CreatePopupMenu();
				menu.AppendMenuW(MF_STRING, 1, L"텍스트 폼을 클립보드로 복사");

				if (menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, screenPt.x, screenPt.y, this) == 1)
				{
					MkStr currText = m_TC_NodeView.GetItemText(currItem).GetBuffer();
					MkStr unitKey;
					if (currText.GetAt(0) == L'\"')
					{
						unitKey = MkDataNode::DataTypeTag.GetString();
					}
					else
					{
						currText.GetFirstBlock(0, L" ", L" ", unitKey);
					}

					const MkDataNode* node = reinterpret_cast<const MkDataNode*>(itemData);
					_CopyToClipboard(*node, unitKey);
				}

				menu.DestroyMenu();
			}
		}
	}

	*pResult = 0;
}

void CMMDViewerDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	HTREEITEM currItem = pNMTreeView->itemNew.hItem;

	// 노드만 대상으로 함
	const MkDataNode* targetNode = NULL;
	while (currItem != NULL)
	{
		DWORD_PTR itemData = m_TC_NodeView.GetItemData(currItem);
		if (itemData != NULL)
		{
			targetNode = reinterpret_cast<const MkDataNode*>(itemData);
			break;
		}

		currItem = m_TC_NodeView.GetNextItem(currItem, TVGN_PARENT);
	}

	if (targetNode != NULL)
	{
		MkArray<MkHashStr> nameList;
		targetNode->GetNodeNameListFromRootNode(nameList); // 최소 하나는 존재

		MkStr msg;
		msg.Reserve(1024);
		MK_INDEXING_LOOP(nameList, i)
		{
			msg += (i == 0) ? L"\"" : L" :: \"";
			msg += nameList[i].GetString();
			msg += L"\"";
		}

		m_EB_HierarchyView.SetWindowTextW(msg.GetPtr());
	}
	
	*pResult = 0;
}

//------------------------------------------------------------------------------------------------//

void CMMDViewerDlg::_UpdateSizeInfo(UINT nType, int cx, int cy)
{
	if ((nType == SIZE_RESTORED) || (nType == SIZE_MAXIMIZED))
	{
		if (m_EB_HierarchyView.m_hWnd != NULL)
		{
			RECT rect;
			m_EB_HierarchyView.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_EB_HierarchyView.SetWindowPos(NULL, 0, 0, cx, rect.bottom - rect.top, SWP_NOZORDER | SWP_NOMOVE);
		}

		if (m_TC_NodeView.m_hWnd != NULL)
		{
			RECT rect;
			m_TC_NodeView.GetWindowRect(&rect);
			ScreenToClient(&rect);
			m_TC_NodeView.SetWindowPos(NULL, 0, 0, cx, cy - rect.top, SWP_NOZORDER | SWP_NOMOVE);
		}

		if (MkDevPanel::InstancePtr() != NULL)
		{
			MK_DEV_PANEL.__ResetAlignmentPosition();
		}
	}
}

void CMMDViewerDlg::_ClearCurrentNode(void)
{
	m_DataNode.Clear();
	m_CurrPath.Clear();
	
	m_EB_HierarchyView.SetWindowTextW(L"");
}

void CMMDViewerDlg::_UpdateAppTitle(void)
{
	MkPathName filePath;
	MkStr fileName, fileExt;
	m_CurrPath.SplitPath(filePath, fileName, fileExt);

	MkStr msg;
	if (fileName.Empty())
	{
		msg = L"MMD Viewer";
	}
	else
	{
		msg.Reserve(512);
		msg += fileName;
		msg += L".";
		msg += fileExt;
		msg += L" - MMD Viewer (";

		if (filePath.GetSize() <= 128)
		{
			msg += filePath;
		}
		else
		{
			MkStr tmp;
			filePath.GetSubStr(MkArraySection(filePath.GetSize() - 127), tmp);
			msg += L"...";
			msg += tmp;
		}

		msg += L")";
	}

	SetWindowTextW(msg.GetPtr());
}

void CMMDViewerDlg::_UpdateControlAvailable(void)
{
	BOOL enableFile = m_CurrPath.Empty() ? FALSE : TRUE;

	m_BT_ReloadCurrentFile.EnableWindow(enableFile);

	m_RB_BinaryFile.EnableWindow(enableFile);
	m_RB_TextFile.EnableWindow(enableFile);
	m_RB_ExcelFile.EnableWindow(enableFile);
	m_RB_JsonFile.EnableWindow(enableFile);

	m_BT_OverwriteCurrentFile.EnableWindow(enableFile);
	m_BT_SaveCurrentFileAs.EnableWindow(enableFile);

	m_BT_OpenCurrentFolder.EnableWindow(enableFile);
}

void CMMDViewerDlg::_UpdateTreeCtrl(HTREEITEM parentItem, const MkDataNode& node)
{
	if (parentItem == NULL)
	{
		m_TC_NodeView.DeleteAllItems();

		parentItem = m_TC_NodeView.AddDataNodeItem(TVI_ROOT, node);
		if (parentItem == NULL)
			return;
	}
	
	// node
	MkArray<MkHashStr> templateNodeList, normalNodeList;
	node.__GetClassifiedChildNodeNameList(templateNodeList, normalNodeList, false); // valid 체크 안하고 모든 노드를 받음

	// 1st. template node
	MK_INDEXING_LOOP(templateNodeList, i)
	{
		_AddNodeItem(parentItem, *node.GetChildNode(templateNodeList[i]));
	}

	// 2nd. unit
	MkArray<MkHashStr> unitList;
	node.GetUnitKeyList(unitList);
	unitList.SortInAscendingOrder();

	MK_INDEXING_LOOP(unitList, i)
	{
		m_TC_NodeView.AddDataUnitItem(parentItem, node, unitList[i]);
	}

	// 3rd. normal node
	MK_INDEXING_LOOP(normalNodeList, i)
	{
		_AddNodeItem(parentItem, *node.GetChildNode(normalNodeList[i]));
	}
}

void CMMDViewerDlg::_AddNodeItem(HTREEITEM parentItem, const MkDataNode& node)
{
	HTREEITEM childItem = m_TC_NodeView.AddDataNodeItem(parentItem, node);
	if (childItem != NULL)
	{
		_UpdateTreeCtrl(childItem, node);
	}
}

void CMMDViewerDlg::_LoadTargetFile(const MkPathName& filePath)
{
	_ClearCurrentNode();

	MK_DEV_PANEL.MsgToLog(L"> " + filePath);

	MkDataNode::eLoadResult result;
	if (m_DataNode.Load(filePath, result))
	{
		m_CurrPath = filePath;

		MkPathName::SetWorkingDirectory(m_CurrPath.GetPath());

		MK_DEV_PANEL.MsgToLog(L"  읽기 성공");
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(L"  읽기 실패");
	}

	_UpdateAppTitle();
	_UpdateControlAvailable();
	_UpdateTreeCtrl(NULL, m_DataNode);

	m_TC_NodeView.Expand(m_TC_NodeView.GetRootItem(), TVE_EXPAND);

	switch (result)
	{
	case MkDataNode::eLR_Text:
		m_RB_BinaryFile.SetCheck(FALSE);
		m_RB_TextFile.SetCheck(TRUE);
		m_RB_ExcelFile.SetCheck(FALSE);
		m_RB_JsonFile.SetCheck(FALSE);
		break;
	case MkDataNode::eLR_Binary:
		m_RB_BinaryFile.SetCheck(TRUE);
		m_RB_TextFile.SetCheck(FALSE);
		m_RB_ExcelFile.SetCheck(FALSE);
		m_RB_JsonFile.SetCheck(FALSE);
		break;
	case MkDataNode::eLR_Excel:
		m_RB_BinaryFile.SetCheck(FALSE);
		m_RB_TextFile.SetCheck(FALSE);
		m_RB_ExcelFile.SetCheck(TRUE);
		m_RB_JsonFile.SetCheck(FALSE);
		break;
	case MkDataNode::eLR_Json:
		m_RB_BinaryFile.SetCheck(FALSE);
		m_RB_TextFile.SetCheck(FALSE);
		m_RB_ExcelFile.SetCheck(FALSE);
		m_RB_JsonFile.SetCheck(TRUE);
		break;
	}
}

void CMMDViewerDlg::_SaveTargetFile(const MkDataNode& node, const MkPathName& filePath, bool updateData)
{
	// backup?
	if ((m_CB_AutoBackup.GetCheck() == TRUE) && filePath.CheckAvailable())
	{
		MkStr dateStr = MK_SYS_ENV.GetCurrentSystemDate();
		dateStr.RemoveKeyword(L".");
		dateStr.PopFront(2);

		MkStr timeStr = MK_SYS_ENV.GetCurrentSystemTime();
		timeStr.RemoveKeyword(L":");

		MkPathName backupPath;
		MkStr fileName, fileExt;
		filePath.SplitPath(backupPath, fileName, fileExt);

		MkPathName newFilePath;
		newFilePath.Reserve(256);
		newFilePath += L"_Backup\\";
		newFilePath += fileName;
		newFilePath += L"(";
		newFilePath += dateStr;
		newFilePath += L"-";
		newFilePath += timeStr;
		newFilePath += L").";
		newFilePath += fileExt;

		MK_DEV_PANEL.InsertEmptyLine();
		MK_DEV_PANEL.MsgToLog(L"> " + newFilePath);

		backupPath += newFilePath;
		if (!filePath.CopyCurrentFile(backupPath))
		{
			MK_DEV_PANEL.MsgToLog(L"  자동 저장 실패");
			::MessageBox(m_hWnd, L"백업 파일 생성 실패. 저장 작업 취소.", L"MMD Editor", MB_OK);
			return;
		}

		MK_DEV_PANEL.MsgToLog(L"  자동 저장 완료");
	}

	// binary
	bool ok = false;
	if ((m_RB_BinaryFile.GetCheck() == TRUE) && (m_RB_TextFile.GetCheck() == FALSE) && (m_RB_ExcelFile.GetCheck() == FALSE) && (m_RB_JsonFile.GetCheck() == FALSE))
	{
		ok = node.SaveToBinary(filePath);
	}
	// text
	else if ((m_RB_BinaryFile.GetCheck() == FALSE) && (m_RB_TextFile.GetCheck() == TRUE) && (m_RB_ExcelFile.GetCheck() == FALSE) && (m_RB_JsonFile.GetCheck() == FALSE))
	{
		ok = node.SaveToText(filePath);
	}
	// excel
	else if ((m_RB_BinaryFile.GetCheck() == FALSE) && (m_RB_TextFile.GetCheck() == FALSE) && (m_RB_ExcelFile.GetCheck() == TRUE) && (m_RB_JsonFile.GetCheck() == FALSE))
	{
		ok = node.SaveToExcel(filePath);
	}
	// json
	else if ((m_RB_BinaryFile.GetCheck() == FALSE) && (m_RB_TextFile.GetCheck() == FALSE) && (m_RB_ExcelFile.GetCheck() == FALSE) && (m_RB_JsonFile.GetCheck() == TRUE))
	{
		ok = node.SaveToJson(filePath);
	}
	else
		return;

	MK_DEV_PANEL.InsertEmptyLine();
	MK_DEV_PANEL.MsgToLog(L"> " + filePath);

	if (ok)
	{
		if (updateData)
		{
			m_CurrPath = filePath;

			_UpdateAppTitle();
			_UpdateControlAvailable();

			MkPathName::SetWorkingDirectory(m_CurrPath.GetPath());
		}

		MK_DEV_PANEL.MsgToLog(L"  저장 완료");
	}
	else
	{
		MK_DEV_PANEL.MsgToLog(L"  저장 실패");
		::MessageBox(m_hWnd, L"파일 저장 실패.", L"MMD Editor", MB_OK);
	}
}

void CMMDViewerDlg::_CopyToClipboard(const MkDataNode& node, const MkHashStr& key)
{
	MkStr msg;

	// node
	if (key.Empty())
	{
		MkByteArray buffer;
		MkDataNodeToMemoryConverter dataNodeToMemoryConverter;
		if (!dataNodeToMemoryConverter.ConvertToMemory(node, buffer))
			return;
		
		MkMemoryToDataTextConverter memoryToDataTextConverter;
		if (!memoryToDataTextConverter.Convert(buffer, msg))
			return;
	}
	// unit
	else
	{
		unsigned int dataSize = node.GetDataSize(key);
		if (dataSize == 0)
			return;

		MkArray<MkStr> valueStr;
		CNodeTreeCtrl::ConvertUnitValueToText(node, key, valueStr, true);

		unsigned int strSize = 20 + key.GetSize();
		MK_INDEXING_LOOP(valueStr, i)
		{
			strSize += valueStr[i].GetSize() + 4;
		}
		msg.Reserve(strSize);

		ePrimitiveDataType unitType = node.GetUnitType(key);
		msg += MkPrimitiveDataType::GetTag(unitType);
		msg += L" ";
		msg += key.GetString();
		msg += L" ";
		msg += MkTagDefinitionForDataNode::TagForUnitAssign;
		msg += L" ";

		if (dataSize == 1)
		{
			msg += valueStr[0];
		}
		else // dataSize > 1
		{
			msg += L"// [";
			msg += dataSize;
			msg += L"]";

			unsigned int lastIndex = dataSize - 1;
			MK_INDEXING_LOOP(valueStr, i)
			{
				bool lineFeed = ((unitType == ePDT_Str) || (unitType == ePDT_ByteArray) || ((i % 5) == 0)); // line feed 조건
				msg += (lineFeed) ? L"\r\n\t" : L" ";
				msg += valueStr[i];
				if (i < lastIndex)
				{
					msg += L" ";
					msg += MkTagDefinitionForDataNode::TagForArrayDivider;
				}
			}
		}

		msg += MkTagDefinitionForDataNode::TagForUnitEnd;
	}
	
	MkClipboard::SetText(m_hWnd, msg);
}

//------------------------------------------------------------------------------------------------//