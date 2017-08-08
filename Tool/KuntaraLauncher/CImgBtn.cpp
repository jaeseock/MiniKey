
#include "stdafx.h"
#include "CImgBtn.h"


//------------------------------------------------------------------------------------------------//

IMPLEMENT_DYNAMIC(CImgBtn, CButton)

bool CImgBtn::SetButtonInfo
(WORD id, int x, int y,
 const MkPathName& readyFilePath, const MkPathName& overFilePath, const MkPathName& pushingFilePath, const MkPathName& disableFilePath)
{
	do
	{
		if (!m_ImageSet.SetSkin(id, static_cast<WORD>(eBS_Ready), readyFilePath))
			break;

		if (!m_ImageSet.SetSkin(id, static_cast<WORD>(eBS_Over), overFilePath))
			break;

		if (!m_ImageSet.SetSkin(id, static_cast<WORD>(eBS_Pushing), pushingFilePath))
			break;

		if (!m_ImageSet.SetSkin(id, static_cast<WORD>(eBS_Disable), disableFilePath))
			break;

		MkInt2 imgSize = m_ImageSet.GetImageSize(static_cast<WORD>(eBS_Ready));
		MoveWindow(x, y, imgSize.x, imgSize.y, 0);
		return true;
	}
	while (false);

	m_ImageSet.Clear();
	return false;
}

bool CImgBtn::SetButtonInfo(WORD id, int x, int y, int readyResID, int overResID, int pushingResID, int disableResID)
{
	do
	{
		if (!m_ImageSet.SetSkin(id, static_cast<WORD>(eBS_Ready), readyResID))
			break;

		if (!m_ImageSet.SetSkin(id, static_cast<WORD>(eBS_Over), overResID))
			break;

		if (!m_ImageSet.SetSkin(id, static_cast<WORD>(eBS_Pushing), pushingResID))
			break;

		if (!m_ImageSet.SetSkin(id, static_cast<WORD>(eBS_Disable), disableResID))
			break;

		MkInt2 imgSize = m_ImageSet.GetImageSize(static_cast<WORD>(eBS_Ready));
		MoveWindow(x, y, imgSize.x, imgSize.y, 0);
		return true;
	}
	while (false);

	m_ImageSet.Clear();
	return false;
}

CImgBtn::CImgBtn()
{
	m_Tracking = false;
	m_Hover = false;
}

CImgBtn::~CImgBtn()
{
	m_ImageSet.Clear();
}

BEGIN_MESSAGE_MAP(CImgBtn, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEHOVER()
END_MESSAGE_MAP()

void CImgBtn::PreSubclassWindow()
{
	CButton::PreSubclassWindow();

	ModifyStyle(0, BS_OWNERDRAW);
}

void CImgBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->CtlType == ODT_BUTTON)
	{
		eBtnState btnState = (m_Hover) ? eBS_Over : eBS_Ready;
		if ((lpDrawItemStruct->itemState & ODS_DISABLED) != 0)
		{
			btnState = eBS_Disable;
		}
		else if ((lpDrawItemStruct->itemState & ODS_SELECTED) != 0)
		{
			btnState = eBS_Pushing;
		}

		m_ImageSet.Draw(this, btnState);
   }
}

void CImgBtn::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_Tracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_Tracking = (::TrackMouseEvent(&tme) == TRUE);
	}

	CButton::OnMouseMove(nFlags, point);
}

void CImgBtn::OnMouseLeave()
{
	m_Tracking = false;
	m_Hover = false;
	Invalidate();

	CButton::OnMouseLeave();
}

void CImgBtn::OnMouseHover(UINT nFlags, CPoint point)
{
	m_Hover = true;
	Invalidate();

	CButton::OnMouseHover(nFlags, point);
}

//------------------------------------------------------------------------------------------------//
