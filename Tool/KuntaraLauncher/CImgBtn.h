
#pragma once

#include "GDIPlusManager.h"


class CImgBtn : public CButton
{
	DECLARE_DYNAMIC(CImgBtn)
public:

	enum eBtnState
	{
		eBS_Ready = 0,
		eBS_Over,
		eBS_Pushing,
		eBS_Disable,

		eBS_Max
	};

	bool SetButtonInfo(WORD id, int x, int y,
		const MkPathName& readyFilePath, const MkPathName& overFilePath, const MkPathName& pushingFilePath, const MkPathName& disableFilePath);

	bool SetButtonInfo(WORD id, int x, int y, int readyResID, int overResID, int pushingResID, int disableResID);

	CImgBtn();
	virtual ~CImgBtn();

protected:
	virtual void PreSubclassWindow();

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);

protected:

	GDIPlusManager::ImageSet m_ImageSet;

	bool m_Tracking;
	bool m_Hover;
};
