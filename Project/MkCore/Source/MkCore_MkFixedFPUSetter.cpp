
#include <float.h>
#include "MkCore_MkFixedFPUSetter.h"


//------------------------------------------------------------------------------------------------//

void MkFixedFPUSetter::FixPrecision(void)
{
	if (m_ControlWord.GetSize() == 0)
	{
		unsigned int cw;
		_controlfp_s(&cw, _PC_24, MCW_PC);
		m_ControlWord.Push(cw);
	}
}

void MkFixedFPUSetter::Restore(void)
{
	if (m_ControlWord.GetSize() == 1)
	{
		unsigned int cw;
		m_ControlWord.Pop(cw);
		_controlfp_s(&cw, _CW_DEFAULT, MCW_PC);
	}
}

//------------------------------------------------------------------------------------------------//
