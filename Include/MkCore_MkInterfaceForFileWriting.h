#ifndef __MINIKEY_CORE_MKINTERFACEFORFILEWRITING_H__
#define __MINIKEY_CORE_MKINTERFACEFORFILEWRITING_H__


//------------------------------------------------------------------------------------------------//
// MkByteArray�� ���Ͽ� �����ϱ� ���� interface
// ��ϵ� �����ʹ� Ư���� ������ �±� ���� ���� �Ϲ����� ���̳ʸ� ����
// �ٸ� ���Ͽ��� MkByteArray�� �ٽ� ������Ű�� �ʹٸ� MkInterfaceForFileReading ����
//
// ex>
//	// MkByteArray(srcArray) -> file
//	MkInterfaceForFileWriting fwInterface;
//	fwInterface.SetUp(L"testblock\\test_block.bin", true, true);
//	fwInterface.Write(srcArray, MkArraySection(0));
//	fwInterface.Clear();
//------------------------------------------------------------------------------------------------//

#include <fstream>
#include "MkCore_MkPathName.h"


class MkInterfaceForFileWriting
{
public:

	// �ʱ�ȭ (���� ���� �� ���)
	// (in) filePath : ����� ������ ���� Ȥ�� root directory���� ��� ���
	// (in) overwrite : ���� ������ ���� �� ��� true�� ����� false�� �ڿ� ������
	// (in) makeDirectoryPath : ���ϱ����� ��ΰ� �����Ǿ� ���� ���� ��� �ڵ� ��� ���� ����
	// return : ���Ͽ��� ��������
	bool SetUp(const MkPathName& filePath, bool overwrite = true, bool makeDirectoryPath = true);

	// SetUp���� ������� ��ϵ� ���� ����� ��ȯ
	inline unsigned int GetWrittenSize(void) const { return m_WrittenSize; }

	// ���� ��ȿ��(������ ���� �ִ� ����) �˻�
	inline bool IsValid(void) const { return m_Stream.is_open(); }

	// ���� Ž�� ��ġ ��ȯ. ��ȿ���� ������ MKDEF_ARRAY_ERROR ��ȯ
	// �̾� ����(overwrite == false) ���� ������ ��� ���� ���� ũ�� ���Ե�
	unsigned int GetCurrentPosition(void);

	// �޸� ����� ���
	// SetUp() ~ Clear() ���̿��� ���� ȣ�� ����
	// (in) srcArray : ���Ͽ� �� �����Ͱ� ��� byte array
	// (in) section : byte array ������ ����
	// return : ���� ��ϵ� ũ��
	unsigned int Write(const MkByteArray& srcArray, const MkArraySection& section);

	// ���� ����
	void Clear(void);

	// ���� �������� ���� ���� ��� ��ȯ
	inline const MkPathName& GetCurrentFilePath(void) const { return m_CurrentFilePath; }

	MkInterfaceForFileWriting() { m_WrittenSize = 0; }
	~MkInterfaceForFileWriting() { Clear(); }

protected:

	std::ofstream m_Stream;
	unsigned int m_WrittenSize;
	unsigned int m_InitialSize;

	MkPathName m_CurrentFilePath;
};

//------------------------------------------------------------------------------------------------//

#endif
