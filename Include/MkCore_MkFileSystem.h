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
	void SetPercentageForCompressing(unsigned int percentageForCompressing = 60);

	// ûũ ���ϸ� ��Ģ ����
	// ex> prefix == L"MK_PACK_", extension == L"mcf" -> "MK_PACK_0.mcf", "MK_PACK_1.mcf", "MK_PACK_2.mcf", ...
	void SetChunkFileNamingRule(const MkStr& prefix = L"MK_PACK_", const MkStr& extension = L"mcf");

	//------------------------------------------------------------------------------------------------//
	// �ʱ�ȭ
	//------------------------------------------------------------------------------------------------//

	// ���� ��Ű¡
	// ���� ���丮�� ���� ���ϵ�κ��� pack�� �����ϰ� ûũ ���ϵ��� ����
	// ��� ���� �˻��� black filter ���
	// ���� ũ�Ⱑ 0�� ������ ���Խ�Ű�� ����
	// (in) absolutePathOfBaseDirectory : ��ŷ �� ���� ���丮 ������
	// (in) nameFilter : ���� �� ���� �̸� ����Ʈ
	// (in) extensionFilter : ���� �� ���� Ȯ���� ����Ʈ
	// (in) prefixFilter : ���� �� ���� ���λ� ����Ʈ
	// (in) exceptionFilter : ���͸��� ��� ���� ���������� ���� �� ���� �̸� ����Ʈ
	// (in) workingDirectoryPath : ûũ ���ϵ��� ������ ����, Ȥ�� root directory ���� ��� ���丮 ���
	// return : ���� ����
	bool SetUpFromOriginalDirectory
		(const MkPathName& absolutePathOfBaseDirectory,
		const MkArray<MkPathName>& nameFilter, const MkArray<MkPathName>& extensionFilter,
		const MkArray<MkStr>& prefixFilter, const MkArray<MkPathName>& exceptionFilter,
		const MkPathName& workingDirectoryPath);

	// ��Ű¡ �� ûũ ���ϵ�κ��� ����
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

	// �ش� ��θ��� ���� ������ ��Ű¡ �� ���翩�� ��ȯ
	// (in) filePath : ���� ���. ����θ� �״��, �����θ� �� root directory ���� ����η� ��ȯ
	// return : ���翩��
	bool CheckAvailable(const MkPathName& filePath) const;

	// �ش� ��θ��� ���� ������ ��Ű¡ �� ���� ��ȯ
	// (in) filePath : ���� ���. ����θ� �״��, �����θ� �� root directory ���� ����η� ��ȯ
	// (out) originalSize : ������ ���� ũ��
	// (out) dataSize : ������ ���� ũ��. originalSize�� �����ϸ� ������� ���� �������� �ǹ�
	// (out) writtenTime : ������ ���� �Ͻ�
	// return : ���翩��
	bool GetFileInfo(const MkPathName& filePath, unsigned int& originalSize, unsigned int& dataSize, unsigned int& writtenTime) const;

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

	// MkDataNode�� ������ ���� ���� ���. ����� �� Ž�������� ��� ����
	// ������ ���, ���ϸ��� "Files"��� key�� array�� ���� ��
	// (out) node : ��� �� MkDataNode
	void ExportFileStructure(MkDataNode& node) const;

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// ��ŷ �ý��ۿ��� ���� ����
	// ���� ûũ ���Ͽ��� ���ŵ����� �ʰ� ���¸� disable�� ����
	// (in) filePath : ���� ���. ����θ� �״��, �����θ� �� root directory ���� ����η� ��ȯ
	// return : ���� ����
	bool RemoveFile(const MkPathName& filePath);

	// ��ŷ �ý��ۿ� �־��� ���丮 ������ ��� ���� ����(���丮 ������� ���� ��)
	// �������� �ʴ� �����̸� �߰�. �̹� �����ϴ� ������ ��� ���� ũ�Ⱑ 0�� ��� ����, �׷��� ������ ����
	// �ټ��� ������ �Ѳ����� �����ϰ��� �� �� ȿ����
	// (in) absolutePathOfBaseDirectory : ���� �� ���� ���丮 ������. ��ΰ� �������� ������ ���� ������ ���ٰ� ����
	// return : ���� ����
	bool UpdateOriginalDirectory(const MkPathName& absolutePathOfBaseDirectory);

	// ûũ ����ȭ
	// RemoveFile(), UpdateOriginalDirectory()�� ȣ��� ûũ�� �� ������ ���� ���� �̻� ������ ��� ���� ����ȭ ����
	// (in) percentageForOptimizing : ûũ������ �� ���� ������ �־��� �ۼ�Ʈ ���� �̻��� ��� ����ȭ ����
	// return : ���� ����
	// ex> percentageForOptimizing == 30%, chunk_0(25%), chunk_1(34%), chunk_2(30%) �� ��� chunk_1, chunk_2 ����ȭ ����
	bool OptimizeChunks(unsigned int percentageForOptimizing);

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

	void _BuildStructureAndCountFiles(const MkHashStr& key, MkDataNode& node) const;
	void _BuildStructureAndCountFiles(const MkArray<MkStr>& token, unsigned int currIndex, unsigned int indexOfFile, MkDataNode* node) const;

	void _FillFilesToNode(const MkHashStr& key, MkDataNode& node) const;
	void _FillFilesToNode(const MkArray<MkStr>& token, unsigned int currIndex, unsigned int indexOfFile, MkDataNode* node) const;

protected:

	// ���� ����
	unsigned int m_ChunkSizeGuideline;
	unsigned int m_PercentageForCompressing;
	MkStr m_ChunkFilePrefix;
	MkStr m_ChunkFileExtension;

	// ������ ûũ ������ �����ϴ� ���� ���丮 ���
	MkPathName m_AbsoluteWorkingDirectoryPath;

	// ûũ ���̺�. ûũ �ϳ��� ûũ ���� �ϳ��� ��ġ��
	MkMap<unsigned int, MkFileChunk> m_ChunkTable;

	// �˻� ���̺�. hash map(�ӵ� �켱)
	// ��Ȱ�� ���(MkFileBlock::eDisable)�� ���Ե��� ����
	MkHashMap<MkHashStr, FileBlockIndex> m_SearchTable;
};

//------------------------------------------------------------------------------------------------//

#endif
