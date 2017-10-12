
#pragma once

#include "GDIPlusManager.h"


class CImgProgress : public CProgressCtrl
{
	DECLARE_DYNAMIC(CImgProgress)
public:

	bool SetProgressInfo(unsigned int id, int x, int y, const MkPathName& frontFilePath, const MkPathName& backFilePath);
	bool SetProgressInfo(unsigned int id, int x, int y, int frontResID, int backResID);

	CImgProgress();
	virtual ~CImgProgress();

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();

protected:

	GDIPlusManager::ImageSet m_ImageSet;
};
