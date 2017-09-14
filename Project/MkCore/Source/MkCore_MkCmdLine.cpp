
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
	// Separator�� ����� ���� ������ �ڸ�
	MkArray<MkStr> lines;
	cmdLine.Tokenize(lines, Separator);

	MK_INDEXING_LOOP(lines, i)
	{
		MkStr& currLine = lines[i];
		bool isNormalLine = true; // key-value pair�� �ƴ� �Ϲ� ���� ����

		// key-value pair���� �˻��� ó��
		if (currLine.CountKeyword(Assigner) > 0)
		{
			MkStr lineCopy = currLine;

			// key-value pair�� �����ڸ� �������� ����
			lineCopy.RemoveBlank();

			// key�� �����ؾ� pair�� �ν�. ������ �Ϲ� ���� ���
			MkStr key;
			unsigned int vPos = lineCopy.GetFirstBlock(0, Assigner, key);
			if ((vPos != MKDEF_ARRAY_ERROR) && (!key.Empty()))
			{
				MkStr value;
				lineCopy.GetSubStr(MkArraySection(vPos), value);
				AddPair(key, value); // value�� ���� �������� pair�� ������ �߰�
				isNormalLine = false;
			}
		}

		// �Ϲ� �����̰� ��ȿ���ڰ� �����ϸ� ��/�� ������ ������ ���
		if (isNormalLine && (currLine.GetFirstValidPosition() >= 0))
		{
			currLine.RemoveFrontSideBlank();
			currLine.RemoveRearSideBlank();

			m_Lines.PushBack(currLine);
		}
	}
}

//---------------------------------------------------------------------------//
