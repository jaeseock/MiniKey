#ifndef __MINIKEY_CORE_MKINTERFACEFORFILETAG_H__
#define __MINIKEY_CORE_MKINTERFACEFORFILETAG_H__


//------------------------------------------------------------------------------------------------//
// 파일처리의 특수한 예로 파일내 특정 태그의 참조/수정을 위한 interface
// 메모리 침범을 피하기 위해 크기가 고정된 자료형만 사용 가능(MKDEF_DECLARE_FIXED_SIZE_TYPE)
// (NOTE) 수정시 overwrite 형태이므로 메모리 오염 주의를 요함
//
// ex>
//	// data -> MkByteArray(srcArray)
//	MkInterfaceForDataWriting dwInterface;
//	dwInterface.Write(true);
//	dwInterface.Write(static_cast<unsigned int>(1234));
//	dwInterface.Write(MkVec2(12.f, 34.f));
//	dwInterface.Write(static_cast<unsigned int>(5678));
//	
//	MkByteArray srcArray;
//	unsigned int inputSize = dwInterface.Flush(srcArray);
//	dwInterface.Clear();
//
//	// MkByteArray(srcArray) -> file
//	MkInterfaceForFileWriting fwInterface;
//	fwInterface.SetUp(L"test_block.bin", true, true);
//	fwInterface.Write(srcArray, MkArraySection(0));
//	srcArray.Clear();
//	fwInterface.Clear();
//
//	// file tag management
//	MkInterfaceForFileTag<unsigned int> ftInterface;
//	ftInterface.SetUp(L"test_block.bin");
//	unsigned int pos0 = sizeof(bool);
//	unsigned int pos1 = sizeof(bool) + sizeof(unsigned int) + sizeof(MkVec2);
//	unsigned int tag_0 = ftInterface.GetTag(pos0); // 1234
//	unsigned int tag_1 = ftInterface.GetTag(pos1); // 5678
//	ftInterface.Overwrite(tag_1, pos0); // tag_0 <-> tag_1
//	ftInterface.Overwrite(tag_0, pos1);
//	unsigned int new_tag_0 = ftInterface.GetTag(pos0); // 5678
//	unsigned int new_tag_1 = ftInterface.GetTag(pos1); // 1234
//	ftInterface.Clear();
//------------------------------------------------------------------------------------------------//

#include <fstream>
#include "MkCore_MkCheck.h"
#include "MkCore_MkPathName.h"


template <class DataType>
class MkInterfaceForFileTag
{
public:

	// 초기화 (파일 오픈 후 대기)
	// (in) filePath : 수정할 파일의 절대 혹은 root directory기준 상대 경로
	// return : 파일오픈 성공여부
	bool SetUp(const MkPathName& filePath)
	{
		// 경로 구성
		m_CurrentFilePath.ConvertToRootBasisAbsolutePath(filePath);
		bool ok = m_CurrentFilePath.CheckAvailable();
		MK_CHECK(ok, filePath + L" 경로에 파일이 존재하지 않음")
			return false;

		// 파일 오픈
		m_Stream.open(m_CurrentFilePath, std::ios::in | std::ios::out | std::ios::binary);
		ok = m_Stream.is_open();
		MK_CHECK(ok, filePath + L" 경로 파일 열기 실패")
			return false;
		
		// 크기 확인
		m_Stream.seekg(0, std::ios_base::end);
		m_FileSize = static_cast<unsigned int>(m_Stream.tellg());
		m_Stream.seekg(0);
		return true;
	}

	// 수정 유효성(파일이 열려 있고 EOF가 아닌 상태) 검사
	inline bool IsValid(void) const { return (m_Stream.is_open() && (!m_Stream.eof())); }

	// tag 읽기
	// (in) beginPosition : 파일내 읽기 시작 위치
	// return : 읽어들인 값. MKDEF_ARRAY_ERROR면 에러(혹은 EOF)
	inline DataType GetTag(unsigned int beginPosition = 0)
	{
		if ((!IsFixedSizeType<DataType>::Get()) || (!IsValid()) || ((beginPosition + sizeof(DataType)) > m_FileSize))
			return MKDEF_ARRAY_ERROR;

		DataType buffer;
		m_Stream.seekg(beginPosition);
		m_Stream.read(reinterpret_cast<char*>(&buffer), sizeof(DataType));
		return buffer;
	}

	// tag 수정
	// SetUp() ~ Clear() 사이에서 복수 호출 가능
	// (in) tag : 수정 값
	// (in) beginPosition : 파일내 수정 위치
	// return : 수정 성공여부
	inline bool Overwrite(const DataType& tag, unsigned int beginPosition = 0)
	{
		if ((!IsFixedSizeType<DataType>::Get()) || (!IsValid()) || ((beginPosition + sizeof(DataType)) > m_FileSize))
			return false;

		m_Stream.seekg(beginPosition);
		m_Stream.write(reinterpret_cast<const char*>(&tag), sizeof(DataType));
		return true;
	}

	// 파일 닫음
	inline void Clear(void)
	{
		if (m_Stream.is_open())
		{
			m_Stream.close();
			m_FileSize = 0;
		}
	}

	// 현재 오픈중인 파일 절대 경로 반환
	inline const MkPathName& GetCurrentFilePath(void) const { return m_CurrentFilePath; }

	MkInterfaceForFileTag() { m_FileSize = 0; }
	~MkInterfaceForFileTag() { Clear(); }

protected:

	std::fstream m_Stream;
	unsigned int m_FileSize;

	MkPathName m_CurrentFilePath;
};

//------------------------------------------------------------------------------------------------//

#endif
