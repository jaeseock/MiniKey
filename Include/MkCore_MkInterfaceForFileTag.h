#ifndef __MINIKEY_CORE_MKINTERFACEFORFILETAG_H__
#define __MINIKEY_CORE_MKINTERFACEFORFILETAG_H__


//------------------------------------------------------------------------------------------------//
// ����ó���� Ư���� ���� ���ϳ� Ư�� �±��� ����/������ ���� interface
// �޸� ħ���� ���ϱ� ���� ũ�Ⱑ ������ �ڷ����� ��� ����(MKDEF_DECLARE_FIXED_SIZE_TYPE)
// (NOTE) ������ overwrite �����̹Ƿ� �޸� ���� ���Ǹ� ����
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

	// �ʱ�ȭ (���� ���� �� ���)
	// (in) filePath : ������ ������ ���� Ȥ�� root directory���� ��� ���
	// return : ���Ͽ��� ��������
	bool SetUp(const MkPathName& filePath)
	{
		// ��� ����
		m_CurrentFilePath.ConvertToRootBasisAbsolutePath(filePath);
		bool ok = m_CurrentFilePath.CheckAvailable();
		MK_CHECK(ok, filePath + L" ��ο� ������ �������� ����")
			return false;

		// ���� ����
		m_Stream.open(m_CurrentFilePath, std::ios::in | std::ios::out | std::ios::binary);
		ok = m_Stream.is_open();
		MK_CHECK(ok, filePath + L" ��� ���� ���� ����")
			return false;
		
		// ũ�� Ȯ��
		m_Stream.seekg(0, std::ios_base::end);
		m_FileSize = static_cast<unsigned int>(m_Stream.tellg());
		m_Stream.seekg(0);
		return true;
	}

	// ���� ��ȿ��(������ ���� �ְ� EOF�� �ƴ� ����) �˻�
	inline bool IsValid(void) const { return (m_Stream.is_open() && (!m_Stream.eof())); }

	// tag �б�
	// (in) beginPosition : ���ϳ� �б� ���� ��ġ
	// return : �о���� ��. MKDEF_ARRAY_ERROR�� ����(Ȥ�� EOF)
	inline DataType GetTag(unsigned int beginPosition = 0)
	{
		if ((!IsFixedSizeType<DataType>::Get()) || (!IsValid()) || ((beginPosition + sizeof(DataType)) > m_FileSize))
			return MKDEF_ARRAY_ERROR;

		DataType buffer;
		m_Stream.seekg(beginPosition);
		m_Stream.read(reinterpret_cast<char*>(&buffer), sizeof(DataType));
		return buffer;
	}

	// tag ����
	// SetUp() ~ Clear() ���̿��� ���� ȣ�� ����
	// (in) tag : ���� ��
	// (in) beginPosition : ���ϳ� ���� ��ġ
	// return : ���� ��������
	inline bool Overwrite(const DataType& tag, unsigned int beginPosition = 0)
	{
		if ((!IsFixedSizeType<DataType>::Get()) || (!IsValid()) || ((beginPosition + sizeof(DataType)) > m_FileSize))
			return false;

		m_Stream.seekg(beginPosition);
		m_Stream.write(reinterpret_cast<const char*>(&tag), sizeof(DataType));
		return true;
	}

	// ���� ����
	inline void Clear(void)
	{
		if (m_Stream.is_open())
		{
			m_Stream.close();
			m_FileSize = 0;
		}
	}

	// ���� �������� ���� ���� ��� ��ȯ
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
