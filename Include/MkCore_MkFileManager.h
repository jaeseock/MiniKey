#ifndef __MINIKEY_CORE_MKFILEMANAGER_H__
#define __MINIKEY_CORE_MKFILEMANAGER_H__


//------------------------------------------------------------------------------------------------//
// global instance - file manager
//
// MkFileSystem ��� ��ŷ �ý����� ������ ���� ����
// ��� ���Ǽ��� ���� ���� ����(�������� ���Ѵٸ� MkFileSystem�� ����� ���� ����)
// - �ϳ��� ���ø����̼��� �ϳ��� ��ŷ �ý��۸� ���
// - chunk file���� root directory�� ��ġ
//   (���߽� root directory ������ ��� ���� ���ϵ��� ��ġ���� �״�� ��ŷ�ϸ� ���߰� ���� ���
//    ������ ����� ����/��� ��θ� ���� �ǹǷ� �����鿡�� ����)
// - ���� Ǯ ��ŷ, ���� ����, ���� ���� �������� ���̽��̹Ƿ� �������� �ʰ� ��뿡�� ����
//
// (NOTE) ��ü ���� ���� chunk file���� �����ϴ� root directory�� ���õǾ� �־�� ��(MkPathName::SetUp())
//
// MkFileManager ��ü�� �������� ���� ���� �����Ƿ� ���� ��ü�� ȣ������ �ʰ� static api�� ����� ���� ����
//
// ���� ������ ���� ��� �켱. ���� ��ο� ���ٸ� ��ŷ �ý����� ������ ��ȯ
// MkFileManager ��ü�� �������� ���� ��� ���� ��θ� ������� ��
//
// ����(�ʱ�ȭ)/������ �����ϸ� ��� ����/���ΰ��� �����Ű�� �ʰ� �б⸸ �����ϹǷ� ��ü ����/������
// �� �ѹ���, �� �� �����忡���� ����ȴٰ� �����ϰ� thread �̽��� �Ű澲�� ����
//------------------------------------------------------------------------------------------------//
// ex>
//	// ���� ���ø����̼��� ��Ʈ ���丮�� ���� ��Ű¡. �� ���� �����ϸ� ��
//	MkFileSystem pack;
//	pack.SetChunkSizeGuideline(5);
//	pack.SetPercentageForCompressing(70);
//	pack.SetUpFromOriginalDirectory(L"D:\\Solutions\\CurrentProject\\MiniKey\\Samples\\EmptyConsoleProject\\bin\\Src\\", L"");
//	pack.Clear();
//
// //...
//
//	// ���ø����̼� �ʱ�ȭ�� ��Ű¡ �� ûũ�� �Ŵ��� �ʱ�ȭ
//	new MkFileManager(L"", true);
//
//	// ûũ�κ��� L"a\\src_2.bmp" ���� �� L"..\\out\\src_2.bmp" ��ο� ����� ���� Ȯ��
//	MkByteArray buffer;
//	if (MkFileManager::GetFileData(L"a\\src_2.bmp", buffer))
//	{
//		MkInterfaceForFileWriting fwInterface;
//		fwInterface.SetUp(L"..\\out\\src_2.bmp");
//		fwInterface.Write(buffer, MkArraySection(0));
//		buffer.Clear();
//	}
//
//	// �ؽ�Ʈ ���� ���� Ȯ��
//	MkStr str;
//	str.ReadTextFile(L"test_0.txt");
//	str += L"\n\n";
//	str.ReadTextFile(L"test_1.txt", false);
//	str.WriteToTextFile(L"..\\out\\test_merge.txt");
//
//------------------------------------------------------------------------------------------------//


#include "MkCore_MkSingletonPattern.h"
#include "MkCore_MkFileSystem.h"


class MkFileManager : public MkSingletonPattern<MkFileManager>
{
public:

	//------------------------------------------------------------------------------------------------//
	// ���
	//------------------------------------------------------------------------------------------------//

	// ���� ��� Ȥ�� ��Ű¡ �ý��� �� ���� ���翩�� ��ȯ
	// (in) filePath : ����, Ȥ�� root directory ���� ��� ���� ���
	// return : ���� ����
	static bool CheckAvailable(const MkPathName& filePath);

	// ���� ��� Ȥ�� ��Ű¡ �ý��� ���� ���� ������ ��ȯ
	// (in) filePath : ����, Ȥ�� root directory ���� ��� ���� ���
	// (out) buffer : �����Ͱ� ��� ����
	// return : ���� ����
	// (NOTE) ���� ũ�Ⱑ 0�̴��� ������ �����ϸ� true ��ȯ
	static bool GetFileData(const MkPathName& filePath, MkByteArray& buffer);

	// ���� ����� ���Ϸ� ������ ���
	// (in) filePath : ����, Ȥ�� root directory ���� ��� ���� ���
	// (in) buffer : �����Ͱ� ��� ����
	// (in) overwrite : ���� ������ ���� �� ��� true�� ����� false�� �ڿ� ������
	// (in) makeDirectoryPath : ���ϱ����� ��ΰ� �����Ǿ� ���� ���� ��� �ڵ� ��� ���� ����
	// return : ���� ����
	static bool SaveDataToFile(const MkPathName& filePath, const MkByteArray& buffer, bool overwrite = true, bool makeDirectoryPath = true);

	//------------------------------------------------------------------------------------------------//
	// implementation
	//------------------------------------------------------------------------------------------------//

	static bool __GetFileDataInRealPath(const MkPathName& filePath, MkByteArray& buffer);
	
	bool __CheckAvailable(const MkPathName& filePath) const;
	bool __GetOriginalFileData(const MkPathName& filePath, MkByteArray& buffer) const;

	void __PrintSystemInformationToLog(void) const;

	inline MkFileSystem& GetFileSystem(void) { return m_FileSystem; }

	MkFileManager(const MkPathName& workingDirectoryPath, bool loadChunk); // ��ŷ �ý��� ���� ������ ���� ��� ���⼭ ����
	virtual ~MkFileManager() { m_FileSystem.Clear(); }

protected:

	MkFileSystem m_FileSystem;
};

#endif
