#pragma once


//------------------------------------------------------------------------------------------------//
// MkByteArray�� zip ���Ͽ� �����ϱ� ���� interface
//
// ex>
// MkByteArray(srcArray) -> file
//	MkInterfaceForZipFileWriting fwInterface;
//	fwInterface.SetUp(L"etc\\etc.zip", true, true);
//	fwInterface.Write(L"sample.sst", srcArray[0]);
//	fwInterface.Write(L"big.png", srcArray[1]);
//	fwInterface.Clear();
//------------------------------------------------------------------------------------------------//

#include "minizip/zip.h"
#include "MkCore_MkPathName.h"


class MkInterfaceForZipFileWriting
{
public:

	// �ʱ�ȭ (���� ���� �� ���)
	// (in) filePath : ����� ������ ���� Ȥ�� root directory���� ��� ���
	// (in) overwrite : ���� ������ ���� �� ��� true�� ����� false�� �߰��� ������
	// (in) makeDirectoryPath : ���ϱ����� ��ΰ� �����Ǿ� ���� ���� ��� �ڵ� ��� ���� ����
	// return : ���Ͽ��� ��������
	bool SetUp(const MkPathName& filePath, bool overwrite = true, bool makeDirectoryPath = true);

	// ���� ��ȿ��(������ ���� �ִ� ����) �˻�
	inline bool IsValid(void) const { return (m_ZipFile != NULL); }

	// �޸� ����� ������ ���
	// SetUp() ~ Clear() ���̿��� ���� ȣ�� ����
	// (NOTE) ���� �ߺ�üũ ���� ����. zip ������ ���� ������ ������ ���� �� �� ����
	// (in) filePath : ���� �� ������ ��� ���
	// (in) srcArray : ���Ͽ� �� �����Ͱ� ��� byte array
	// return : ��������
	bool Write(const MkPathName& filePath, const MkByteArray& srcArray);

	// ���� ����
	void Clear(void);

	// ���� �������� zip���� ���� ��� ��ȯ
	inline const MkPathName& GetCurrentFilePath(void) const { return m_CurrentFilePath; }

	MkInterfaceForZipFileWriting();
	~MkInterfaceForZipFileWriting() { Clear(); }

protected:

	zipFile m_ZipFile;

	MkPathName m_CurrentFilePath;
};
