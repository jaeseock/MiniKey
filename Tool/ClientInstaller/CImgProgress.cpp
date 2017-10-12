
#include "stdafx.h"
#include "CImgProgress.h"


//------------------------------------------------------------------------------------------------//

IMPLEMENT_DYNAMIC(CImgProgress, CProgressCtrl)

bool CImgProgress::SetProgressInfo(unsigned int id, int x, int y, const MkPathName& frontFilePath, const MkPathName& backFilePath)
{
	do
	{
		if (!m_ImageSet.SetSkin(id, 0, frontFilePath))
			break;

		if (!m_ImageSet.SetSkin(id, 1, backFilePath))
			break;

		MkInt2 imgSize = m_ImageSet.GetImageSize(0);
		MoveWindow(x, y, imgSize.x, imgSize.y, 0);
		return true;
	}
	while (false);

	m_ImageSet.Clear();
	return false;
}

bool CImgProgress::SetProgressInfo(unsigned int id, int x, int y, int frontResID, int backResID)
{
	do
	{
		if (!m_ImageSet.SetSkin(id, 0, frontResID))
			break;

		if (!m_ImageSet.SetSkin(id, 1, backResID))
			break;

		MkInt2 imgSize = m_ImageSet.GetImageSize(0);
		MoveWindow(x, y, imgSize.x, imgSize.y, 0);
		return true;
	}
	while (false);

	m_ImageSet.Clear();
	return false;
}

CImgProgress::CImgProgress()
{
	
}

CImgProgress::~CImgProgress()
{
	m_ImageSet.Clear();
}

//------------------------------------------------------------------------------------------------//
BEGIN_MESSAGE_MAP(CImgProgress, CProgressCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

BOOL CImgProgress::OnEraseBkgnd(CDC* pDC)
{
	return TRUE; // 아무것도 그리지 않음
}

void CImgProgress::OnPaint()
{
	CPaintDC dc(this); // 없으면 블록 걸린다;;

	int lower = 0, upper = 0;
	GetRange(lower, upper);
	if (lower < upper)
	{
		float ratio = static_cast<float>(GetPos() - lower) / static_cast<float>(upper - lower);
		MkInt2 fImgSize = m_ImageSet.GetImageSize(0);
		int width = static_cast<int>(static_cast<float>(fImgSize.x) * ratio);

		if (width == 0) // 0%
		{
			m_ImageSet.Draw(this, 1); // backside only
		}
		else if (width == fImgSize.x) // 100%
		{
			m_ImageSet.Draw(this, 0); // frontside only
		}
		else
		{
			m_ImageSet.Draw(this, 0, MkInt2::Zero, MkInt2(width, fImgSize.y)); // frontside
			m_ImageSet.Draw(this, 1, MkInt2(width, 0), MkInt2(fImgSize.x - width, fImgSize.y)); // backside
		}
	}
}

void CImgProgress::OnNcPaint()
{
	// 아무것도 그리지 않음
}
