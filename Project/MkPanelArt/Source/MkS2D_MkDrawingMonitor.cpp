

#include "MkCore_MkDevPanel.h"

#include "MkS2D_MkDrawingMonitor.h"


//------------------------------------------------------------------------------------------------//

void MkDrawingMonitor::Reset(void)
{
	m_RenderTargetCounter = 0;

	m_ValidSRectCounter = 0;
	m_ValidTextureCounter = 0;
	m_RectSetCounter = 0;
}

void MkDrawingMonitor::EndOfFrame(void)
{
	const static MkStr rtcHeader = L"Total render target count : ";
	MK_DEV_PANEL.__MsgToDrawingBoard(0, rtcHeader + MkStr(m_RenderTargetCounter));

	const static MkStr vrcHeader = L"Valid SRect count : ";
	MK_DEV_PANEL.__MsgToDrawingBoard(1, vrcHeader + MkStr(m_ValidSRectCounter));

	const static MkStr vtcHeader = L"Valid texture count : ";
	MK_DEV_PANEL.__MsgToDrawingBoard(2, vtcHeader + MkStr(m_ValidTextureCounter));

	const static MkStr rscHeader = L"Rect set(Draw call) count : ";
	MK_DEV_PANEL.__MsgToDrawingBoard(3, rscHeader + MkStr(m_RectSetCounter));
}

//------------------------------------------------------------------------------------------------//
