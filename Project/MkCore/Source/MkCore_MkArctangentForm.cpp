#include <math.h>
#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkFloatOp.h"
#include "MkCore_MkArctangentForm.h"


#define MKDEF_ARCTAN_SEED_GEN_RADIAN 45.f

//------------------------------------------------------------------------------------------------//

bool MkArctangentForm::SetUp(float frontRange, float backRange)
{
	m_FrontRange = frontRange;
	m_LengthOfRange = backRange - m_FrontRange;

	if (MkFloatOp::CloseToZero(m_LengthOfRange))
	{
		m_LengthOfRange = 0.f;
		return false;
	}

	m_BeginWeight = atan(m_FrontRange * MKDEF_ARCTAN_SEED_GEN_RADIAN);
	m_EndWeight = atan(backRange * MKDEF_ARCTAN_SEED_GEN_RADIAN) - m_BeginWeight;

	return true;
}

float MkArctangentForm::GetGraph(float x) const
{
	if (m_LengthOfRange == 0.f)
		return 0.f;

	// 0 ~ 1 clamp
	x = Clamp<float>(x, 0.f, 1.f);

	// atan
	float atValue = atan((x * m_LengthOfRange + m_FrontRange) * MKDEF_ARCTAN_SEED_GEN_RADIAN) - m_BeginWeight;

	// convert to 0 ~ 1
	return (atValue / m_EndWeight);
}

//------------------------------------------------------------------------------------------------//
