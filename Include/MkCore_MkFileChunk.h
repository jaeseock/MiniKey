#ifndef __MINIKEY_CORE_MKFILECHUNK_H__
#define __MINIKEY_CORE_MKFILECHUNK_H__


//------------------------------------------------------------------------------------------------//
// ��ŷ�� ûũ ���� ������
// ���� ����� ����
// �ϳ��� ûũ ���Ͽ� ���� ����
//
// chunk file������ ������,
// 0. tag : �Ҽӵ� ��� ��� �� (unsigned int)
// 1. block 0
// 2. block 1
// ...
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkGlobalFunction.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkFileBlock.h"


class MkFilePathListContainer;

class MkFileChunk
{
public:

	//------------------------------------------------------------------------------------------------//
	// �ʱ�ȭ
	//------------------------------------------------------------------------------------------------//

	// �������ϵ�κ��� ûũ ���� ����
	// chunkFilePath ��ο� ûũ������ �����ϰ� filePathListContainer�� �������� �о� ��� ���� �� ûũ���� ����
	// ���� ���� �� ûũ ���� ũ�Ⱑ chunkSizeGuideline�� �Ѿ�ų� ���� ����Ʈ�� ����Ǿ��� ���
	// ������ ���� ���� ����Ʈ���� filePathListContainer���� ������ �� ����
	//
	// (in) chunkFilePath : ûũ ������ ����, Ȥ�� root directory ���� ��� ��θ�
	// (in/out) filePathListContainer : ���� ���� ������/����� ����Ʈ. �Ҹ�� ������ ûũ ���� �Ϸ�� �����Ǿ� ��ȯ��
	// (out) memberFilePathList : ûũ�� ��ϵ�(relativeFilePathList���� �Ҹ��) ���� ����Ʈ
	// (in) chunkSizeGuideline : chunk file �Ѱ� ũ��
	// (in) percentageForCompressing : ���� ���
	// (in) createNewChunk : �� ûũ�� �������� ����. true�� ���� ûũ ����/���� ���� �� ���� �����
	//                       false�� ���� ��ϵ� �ڿ� �߰�
	// return : ���� ����
	bool AttachOriginalFiles
		(const MkPathName& chunkFilePath, MkFilePathListContainer& filePathListContainer, MkArray<MkPathName>& memberFilePathList,
		unsigned int chunkSizeGuideline, unsigned int percentageForCompressing, bool createNewChunk = true);

	// ûũ ���Ϸκ��� ����
	// (in) chunkFilePath : ûũ ������ ����, Ȥ�� root directory ���� ��� ��θ�
	// (out) memberFilePathList : ûũ�� ��ϵ� ���� ����Ʈ. disable ������ ��� �����ڿ��� ���
	// return : ���� ����
	bool SetUpFromChunkFile(const MkPathName& chunkFilePath, MkArray<MkPathName>& memberFilePathList);

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// ûũ ���� ũ�� ��ȯ
	inline unsigned int GetChunkSize(void) const { return m_ChunkSize; }

	// ��� �Ұ����� ��� ũ���� �� ��ȯ
	inline unsigned int GetBlankSize(void) const { return m_BlankSize; }

	// ��ü ûũ ���� �� ��� �Ұ����� ũ�⸦ �ۼ�Ʈ�� ��ȯ
	inline unsigned int GetBlankPercentage(void) const { return ConvertToPercentage<unsigned int, unsigned int>(m_BlankSize, m_ChunkSize); }

	// ��ϵ� ��� �� ��ȯ
	inline unsigned int GetBlockCount(void) const { return m_BlockList.GetSize(); }

	// ûũ ���� ��� ��ȯ
	inline const MkPathName& GetAbsoluteChunkFilePath(void) const { return m_AbsoluteChunkFilePath; }

	// ��� ���� ��ȯ
	// ������ ���� ����(ũ�� �� �����Ͻ�)�� ��ȯ
	bool GetBlockInfo(unsigned int blockIndex, unsigned int& originalSize, unsigned int& dataSize, unsigned int& writtenTime) const;

	// ��� ���� ��ȯ
	// data node�� ��� �� ���ϰ� �����丮 ������ ��� ��ȯ
	// includeChunkInfo�� true�� ��� ���ϸ��� �Ҽ� ûũ���ϸ�� block index�� �߰� 
	bool GetBlockInfo(unsigned int blockIndex, MkDataNode& directoryNode, bool includeChunkInfo) const;

	// blockIndex�� ����� ������ ������ ���� ���� �����͸� buffer�� ��� ��ȯ
	// (in) blockIndex : ��� ��ȣ
	// (out) buffer : �����Ͱ� ��� ����
	// return : ��������
	bool GetOriginalFileData(unsigned int blockIndex, MkByteArray& buffer) const;

	// ûũ�� ���� ��ȿ ������ ���� ���Ϸ� ����
	// (in) blockIndex : ��� ��ȣ
	// (in) destinationDirectoryPath : ������ ����, Ȥ�� root directory ���� ��� ���丮 ���
	// return : ���� ����
	bool ExtractAvailableFile(unsigned int blockIndex, const MkPathName& destinationDirectoryPath) const;

	// ûũ�� ���� ��� ��ȿ ���ϵ��� ���� ���Ϸ� ����
	// (in) destinationDirectoryPath : ������ ����, Ȥ�� root directory ���� ��� ���丮 ���
	// return : ���� ����
	bool ExtractAllAvailableFiles(const MkPathName& destinationDirectoryPath) const;

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// blockIndex�� ��� ����. disable ���·� ����
	// ���� ���Ͽ��� �������� ���� �ƴϰ� m_BlockList���� ���� ����
	// return : ���� ����
	bool RemoveFile(unsigned int blockIndex);

	// ��ȿ�� ��ϸ� ���� ûũ ���� �籸��
	// ��� ����� disable�� ��� ûũ ���� ����
	// �ε����� ����� ��� ������ ��ȯ
	// (out) movedPathList : ���� ��� �����
	// (out) newIndexList : ���� ��� �ε���
	// return : ���� ����
	bool OptimizeChunk(MkArray<MkPathName>& movedPathList, MkArray<unsigned int>& newIndexList);

	// ����
	void Clear(void);

	MkFileChunk() { Clear(); }
	~MkFileChunk() { Clear(); }

	//------------------------------------------------------------------------------------------------//

protected:

	void _AddCountTag(MkInterfaceForFileWriting& fwInterface, unsigned int count) const;

	bool _CheckAvailableBlock(unsigned int blockIndex) const;

	bool _RebuildChunk(MkArray<MkPathName>& movedPathList, MkArray<unsigned int>& newIndexList);

	//------------------------------------------------------------------------------------------------//

protected:

	MkPathName m_AbsoluteChunkFilePath; // ûũ ���� ������

	MkArray<MkFileBlock> m_BlockList;

	unsigned int m_ChunkSize; // ûũ ���� ũ��
	unsigned int m_BlankSize; // ��� �Ұ����� ��� ũ���� ��

	// for optimizing
	unsigned int m_AvailableBlockCount; // ��ȿ�� ��� ��
	unsigned int m_FirstDisableBlockIndex; // ���� disable block index
};

//------------------------------------------------------------------------------------------------//

#endif
