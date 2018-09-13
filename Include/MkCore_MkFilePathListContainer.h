#ifndef __MINIKEY_CORE_MKFILEPATHLISTCONTAINER_H__
#define __MINIKEY_CORE_MKFILEPATHLISTCONTAINER_H__


//------------------------------------------------------------------------------------------------//
// 패킹 시스템에 등록될 원본 파일 경로 보관소
// (NOTE) 범위에 대한 검증 및 예외처리 하지 않음
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkPathName.h"

class MkFilePathListContainer
{
public:

	// 기반 절대 경로 반환
	inline const MkPathName& GetAbsolutePathOfBaseDirectory(void) const { return m_AbsolutePathOfBaseDirectory; }

	// 상대 파일 경로 배열 참조
	inline MkArray<MkPathName>& GetRelativeFilePathList(void) { return m_RelativeFilePathList; }

	// 단일 파일 추가
	inline void AddRelativeFilePath(const MkPathName& relativeFilePath, unsigned int writtenTime = 0)
	{
		m_RelativeFilePathList.PushBack(relativeFilePath);

		if (writtenTime != 0)
		{
			m_OuterWrittenTimeTable.Create(m_RelativeFilePathList.GetSize() - 1, writtenTime);
		}
	}

	// 상대 파일 경로 반환
	inline const MkPathName& GetRelativeFilePath(unsigned int index) const { return m_RelativeFilePathList[index]; }

	// 원본 파일 절대 경로 반환
	// 사전 설정된 경로가 없으면 기준 경로와 상대경로를 합성해 반환
	inline const MkPathName& GetAbsoluteOriginalFilePath(unsigned int index)
	{
		m_TemporaryAbsoluteOriginalFilePath.Clear();
		const MkPathName& relativeFilePath = m_RelativeFilePathList[index];
		m_TemporaryAbsoluteOriginalFilePath.Reserve(m_AbsolutePathOfBaseDirectory.GetSize() + relativeFilePath.GetSize());
		m_TemporaryAbsoluteOriginalFilePath += m_AbsolutePathOfBaseDirectory;
		m_TemporaryAbsoluteOriginalFilePath += relativeFilePath;
		return m_TemporaryAbsoluteOriginalFilePath;
	}

	// 외부 수정시간 반환
	inline unsigned int GetOuterWrittenTime(unsigned int index) const { return m_OuterWrittenTimeTable.Exist(index) ? m_OuterWrittenTimeTable[index] : 0; }

	// size만큼 삭제
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

	// 비었는지 여부
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
