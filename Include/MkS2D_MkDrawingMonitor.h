#pragma once


//------------------------------------------------------------------------------------------------//
// global instance - drawing monitor
//
// 렌더링 관련 디버깅 정보 카운터
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkSingletonPattern.h"


#define MK_DRAWING_MONITOR MkDrawingMonitor::Instance()


//------------------------------------------------------------------------------------------------//


class MkDrawingMonitor : public MkSingletonPattern<MkDrawingMonitor>
{
public:

	void Reset(void);

	inline void IncreaseRenderTargetCounter(void) { ++m_RenderTargetCounter; }

	inline void IncreaseValidSRectCounter(unsigned int count) { m_ValidSRectCounter += count; }
	inline void IncreaseValidDecoStrCounter(void) { ++m_ValidDecoStrCounter; }
	inline void IncreaseValidTextureCounter(unsigned int count) { m_ValidTextureCounter += count; }
	inline void IncreaseRectSetCounter(unsigned int count) { m_RectSetCounter += count; }

	void EndOfFrame(void);


	MkDrawingMonitor() : MkSingletonPattern<MkDrawingMonitor>() { Reset(); }
	virtual ~MkDrawingMonitor() {}


protected:

	unsigned int m_RenderTargetCounter;

	unsigned int m_ValidSRectCounter;
	unsigned int m_ValidDecoStrCounter;
	unsigned int m_ValidTextureCounter;
	unsigned int m_RectSetCounter;

};
