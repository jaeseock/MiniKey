#ifndef __MINIKEY_CORE_MKFILEPATHLISTCONTAINER_H__
#define __MINIKEY_CORE_MKFILEPATHLISTCONTAINER_H__


//------------------------------------------------------------------------------------------------//
// ��ŷ �ý��ۿ� ��ϵ� ���� ���� ��� ������
// (NOTE) ������ ���� ���� �� ����ó�� ���� ����
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkPathName.h"

class MkFilePathListContainer
{
public:

	// ��� ���� ��� ��ȯ
	inline const MkPathName& GetAbsolutePathOfBaseDirectory(void) const { return m_AbsolutePathOfBaseDirectory; }

	// ��� ���� ��� �迭 ����
	inline MkArray<MkPathName>& GetRelativeFilePathList(void) { return m_RelativeFilePathList; }

	// ���� ���� �߰�
	inline void AddRelativeFilePath(const MkPathName& relativeFilePath, unsigned int writtenTime = 0)
	{
		m_RelativeFilePathList.PushBack(relativeFilePath);

		if (writtenTime != 0)
		{
			m_OuterWrittenTimeTable.Create(m_RelativeFilePathList.GetSize() - 1, writtenTime);
		}
	}

	// ��� ���� ��� ��ȯ
	inline const MkPathName& GetRelativeFilePath(unsigned int index) const { return m_RelativeFilePathList[index]; }

	// ���� ���� ���� ��� ��ȯ
	// ���� ������ ��ΰ� ������ ���� ��ο� ����θ� �ռ��� ��ȯ
	inline const MkPathName& GetAbsoluteOriginalFilePath(unsigned int index)
	{
		m_TemporaryAbsoluteOriginalFilePath.Clear();
		const MkPathName& relativeFilePath = m_RelativeFilePathList[index];
		m_TemporaryAbsoluteOriginalFilePath.Reserve(m_AbsolutePathOfBaseDirectory.GetSize() + relativeFilePath.GetSize());
		m_TemporaryAbsoluteOriginalFilePath += m_AbsolutePathOfBaseDirectory;
		m_TemporaryAbsoluteOriginalFilePath += relativeFilePath;
		return m_TemporaryAbsoluteOriginalFilePath;
	}

	// �ܺ� �����ð� ��ȯ
	inline unsigned int GetOuterWrittenTime(unsigned int index) const { return m_OuterWrittenTimeTable.Exist(index) ? m_OuterWrittenTimeTable[index] : 0; }

	// size��ŭ ����
	inline void PopBack(unsigned int size)
	{
		unsigned int lastSize = m_RelativeFilePathList.GetSize();
		if ((lastSize > 0) && (size > 0))
		{
			unsigned int index = (size < lastSize) ? (lastSize - size) : 0;
			while (index < lastSize)
			{
				m_OuterWrittenTimeTable.Erase(index);
				++index;
			}
		}

		m_RelativeFilePathList.PopBack(size);
	}

	// ������� ����
	inline bool Empty(void) { return m_RelativeFilePathList.Empty(); }

	inline MkFilePathListContainer() {}
	inline MkFilePathListContainer(const MkPathName& absolutePathOfBaseDirectory) { m_AbsolutePathOfBaseDirectory = absolutePathOfBaseDirectory; }

	inline ~MkFilePathListContainer()
	{
		m_AbsolutePathOfBaseDirectory.Clear();
		m_RelativeFilePathList.Clear();
		m_OuterWrittenTimeTable.Clear();
	}

protected:

	MkPathName m_AbsolutePathOfBaseDirectory;
	MkArray<MkPathName> m_RelativeFilePathList;
	MkMap<unsigned int, unsigned int> m_OuterWrittenTimeTable;
	MkPathName m_TemporaryAbsoluteOriginalFilePath;
};

//------------------------------------------------------------------------------------------------//

#endif
