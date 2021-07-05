
// MMD ViewerDlg.h : 헤더 파일
//

#pragma once

#include "MkCore_MkWin32Application.h"
#include "MkCore_MkDataNode.h"
#include "NodeTreeCtrl.h"


class CMMDViewerDlg : public CDialog
{
public:
	CMMDViewerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	enum { IDD = IDD_MMDVIEWER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedLoadFile();
	afx_msg void OnBnClickedReloadCurrentFile();
	afx_msg void OnBnClickedOverwriteCurrentFile();
	afx_msg void OnBnClickedSaveCurrentFileAs();
	afx_msg void OnBnClickedOpenCurrentFolder();

	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);

protected:

	void _UpdateSizeInfo(UINT nType, int cx, int cy);

	void _ClearCurrentNode(void);
	void _UpdateAppTitle(void);
	void _UpdateControlAvailable(void);
	void _UpdateTreeCtrl(HTREEITEM parentItem, const MkDataNode& node);
	void _AddNodeItem(HTREEITEM parentItem, const MkDataNode& node);

	void _LoadTargetFile(const MkPathName& filePath);
	void _SaveTargetFile(const MkDataNode& node, const MkPathName& filePath, bool updateData);
	void _CopyToClipboard(const MkDataNode& node, const MkHashStr& key);

protected:

	HICON m_hIcon;

	MkWin32Application m_MkApplication;
	MkDataNode m_DataNode;
	MkPathName m_CurrPath;
	
	CButton m_BT_ReloadCurrentFile;
	CButton m_RB_BinaryFile;
	CButton m_RB_TextFile;
	CButton m_RB_ExcelFile;
	CButton m_RB_JsonFile;
	CButton m_BT_OverwriteCurrentFile;
	CButton m_BT_SaveCurrentFileAs;
	CButton m_CB_AutoBackup;
	CButton m_BT_OpenCurrentFolder;
	CEdit m_EB_HierarchyView;
	CNodeTreeCtrl m_TC_NodeView;

	CImageList m_TreeImage;
	CToolTipCtrl m_ToolTip;
};
