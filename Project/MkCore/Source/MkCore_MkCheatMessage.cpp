
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkStringTableForParser.h"
#include "MkCore_MkCheatMessage.h"


//------------------------------------------------------------------------------------------------//

bool MkCheatMessage::ExcuteMsg(const MkStr& command, const MkArray<MkStr>& argument, MkStr& resultMsg) const
{
	MkStr cmd = command;
	cmd.ToLower();

	if ((cmd == L"q") && argument.Empty())
	{
		MK_DEV_PANEL.__QuitApplication();
		return true;
	}

	if ((cmd == L"cls") && argument.Empty())
	{
		MK_DEV_PANEL.ClearLogWindow();
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------//

bool MkCheatMessage::__ExcuteMsg(const MkStr& inputMsg, MkStr& resultMsg) const
{
	if (inputMsg.Empty())
		return false;

	MkStr msgCopy = inputMsg;

	MkStringTableForParser stringTable;
	stringTable.BuildStringTable(msgCopy);

	MkArray<MkStr> tokens, argument;
	MkStr command;
	unsigned int tokenCount = msgCopy.Tokenize(tokens);
	if (tokenCount == 0) // 공문자로만 이루어짐
	{
		return false;
	}
	else if (tokenCount == 1) // argument 없음
	{
		command = tokens[0];
	}
	else
	{
		command = tokens[0];
		argument.Reserve(tokenCount - 1);
		for (unsigned int i=1; i<tokenCount; ++i)
		{
			MkHashStr currToken = tokens[i];
			argument.PushBack(stringTable.Exist(currToken) ? stringTable.GetString(currToken) : tokens[i]);
		}
	}

	return ExcuteMsg(command, argument, resultMsg);
}

//------------------------------------------------------------------------------------------------//
