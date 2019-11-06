#pragma once


//------------------------------------------------------------------------------------------------//
// zip ���Ͽ��� �����͸� MkByteArray���� �����س��� ���� interface
//
// ex>
//	// file -> MkByteArray(destBuffer)
//	MkByteArray destBuffer[2];
//	MkInterfaceForZipFileReading frInterface;
//	frInterface.SetUp(L"etc\\etc.zip"); // zip���� �ȿ� "sample.sst", "image/big.png"�� �ִٸ�
//	frInterface.Read(L"etc\\sample.sst", destBuffer[0]);
//	frInterface.Read(L"etc\\image\\big.png", destBuffer[1]);
//	frInterface.Clear();
//------------------------------------------------------------------------------------------------//

#include "minizip/unzip.h"
#include "MkCore_MkPathName.h"


class MkInterfaceForZipFileReading
{
public:

	// �ʱ�ȭ (���� ���� �� ���)
	// (in) filePath : ���� zip������ ���� Ȥ�� root directory���� ��� ���
	// (in) addRelativePathToKey : zip���� �� ����Ʈ�� root directory���� ��� ��θ� �������� ����
	// return : ���Ͽ��� ��������
	bool SetUp(const MkPathName& filePath, bool addRelativePathToKey = true);

	// �б� ��ȿ�� ��ȯ
	inline bool IsValid(void) const { return (m_ZipFile != NULL); }

	// ���� �� ���� �� ��ȯ
	inline unsigned int GetFileCount(void) const { return m_FileInfos.GetSize(); }

	// ���� �� ��� ���� ��ο� �� ��ȯ
	inline unsigned int GetFileList(MkArray<MkHashStr>& buffer) const { return m_FileInfos.GetKeyList(buffer); }

	// �޸� ������� ���� Ǯ�� ���� �����͸� �о����
	// SetUp() ~ Clear() ���̿��� ���� ȣ�� ����
	// (in) filePath : �о���� ������ root directory���� ��� ���
	// (in) password : ��ȣȭ ������ ��� �ش� ��ȣ
	// (in) destBuffer : �о���� �����Ͱ� ��� byte array
	// return : ���� �о���� ũ��
	unsigned int Read(const MkPathName& filePath, MkByteArray& destBuffer);
	unsigned int Read(const MkPathName& filePath, const MkStr& password, MkByteArray& destBuffer);

	// ���� ����
	void Clear(void);

	// ���� �������� zip���� ���� ��� ��ȯ
	inline const MkPathName& GetCurrentFilePath(void) const { return m_CurrentFilePath; }

	MkInterfaceForZipFileReading();
	~MkInterfaceForZipFileReading() { Clear(); }

protected:

	typedef struct __CompFileInfo
	{
		uLong posInCentralDir;
		uLong numFile;
		bool needPassword;
	}
	_CompFileInfo;

	unsigned int _Read(const MkPathName& filePath, const MkStr& password, MkByteArray& destBuffer);

protected:

	unzFile m_ZipFile;

	MkHashMap<MkHashStr, _CompFileInfo> m_FileInfos;

	MkPathName m_CurrentFilePath;
};
