
#include "MkCore_MkStr.h"
#include "MkS2D_MkRenderer.h"
#include "MkS2D_MkS2DCheatMessage.h"


//------------------------------------------------------------------------------------------------//

bool MkS2DCheatMessage::ExcuteMsg(const MkStr& command, const MkArray<MkStr>& argument, MkStr& resultMsg) const
{
	if (MkCheatMessage::ExcuteMsg(command, argument, resultMsg))
		return true;

	if ((command == L"ChangeDisplayMode") && (argument.GetSize() == 3))
	{
		MkInt2 size(argument[0].ToInteger(), argument[1].ToInteger());
		MkStr buffer = argument[2];
		buffer.ToLower();
		bool fullScreen = ((buffer == L"f") || (buffer == L"full"));
		MK_RENDERER.ChangeDisplayMode(size, fullScreen);
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------//
