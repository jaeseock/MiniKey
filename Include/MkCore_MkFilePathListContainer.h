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

	// ��� ���� ��� �迭 ����
	inline MkArray<MkPathName>& GetRelativeFilePathList(void) { return m_RelativeFilePathList; }

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

	// size��ŭ ����
	inline void PopBack(unsigned int size)
	{
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
	}

protected:

	MkPathName m_AbsolutePathOfBaseDirectory;
	MkArray<MkPathName> m_RelativeFilePathList;
	MkPathName m_TemporaryAbsoluteOriginalFilePath;
};

//------------------------------------------------------------------------------------------------//

#endif
