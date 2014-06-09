#pragma once


//------------------------------------------------------------------------------------------------//
// render 치트 처리
//
// - ChangeDisplayMode (width) (height) (mode)
//   ex> ChangeDisplayMode 1024 768 win
//       ChangeDisplayMode 800 600 full
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkCheatMessage.h"


class MkS2DCheatMessage : public MkCheatMessage
{
public:

	virtual bool ExcuteMsg(const MkStr& command, const MkArray<MkStr>& argument, MkStr& resultMsg) const;

public:

	MkS2DCheatMessage() : MkCheatMessage() {}
	virtual ~MkS2DCheatMessage() {}
};
