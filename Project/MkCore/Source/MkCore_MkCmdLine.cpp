
#include <Windows.h>
#include "MkCore_MkCmdLine.h"

//---------------------------------------------------------------------------//

const MkStr MkCmdLine::Separator(MKDEF_CMDLINE_SEPARATOR);
const MkStr MkCmdLine::Assigner(MKDEF_CMDLINE_ASSIGNER);

void MkCmdLine::SetUp(void)
{
	Clear();

	const wchar_t* cmdLine = ::GetCommandLine();
	int lineCount = 0;
	LPWSTR* cmdLines = ::CommandLineToArgvW(cmdLine, &lineCount);
	if (lineCount > 1)
	{
		for (int i=1; i<lineCount; ++i)
		{
			*this += MkStr(cmdLines[i]);
		}
	}
}

MkCmdLine& MkCmdLine::operator = (const MkStr& cmdLine)
{
	Clear();
	*this += cmdLine;
	return *this;
}

MkCmdLine& MkCmdLine::operator = (const MkCmdLine& cmdLine)
{
	m_Lines = cmdLine.GetNormalLines();
	m_Pairs = cmdLine.GetPairs();
	return *this;
}

MkCmdLine& MkCmdLine::operator += (const MkStr& cmdLine)
{
	if (!cmdLine.Empty())
	{
		_Parse(cmdLine);
	}
	return *this;
}

void MkCmdLine::AddPair(const MkStr& key, const MkStr& value)
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

bool MkCmdLine::RemovePair(const MkStr& key)
{
	bool ok = m_Pairs.Exist(key);
	if (ok)
	{
		m_Pairs.Erase(key);
	}
	return ok;
}

void MkCmdLine::GetFullStr(MkStr& buffer) const
{
	buffer.Clear();

	bool needSpace = false;
	if (!m_Lines.Empty())
	{
		MK_INDEXING_LOOP(m_Lines, i)
		{
			if (needSpace)
			{
				buffer += L" ";
			}
			else
			{
				needSpace = true;
			}

			buffer += m_Lines[i];
			buffer += Separator;
		}

		if (!m_Pairs.Empty())
		{
			buffer += L" ";
		}
	}

	if (!m_Pairs.Empty())
	{
		MkConstMapLooper<MkStr, MkArray<MkStr> > keyLooper(m_Pairs);
		MK_STL_LOOP(keyLooper)
		{
			const MkArray<MkStr>& values = keyLooper.GetCurrentField();
			MK_INDEXING_LOOP(values, i)
			{
				if (needSpace)
				{
					buffer += L" ";
				}
				else
				{
					needSpace = true;
				}

				buffer += keyLooper.GetCurrentKey();
				buffer += Assigner;
				buffer += values[i];
				buffer += Separator;
			}
		}
	}
}

void MkCmdLine::Clear(void)
{
	m_Lines.Clear();
	m_Pairs.Clear();
}

void MkCmdLine::_Parse(const MkStr& cmdLine)
{
	// Separator를 사용해 라인 단위로 자름
	MkArray<MkStr> lines;
	cmdLine.Tokenize(lines, Separator);

	MK_INDEXING_LOOP(lines, i)
	{
		MkStr& currLine = lines[i];
		bool isNormalLine = true; // key-value pair가 아닌 일반 라인 여부

		// key-value pair인지 검사해 처리
		if (currLine.CountKeyword(Assigner) > 0)
		{
			MkStr lineCopy = currLine;

			// key-value pair는 공문자를 인정하지 않음
			lineCopy.RemoveBlank();

			// key가 존재해야 pair로 인식. 없으면 일반 라인 취급
			MkStr key;
			unsigned int vPos = lineCopy.GetFirstBlock(0, Assigner, key);
			if ((vPos != MKDEF_ARRAY_ERROR) && (!key.Empty()))
			{
				MkStr value;
				lineCopy.GetSubStr(MkArraySection(vPos), value);
				AddPair(key, value); // value는 비었어도 정상적인 pair로 인정해 추가
				isNormalLine = false;
			}
		}

		// 일반 라인이고 유효문자가 존재하면 앞/뒤 공문자 제거해 등록
		if (isNormalLine && (currLine.GetFirstValidPosition() >= 0))
		{
			currLine.RemoveFrontSideBlank();
			currLine.RemoveRearSideBlank();

			m_Lines.PushBack(currLine);
		}
	}
}

//---------------------------------------------------------------------------//
