#ifndef __MINIKEY_CORE_MKSINTERFACEFORFILEREADING_H__
#define __MINIKEY_CORE_MKSINTERFACEFORFILEREADING_H__


//------------------------------------------------------------------------------------------------//
// ���� ���Ͽ��� �����͸� MkByteArray���� �����س��� ���� interface
//
// ex>
//	// file -> MkByteArray(destBuffer)
//	MkByteArray destBuffer;
//	MkInterfaceForFileReading frInterface;
//	frInterface.SetUp(L"testblock\\test_block.bin");
//	frInterface.Read(destBuffer, MkArraySection(0));
//	frInterface.Clear();
//------------------------------------------------------------------------------------------------//

#include <fstream>
#include "MkCore_MkPathName.h"


class MkInterfaceForFileReading
{
public:

	// �ʱ�ȭ (���� ���� �� ���)
	// (in) filePath : ����� ������ ���� Ȥ�� root directory���� ��� ���
	// return : ���Ͽ��� ��������
	bool SetUp(const MkPathName& filePath);

	// ���� ���� �ִ� ������ ��üũ�⸦ ��ȯ
	inline unsigned int GetFileSize(void) const { return m_FileSize; }

	// �б� ��ȿ��(������ ���� �ְ� EOF�� �ƴ� ����) ��ȯ
	bool IsValid(void) const;

	// ���� Ž�� ��ġ �̵�
	void SetCurrentPosition(unsigned int newPosition);

	// ���� Ž�� ��ġ ��ȯ. ��ȿ���� ������ MKDEF_ARRAY_ERROR ��ȯ
	unsigned int GetCurrentPosition(void);

	// �޸� ������� �о����
	// SetUp() ~ Clear() ���̿��� ���� ȣ�� ����
	// (in) destBuffer : �о���� �����Ͱ� ��� byte array
	// (in) section : ���ϳ� �о���� ����
	// return : ���� �о���� ũ��
	unsigned int Read(MkByteArray& destBuffer, const MkArraySection& section);

	// ���� ����
	void Clear(void);

	// ���� �������� ���� ���� ��� ��ȯ
	inline const MkPathName& GetCurrentFilePath(void) const { return m_CurrentFilePath; }

	MkInterfaceForFileReading() { m_FileSize = 0; }
	~MkInterfaceForFileReading() { Clear(); }

protected:

	std::ifstream m_Stream;
	unsigned int m_FileSize;

	MkPathName m_CurrentFilePath;
};

//------------------------------------------------------------------------------------------------//

#endif
