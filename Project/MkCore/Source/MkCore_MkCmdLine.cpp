
#include <Windows.h>
#include "MkCore_MkStringOp.h"
#include "MkCore_MkCmdLine.h"

//---------------------------------------------------------------------------//

const std::string MkCmdLine::Separator(MKDEF_CMDLINE_SEPARATOR);
const std::string MkCmdLine::Assigner(MKDEF_CMDLINE_ASSIGNER);


MkCmdLine& MkCmdLine::operator = (const char* cmdLine)
{
	Clear();
	*this += cmdLine;
	return *this;
}

MkCmdLine& MkCmdLine::operator = (const MkCmdLine& cmdLine)
{
	m_FullStr = cmdLine.GetFullStr();
	m_Lines = cmdLine.GetNormalLines();
	m_Pairs = cmdLine.GetPairs();
	return *this;
}

MkCmdLine& MkCmdLine::operator += (const char* cmdLine)
{
	if (cmdLine != NULL)
	{
		std::string cmdStr = cmdLine;
		_Parse(cmdStr);
		UpdateFullStr();
	}
	return *this;
}

void MkCmdLine::AddPair(const std::string& key, const std::string& value)
{
	if (m_Pairs.Exist(key))
	{
		m_Pairs[key].PushBack(value);
	}
	else
	{
		m_Pairs.Create(key).PushBack(value);
	}
}

bool MkCmdLine::RemovePair(const std::string& key)
{
	bool ok = m_Pairs.Exist(key);
	if (ok)
	{
		m_Pairs.Erase(key);
	}
	return ok;
}

void MkCmdLine::UpdateFullStr(void)
{
	if (!m_FullStr.empty())
	{
		m_FullStr.clear();
	}

	bool needSpace = false;
	if (!m_Lines.Empty())
	{
		MK_INDEXING_LOOP(m_Lines, i)
		{
			if (needSpace)
			{
				m_FullStr += " ";
			}
			else
			{
				needSpace = true;
			}

			m_FullStr += m_Lines[i];
			m_FullStr += Separator;
		}

		if (!m_Pairs.Empty())
		{
			m_FullStr += " ";
		}
	}

	if (!m_Pairs.Empty())
	{
		MkMapLooper<std::string, MkArray<std::string> > keyLooper(m_Pairs);
		MK_STL_LOOP(keyLooper)
		{
			MkArray<std::string>& values = keyLooper.GetCurrentField();
			MK_INDEXING_LOOP(values, i)
			{
				if (needSpace)
				{
					m_FullStr += " ";
				}
				else
				{
					needSpace = true;
				}

				m_FullStr += keyLooper.GetCurrentKey();
				m_FullStr += Assigner;
				m_FullStr += values[i];
				m_FullStr += Separator;
			}
		}
	}
}

void MkCmdLine::Clear(void)
{
	if (!m_FullStr.empty())
	{
		m_FullStr.clear();
	}

	m_Lines.Clear();
	m_Pairs.Clear();
}

void MkCmdLine::_Parse(const std::string& cmdStr)
{
	// Separator를 사용해 라인 단위로 자름
	std::vector<std::string> lines;
	MkStringOp::Tokenize(cmdStr, lines, Separator);

	for (size_t i=0; i<lines.size(); ++i)
	{
		std::string& currLine = lines[i];
		bool isNormalLine = true; // key-value pair가 아닌 일반 라인 여부

		// key-value pair인지 검사해 처리
		if (MkStringOp::CountKeyword(currLine, Assigner) > 0)
		{
			std::string lineCopy = currLine;

			// key-value pair는 공문자를 인정하지 않음
			MkStringOp::RemoveBlank(lineCopy);

			// key가 존재해야 pair로 인식. 없으면 일반 라인 취급
			std::string key;
			if (MkStringOp::GetFirstBlock(lineCopy, Assigner, key, 0, true) &&
				(!key.empty()))
			{
				AddPair(key, lineCopy); // value는 비었어도 정상적인 pair로 인정해 추가
				isNormalLine = false;
			}
		}

		// 일반 라인이고 유효문자가 존재하면 앞/뒤 공문자 제거해 등록
		if (isNormalLine && (MkStringOp::GetFirstValidPosition(currLine) >= 0))
		{
			MkStringOp::RemoveFrontSideBlank(currLine);
			MkStringOp::RemoveRearSideBlank(currLine);

			m_Lines.PushBack(currLine);
		}
	}
}

//---------------------------------------------------------------------------//
