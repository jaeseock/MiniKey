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

	inline void IncreaseValidObjectCounter(unsigned int count) { m_ValidObjectCounter += count; }
	inline void IncreaseValidMaterialCounter(unsigned int count) { m_ValidMaterialCounter += count; }
	inline void IncreaseObjectUnionCounter(unsigned int count) { m_ObjectUnionCounter += count; }

	void EndOfFrame(void);

	MkDrawingMonitor() : MkSingletonPattern<MkDrawingMonitor>() { Reset(); }
	virtual ~MkDrawingMonitor() {}


protected:

	unsigned int m_RenderTargetCounter;

	unsigned int m_ValidObjectCounter;
	unsigned int m_ValidMaterialCounter;
	unsigned int m_ObjectUnionCounter;

};
