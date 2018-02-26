
#pragma once

#include "afxwin.h"
#include "afxcmn.h"

#include "MkCore_MkPathName.h"


class MkHashStr;
class MkDataNode;

class CNodeTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(CNodeTreeCtrl)
public:
	CNodeTreeCtrl();
	virtual ~CNodeTreeCtrl();

	HTREEITEM AddDataNodeItem(HTREEITEM parent, const MkDataNode& node);
	HTREEITEM AddDataUnitItem(HTREEITEM parent, const MkDataNode& parentNode, const MkHashStr& unitName);
	void AddDataArrayItem(HTREEITEM parent, const MkArray<MkStr>& values);
	bool GetDragFilePath(MkPathName& buffer);

	static void ConvertUnitValueToText(const MkDataNode& parentNode, const MkHashStr& unitName, MkArray<MkStr>& strBuffer, bool applyDQMTag);

	DECLARE_MESSAGE_MAP()

	static void _MakeNodeItemName(const MkDataNode& node, MkStr& buffer);

    afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDropFiles(HDROP hDropInfo);

protected:

	MkPathName m_DragFilePath;
};
