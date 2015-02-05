#include "MkCore_MkDevPanel.h"
#include "MkPA_MkDrawingMonitor.h"


//------------------------------------------------------------------------------------------------//

void MkDrawingMonitor::Reset(void)
{
	m_RenderTargetCounter = 0;

	m_ValidPanelCounter = 0;
	m_ValidMaterialCounter = 0;
	m_PanelSetCounter = 0;
}

void MkDrawingMonitor::EndOfFrame(void)
{
	const static MkStr rtcHeader = L"Total render target count : ";
	MK_DEV_PANEL.__MsgToDrawingBoard(0, rtcHeader + MkStr(m_RenderTargetCounter));

	const static MkStr vrcHeader = L"Valid panel count : ";
	MK_DEV_PANEL.__MsgToDrawingBoard(1, vrcHeader + MkStr(m_ValidPanelCounter));

	const static MkStr vmcHeader = L"Valid material count : ";
	MK_DEV_PANEL.__MsgToDrawingBoard(2, vmcHeader + MkStr(m_ValidMaterialCounter));

	const static MkStr rscHeader = L"Panel set(Draw call) count : ";
	MK_DEV_PANEL.__MsgToDrawingBoard(3, rscHeader + MkStr(m_PanelSetCounter));
}

//------------------------------------------------------------------------------------------------//
