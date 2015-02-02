#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - drawing monitor
//
// ������ ���� ����� ���� ī����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"


#define MK_DRAWING_MONITOR MkDrawingMonitor::Instance()

class MkDrawingMonitor : public MkSingletonPattern<MkDrawingMonitor>
{
public:

	void Reset(void);

	inline void IncreaseRenderTargetCounter(void) { ++m_RenderTargetCounter; }

	inline void IncreaseValidPanelCounter(unsigned int count) { m_ValidPanelCounter += count; }
	inline void IncreaseValidTextureCounter(unsigned int count) { m_ValidTextureCounter += count; }
	inline void IncreasePanelSetCounter(unsigned int count) { m_PanelSetCounter += count; }

	void EndOfFrame(void);

	MkDrawingMonitor() : MkSingletonPattern<MkDrawingMonitor>() { Reset(); }
	virtual ~MkDrawingMonitor() {}


protected:

	unsigned int m_RenderTargetCounter;

	unsigned int m_ValidPanelCounter;
	unsigned int m_ValidTextureCounter;
	unsigned int m_PanelSetCounter;

};
