#ifndef __MINIKEY_CORE_MKFILESYSTEM_H__
#define __MINIKEY_CORE_MKFILESYSTEM_H__


//------------------------------------------------------------------------------------------------//
// ���� ��ŷ �ý��� ������
// �ټ��� original ������ �Ҽ��� chunk�� �����ֱ⸸ �ص� �ε��� �ð� �̵� �߻�
// ������ ������� ����� ����/����� ���·� �����
//
// �ϳ��� MkFileSystem�� �ϳ��� ���� �ý����� ����. ���ø����̼� ��κ��� �ϳ��� ����ص� ���
//
// ������ ûũ ���Ͽ� ���� ����/�߰��� ���� ����� disable��Ű�� ���κп� �߰��ϴ� ������ ûũ ���� �������� �ּ�ȭ
// ��� �������� disable �κ��� �������� �Ѿ ��� ûũ ���� ��ü�� ����ȭ�� �籸�� �� �� ����
//
// ûũ ���� �ڿ� ������ ��Ͻ� ���� �߻��ϸ� ���� ���ʱ�ȭ�� ���� ����� �ڵ� ������
//
// �ϳ��� ûũ ������ ũ�� ���� ȿ���� ���������� ����ȭ�� ���� ���� ����� Ŀ��
// �ݴ�� �۰� �����ϸ� ��Ű¡�� ������ ������Ƿ� ������ ���� ũ�� ���� �ʿ�
//
// ��ŷ�� ���Ͽ� ���� ������ ����η� �̷����
// ex>
//	L"D:\\OriginalRoot\\local\\map\\test.dds" ������ SetUpFromOriginalDirectory(L"D:\\OriginalRoot", ...)��
//	��ŷ�Ǿ����� ���� �� ���Ͽ� ���� �ý��� �� ��δ� L"local\\map\\test.dds" �� ��
//	-> GetOriginalFileData(L"local\\map\\test.dds", buffer);
//	-> ExtractAvailableFile(L"local\\map\\test.dds", L"..\\out_0\\");
//
//	���� �������� ������ root directory�� L"C:\\applications\\clock\\FileRoot\\"�� ���¿��� filePath��
//	L"C:\\applications\\clock\\FileRoot\\local\\map\\test.dds"ó�� �����η� ���Դٸ�,
//	���������� �� root directory ���� ��� ���(L"local\\map\\test.dds")�� ��ȯ�� ���
//
// ���� ���� ���� ������ ����� ��ŷ ��� ���ϵ��� root directory ������ ���� ������ ����
// root directory�� �״�� ��ŷ, ���� �ٸ� ���ø����̼ǿ��� ûũ ���ϵ��� root directory�� ����
// ���� �ý����� ������ ��� �ϸ� ���߰� ���� ��� ������ ����� ����/��� ��� ȥ�� ����
//------------------------------------------------------------------------------------------------//


#include "MkCore_MkMap.h"
#include "MkCore_MkHashMap.h"
#include "MkCore_MkHashStr.h"
#include "MkCore_MkFileChunk.h"


class MkFilePathListContainer;
class MkDataNode;

class MkFileSystem
{
public:

	//------------------------------------------------------------------------------------------------//
	// �ʱ� ����
	//------------------------------------------------------------------------------------------------//

	// �ϳ��� ûũ�� ���� ũ�� ����(MB ���� : 1 ~ 4095)
	// ���̵�����̹Ƿ� ������ ������ �ƴ�(������ ��¦ �Ѿ)
	void SetChunkSizeGuideline(unsigned int chunkSizeInMB = 512);

	// ���� ��� ����(% ���� : 0 ~ 100)
	// �����(���� �� ũ��/���� ũ��)�� ���� �ʴٸ� ���� ������ ������ �����ϱ⶧���� ���� ��� ������ ��츸 ������ ����
	// 0�̸� ���� ������ ���� �ʰ� 100 �̻��̸� ��κ� ����(�幰�� ������ �뷮�� �ణ �þ�� ��쵵 ����)
	// �������� ���� �ܻ��� bmp�� 20~40% ������ �̹� ����Ǿ� �ִ� jpg, mp3 ���� 90~100% �����̹Ƿ� �����ϰ� ����
	void SetPercentageForCompressing(unsigned int percentageForCompressing = 70);

	// ûũ ���ϸ� ��Ģ ����
	// ex> prefix == L"MK_CHUNK_SYSTEM_", extension == L"mcs" -> "MK_CHUNK_SYSTEM_0.mcs", "MK_CHUNK_SYSTEM_1.mcs", "MK_CHUNK_SYSTEM_2.mcs", ...
	void SetChunkFileNamingRule(const MkStr& prefix = L"MK_CHUNK_SYSTEM_", const MkStr& extension = L"mcs");

	// SetUpFromOriginalDirectory(), UpdateFromOriginalDirectory() ȣ��� ���� �� ���͸� ��Ģ
	// ���� �׸� ���ؼ��� MkPathName::GetBlackFileList() ����
	void SetFilterForOriginalFiles
		(const MkArray<MkPathName>* exceptionFilter, const MkArray<MkPathName>* nameFilter, const MkArray<MkPathName>* extensionFilter, const MkArray<MkStr>* prefixFilter);

	// �־��� �ý��� ���� data node(����)�� ����� ����
	// (in) node/settingFilePath : �ý��� ���� data node/������ ����, Ȥ�� root directory ���� ��� ���丮 ���
	// return : ���� ����
	// node ex>
	//	uint ChunkSizeInMB = 512;
	//	uint PercentageForCompressing = 70;
	//	str Prefix = "MK_CHUNK_SYSTEM_";
	//	str Extension = "mcs";
	//	str NameFilter = "datanode_00.bin" / "abc\src_2.bmp" / "def/";
	//	str ExtensionFilter = "exe" / "txt";
	//	//str PrefixFilter = "";
	//	str ExceptionFilter = "test_0.txt";
	void SetSystemSetting(const MkDataNode& node);
	bool SetSystemSetting(const MkPathName& settingFilePath);
	bool SetSystemSetting(void); // root directory�� DefaultSettingFileName ������ ����Ͽ� �ε�

	//------------------------------------------------------------------------------------------------//
	// �ʱ�ȭ
	//------------------------------------------------------------------------------------------------//

	// ���� ��Ű¡
	// ���� ���丮�� ���� ���ϵ�κ��� pack�� �����ϰ� ûũ ���ϵ��� ����
	// ��� ���� �˻��� black filter ���
	// ���� ũ�Ⱑ 0�� ������ ���Խ�Ű�� ����
	// (NOTE) SetFilterForOriginalFiles()���� ������ ���͸� ��Ģ ���� ����
	// (in) absolutePathOfBaseDirectory : ��ŷ �� ���� ���丮 ������
	// (in) workingDirectoryPath : ûũ ���ϵ��� ������ ����, Ȥ�� root directory ���� ��� ���丮 ���
	// return : ���� ����
	bool SetUpFromOriginalDirectory(const MkPathName& absolutePathOfBaseDirectory, const MkPathName& workingDirectoryPath);

	// ������ �ý��� ������ ����� ��Ű¡ �� ûũ ���ϵ�κ��� ����
	// (in) workingDirectoryPath : ûũ ���ϵ��� �����ϴ� ����, Ȥ�� root directory ���� ��� ���丮 ���
	// return : ���� ����
	bool SetUpFromChunkFiles(const MkPathName& workingDirectoryPath);

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// ��ϵ� ��� ûũ ��
	inline unsigned int GetTotalChunkCount(void) const { return m_ChunkTable.GetSize(); }

	// ��ϵ� ��� ���� ��
	inline unsigned int GetTotalFileCount(void) const { return m_SearchTable.GetSize(); }

	// ûũ ���� �� ��ȿ ũ�� ���� ��ȯ
	// (out) buffer : <ûũ ���� ��, ��ȿ ũ�� ���� �ۼ�Ʈ> ������ ���̺�
	// return : ûũ ���翩��
	bool GetBlankPercentage(MkMap<MkStr, unsigned int>& buffer) const;

	// ûũ ���� �� ��ȿ ũ�� ������ blankPercentage�� ���� �ʰ��ϴ� ûũ �ε��� ����Ʈ�� ��ȯ
	// (in) blankPercentage : ûũ������ �� ���� ����
	// return : ��� ûũ ���翩��
	bool GetBlankChunks(unsigned int blankPercentage, MkArray<unsigned int>& chunkIndice) const;

	// �ش� ��θ��� ���� ������ ��Ű¡ �� ���翩�� ��ȯ
	// (in) filePath : ���� ���. ����θ� �״��, �����θ� �� root directory ���� ����η� ��ȯ
	// return : ���翩��
	bool CheckAvailable(const MkPathName& filePath) const;

	// �ش� ��θ��� ���õ� ���� ���͸� ����ϴ��� ����(��ŷ�ǵ��� ������ ��������) ��ȯ
	// (in) filePath : ����/���丮 ���. ����θ� �״��, �����θ� �� root directory ���� ����η� ��ȯ
	// return : �������
	bool CheckFileFilter(const MkPathName& fileOrDirPath) const;

	// �ش� ��θ��� ���� ������ ��Ű¡ �� ���� ��ȯ
	// (in) filePath : ���� ���. ����θ� �״��, �����θ� �� root directory ���� ����η� ��ȯ
	// (out) originalSize : ������ ���� ũ��
	// (out) dataSize : ������ ���� ũ��. originalSize�� �����ϸ� ������� ���� �������� �ǹ�
	// (out) writtenTime : ������ ���� �Ͻ�
	// return : ���翩��
	bool GetFileInfo(const MkPathName& filePath, unsigned int& originalSize, unsigned int& dataSize, unsigned int& writtenTime) const;

	// �ش� ��θ��� ���� ������ ��Ű¡ �� ������ �������� ���� ��ȯ
	// (in) filePath : ���� ���. ����θ� �״��, �����θ� �� root directory ���� ����η� ��ȯ
	// (in) originalSize : ������ ���� ũ��
	// (in) writtenTime : ������ ���� �Ͻ�
	// return : ������ ������ -1, �����ϰ� �����ϸ� 0, ���������� �ٸ��� 1 ��ȯ
	int GetFileDifference(const MkPathName& filePath, unsigned int originalSize, unsigned int writtenTime) const;

	// ��Ű¡ �� ��ȿ�� ���� ���� �����͸� buffer�� ��� ��ȯ
	// (in) filePath : ���� ���. ����θ� �״��, �����θ� �� root directory ���� ����η� ��ȯ
	// (out) buffer : �����Ͱ� ��� ����
	// return : ��������
	bool GetOriginalFileData(const MkPathName& filePath, MkByteArray& buffer) const;

	// ��Ű¡ �� ��ȿ�� ������ ���� ���Ϸ� ����
	// (in) filePath : ���� ���. ����θ� �״��, �����θ� �� root directory ���� ����η� ��ȯ
	// (in) destinationDirectoryPath : ������ ����, Ȥ�� root directory ���� ��� ���丮 ���
	// return : ���� ����
	bool ExtractAvailableFile(const MkPathName& filePath, const MkPathName& destinationDirectoryPath) const;

	// ��Ű¡ �� ��� ��ȿ ���ϵ��� ���� ���Ϸ� ����
	// (in) destinationDirectoryPath : ������ ����, Ȥ�� root directory ���� ��� ���丮 ���
	// return : ���� ����
	bool ExtractAllAvailableFiles(const MkPathName& destinationDirectoryPath) const;

	// ���� �ý��� ������ MkDevPanel�� ���
	// (NOTE) ȣ�� ������ �翬�ϰԵ� MkDevPabnel ���� ���Ŀ� ����
	void PrintSystemInfoToDevPanel(bool printFilterInfo = false) const;

	// MkDataNode�� ������ ���� ���� ���
	// (out) node : ��� �� MkDataNode
	// (in) includeChunkInfo : ���Ϻ��� �Ҽ� chunk�� block index������ ���Խ�ų�� ����
	void ExportSystemStructure(MkDataNode& node, bool includeChunkInfo = false) const;

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// ��ŷ �ý��ۿ��� ���� ����
	// ���� ûũ ���Ͽ��� ���ŵ����� �ʰ� ���¸� disable�� ����
	// (in) filePath : ���� ���. ����θ� �״��, �����θ� �� root directory ���� ����η� ��ȯ
	// return : ���� ����
	bool RemoveFile(const MkPathName& filePath);

	// ���� ���� ����
	// �������� �ʴ� �����̸� �߰�. �̹� �����ϴ� ������ ��� ���� ũ�Ⱑ 0�� ��� ����, �׷��� ������ ����
	// (in) absolutePathOfBaseDirectory : root�� ���� �� ���� ���丮 ������
	// (in) relativePathOfFile : root������ ���� ��� ���. ���� �˻��� key(file path)�� ����
	// (in) writtenTime : �־��� ���� ���� �ð�. 0�� ��� ���� ������ ���� ���
	// return : ���� ����
	// ex>
	//	L"c:\\_Tmp\\Text\\test.txt" ������ ���� �ý��ۿ� L"Text\\test.txt"�� �����Ϸ� �� ���,
	//	absolutePathOfBaseDirectory : L"c:\\_Tmp\\"
	//	relativePathOfFile : L"Text\\test.txt"
	bool UpdateFromOriginalFile(const MkPathName& absolutePathOfBaseDirectory, const MkPathName& relativePathOfFile, unsigned int writtenTime = 0);

	// ��ŷ �ý��ۿ� �־��� ���丮 ������ ��� ���� ����(���丮 ������� ���� ��)
	// �������� �ʴ� �����̸� �߰�. �̹� �����ϴ� ������ ��� ���� ũ�Ⱑ 0�� ��� ����, �׷��� ������ ����
	// �ټ��� ������ �Ѳ����� �����ϰ��� �� �� ȿ����
	// (NOTE) SetFilterForOriginalFiles()���� ������ ���͸� ��Ģ ���� ����
	// (in) absolutePathOfBaseDirectory : ���� �� ���� ���丮 ������. ��ΰ� �������� ������ ���� ������ ���ٰ� ����
	// (in) removeOrgFilesAfterUpdating : ���� �Ϸ� �� ���� ���� ��������
	// return : ���� ����
	bool UpdateFromOriginalDirectory(const MkPathName& absolutePathOfBaseDirectory, bool removeOrgFilesAfterUpdating = false);

	// ��� ûũ�� ������� ���ط� �˻��� ����ȭ
	// RemoveFile(), UpdateFromOriginalDirectory()�� ȣ��� ûũ�� �� ������ ���� ���� �̻� ������ ��� ���� ����ȭ ����
	// (in) percentageForOptimizing : ûũ������ �� ���� ������ �־��� �ۼ�Ʈ ���� �̻��� ��� ����ȭ ����
	// return : ���� ����
	// ex> percentageForOptimizing == 30%, chunk_0(25%), chunk_1(34%), chunk_2(30%) �� ��� chunk_1, chunk_2 ����ȭ ����
	bool OptimizeAllChunks(unsigned int percentageForOptimizing);

	// ���� ûũ ����ȭ
	// (in) chunkIndex : �ش� ûũ �ε���
	// return : ���� ����
	bool OptimizeChunk(unsigned int chunkIndex);

	// ����
	void Clear(void);

	MkFileSystem();
	~MkFileSystem() { Clear(); }

protected:

	typedef struct _FileBlockIndex
	{
		unsigned int chunkIndex;
		unsigned int blockIndex;
	}
	FileBlockIndex;

	void _GenerateChunkFilePath(MkPathName& chunkFilePath, unsigned int index) const;

	bool _GetFileBlockIndex(const MkPathName& filePath, FileBlockIndex& fbi) const;

	bool _CreateNewChunkFromOriginalFiles(unsigned int chunkIndex, MkFilePathListContainer& filePathListContainer);

	void _RegisterPathListToSearchTable(const MkArray<MkPathName>& memberFilePathList, unsigned int chunkIndex, unsigned int blockOffset);

	void _ExportSystemStructure(MkDataNode& node, const MkArray<MkStr>& token, unsigned int depth, const FileBlockIndex& fbi, bool includeChunkInfo) const;

	bool _UpdateFilesToAvailableChunk(const MkArray<unsigned int>& availableChunks, MkFilePathListContainer& filePathListContainer);
	bool _UpdateFilesToNewChunk(MkFilePathListContainer& filePathListContainer);

protected:

	// ���� ����
	unsigned int m_ChunkSizeGuideline;
	unsigned int m_PercentageForCompressing;
	MkStr m_ChunkFilePrefix;
	MkStr m_ChunkFileExtension;
	MkPathName::BlackFilter m_PathFilter;

	// ������ ûũ ������ �����ϴ� ���� ���丮 ���
	MkPathName m_AbsoluteWorkingDirectoryPath;

	// ûũ ���̺�. ûũ �ϳ��� ûũ ���� �ϳ��� ��ġ��
	MkMap<unsigned int, MkFileChunk> m_ChunkTable;

	// �˻� ���̺�. hash map(�ӵ� �켱)
	// ��Ȱ�� ���(MkFileBlock::eDisable)�� ���Ե��� ����
	MkHashMap<MkHashStr, FileBlockIndex> m_SearchTable;

public:

	// ���� �ý��� �⺻ �������ϸ�
	static const MkPathName DefaultSettingFileName;
};

//------------------------------------------------------------------------------------------------//

#endif
