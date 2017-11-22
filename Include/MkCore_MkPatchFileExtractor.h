#pragma once

//------------------------------------------------------------------------------------------------//
// ��ġ �����丮 ������
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkPathName.h"


class MkPatchFileExtractor
{
public:

	bool SetDirectoryPath(const MkPathName& historyDirPath, const MkPathName& sourceDirPath);

	inline const MkArray<unsigned int>& GetHistoryList(void) const { return m_HistoryList; }

	bool MakeUpdateHistory(unsigned int startIndex, unsigned int endIndex);
	bool Extract(const MkPathName& targetDirPath);

protected:

	bool _ErrorMsg(const MkStr& msg) const;
	bool _ErrorMsg(const MkStr& filePath, const MkStr& msg) const;

protected:

	MkPathName m_HistoryDirPath;
	MkArray<unsigned int> m_HistoryList; // �������� ���� �� ���·� ����

	MkPathName m_SourceDirPath;
	MkHashMap<MkHashStr, unsigned int> m_SourceTable;
};

//------------------------------------------------------------------------------------------------//
