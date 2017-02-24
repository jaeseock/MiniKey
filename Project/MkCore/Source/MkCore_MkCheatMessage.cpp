
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

	msgCopy.RemoveKeyword(MkStr::CR);

	bool result = false;

	// 라인별 분리
	MkArray<MkStr> lines;
	unsigned int lineCount = msgCopy.Tokenize(lines, MkStr::LF);
	MK_INDEXING_LOOP(lines, i)
	{
		MkArray<MkStr> tokens;
		MkStr command;
		unsigned int tokenCount = lines[i].Tokenize(tokens);
		if (tokenCount > 0) // 유효문자가 존재하면
		{
			bool success = false;
			if (DoAutoParsing(tokens[0]))
			{
				command = tokens[0];

				MkArray<MkStr> argument;
				if (tokenCount > 1) // argument가 존재하면
				{
					argument.Reserve(tokenCount - 1);
					for (unsigned int i=1; i<tokenCount; ++i)
					{
						MkHashStr currToken = tokens[i];
						argument.PushBack(stringTable.Exist(currToken) ? stringTable.GetString(currToken) : tokens[i]);
					}
				}

				success = ExcuteMsg(command, argument, resultMsg);
			}
			else
			{
				MkStr lineMsg = tokens[0];
				for (unsigned int i=1; i<tokenCount; ++i)
				{
					MkHashStr currToken = tokens[i];
					lineMsg += MkStr::SPACE;
					lineMsg += stringTable.Exist(currToken) ? stringTable.GetString(currToken) : tokens[i];
				}

				success = ExcuteLine(lineMsg, resultMsg);
			}

			if (success)
			{
				result = true; // 하나라도 성공하면 의미 있음
			}
			else
			{
				resultMsg += MkStr::CRLF;
				resultMsg += L"> MkCheatMessage::\"" + lines[i] + L"\" 실행 실패";
			}
		}
	}

	return result;
}

//------------------------------------------------------------------------------------------------//
