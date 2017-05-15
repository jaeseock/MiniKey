#ifndef __MINIKEY_CORE_MKFILEBLOCK_H__
#define __MINIKEY_CORE_MKFILEBLOCK_H__


//------------------------------------------------------------------------------------------------//
// ��ŷ�� ���� ���� ������
//
// chunk file������ ��� �κ� ������,
// 0. tag : m_BlockState (unsigned int)
// 1. tag : m_BlockSize (unsigned int) -> sizeof(0 + 1 + 2) + m_HeaderSize + m_DataSize
// 2. tag : m_HeaderSize (unsigned int) -> sizeof(3 + 4 + 5 + 6)
// 3. header : m_RelativeFilePath (MkStr)
// 4. header : m_UncompressedSize (unsigned int)
// 5. header : m_WrittenTime (unsigned int)
// 6. header : m_DataSize (unsigned int)
// 7. data (MkByteArray)
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStr.h"

class MkPathName;
class MkInterfaceForFileWriting;
class MkInterfaceForFileReading;

class MkFileBlock
{
public:

	enum eFileBlockState
	{
		eDisable = 0, // ��� �Ұ�
		eUncompressed, // ��� ����, ����� ����
		eCompressed // ��� ����, ���� ����
	};

public:

	// ���� ������ �����͸� �о�鿩 ��� ����� ������ �� ���࿩�θ� �Ǻ��� ����� ����/����� �� �����͸�
	// chunkFileInterface�� ���� chunk file �ڿ� ���
	//
	// �����(���� �� ũ��/���� ũ��)�� ���� �ʴٸ� ���� ������ ������ �����ϱ⶧���� ���� ��� ������ ��츸 ������ ����
	// 0�̸� ���� ������ ���� �ʰ� 100 �̻��̸� ��κ� ����(jpgó�� �̹� ����� ������ ��� ������ϸ� ������ �뷮�� �ణ �þ �� ����)
	// ex> (percentageForCompressing == 70), 1000ũ���� ������ ������ 700 ���ϸ� ����, �ƴϸ� ��������
	//
	// (in) absoluteOriginalFilePath : �߰��� ���� ������ ���� ���
	// (in) relativeFilePath : ���� �ý��ۻ󿡼� ���� ������ ��� ���� ���
	// (in) chunkFileInterface : chunk file�� ���µ� ���� interface
	// (in) percentageForCompressing : ���� ���
	// return : ��� ũ��(��ϵ� ũ��==������ ���� ũ��==m_BlockSize) ��ȯ
	//          ��ŷ�� ������ �߻��ϰų� ���� ���� ũ�Ⱑ 0�̸� 0 ��ȯ
	// (NOTE) chunkFileInterface ��ȿ�� �˻� ���� ����
	unsigned int SetUpFromOriginalFileAndAttachToChunkFile
		(const MkPathName& absoluteOriginalFilePath, const MkPathName& relativeFilePath,
		MkInterfaceForFileWriting& chunkFileInterface, unsigned int percentageForCompressing);

	// ûũ ���Ϸκ��� ��� ����
	// (in) chunkFileInterface : ûũ������ ���µǾ� ��� ���� ��ġ�� ����Ű�� �ִ� file reading interface
	// return : ���� ����
	// (NOTE) chunkFileInterface ��ȿ�� �˻� ���� ����
	bool SetUpFromChunkFile(MkInterfaceForFileReading& chunkFileInterface);

	// �� ����� newChunkFileInterface�� ���� �� ûũ ���Ͽ� ���
	// ��� ������ �� ûũ ���� �������� ���߾���
	// (in) currentChunkFileInterface : �����͸� �о���� �� ûũ ���� interface
	// (in) newChunkFileInterface : ����� �߰� �� �� ûũ ���� interface
	// (NOTE) currentChunkFileInterface, newChunkFileInterface ��ȿ�� �˻� ���� ����
	bool CopyBlockToNewChunk(MkInterfaceForFileReading& currentChunkFileInterface, MkInterfaceForFileWriting& newChunkFileInterface);

	// ����
	void Clear(void);

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	// ������ ������ ���� ���� ������ ��ȯ
	// (NOTE) chunkFileInterface ��ȿ�� �˻� ���� ����
	bool GetOriginalFileData(MkInterfaceForFileReading& chunkFileInterface, MkByteArray& buffer) const;

	// ��� ���� ��ȯ
	inline eFileBlockState GetBlockState(void) const { return m_BlockState; }

	// ��� ��뿩�� ��ȯ
	inline bool Available(void) const { return (m_BlockState != eDisable); }

	// ��� ũ�� ��ȯ
	inline unsigned int GetBlockSize(void) const { return m_BlockSize; }

	// ��� ũ�� ��ȯ
	inline unsigned int GetHeaderSize(void) const { return m_HeaderSize; }

	// ��� ������ ��� ��� ��ȯ
	inline const MkStr& GetRelativeFilePath(void) const { return m_RelativeFilePath; }

	// 2000�� ���� �ʴ����� ȯ��� ���� ���� �����Ͻ� ��ȯ
	inline unsigned int GetWrittenTime(void) const { return m_WrittenTime; }

	// �� ������ ũ�� ��ȯ
	inline unsigned int GetDataSize(void) const { return m_DataSize; }

	// ���� �� ������ ũ�� ��ȯ
	inline unsigned int GetUncompressedSize(void) const { return m_UncompressedSize; }

	// ����� ��ȯ(������)
	inline unsigned int GetCompressionEfficiency(void) const { return m_CompressionEfficiency; }

	// ũ��� �����Ͻø� ���� �ٸ� ���θ� ��ȯ
	inline bool CheckDifference(unsigned int uncompressedSize, unsigned int writtenTime) const
	{
		return ((uncompressedSize != m_UncompressedSize) || (writtenTime != m_WrittenTime));
	}

	//------------------------------------------------------------------------------------------------//
	// ����
	//------------------------------------------------------------------------------------------------//

	bool SetBlockState(const MkPathName& absoluteChunkFilePath, eFileBlockState newState);

	MkFileBlock() { Clear(); }
	~MkFileBlock() {}

	//------------------------------------------------------------------------------------------------//

protected:

	// ûũ ���Ͽ� tag, header, data block�� ���
	bool _WriteBlockToChunk(MkInterfaceForFileWriting& chunkFileInterface,
		const MkByteArray& tagBlock, const MkByteArray& headerBlock, const MkByteArray& dataBlock);

	void _SetTagBlock(MkByteArray& block) const;
	unsigned int _SetHeaderBlock(MkByteArray& block) const;

	//------------------------------------------------------------------------------------------------//

protected:

	eFileBlockState m_BlockState; // ��� ����
	unsigned int m_BlockSize; // ��� ũ��
	unsigned int m_HeaderSize; // �Ʒ� ����� ������ ��� ũ��

	MkStr m_RelativeFilePath; // ��� ������ ��� ���
	unsigned int m_UncompressedSize; // ���� ������ ũ��
	unsigned int m_WrittenTime; // ���� ���� �����Ͻø� 2000�� ���� �ʴ����� ȯ���� ��
	unsigned int m_DataSize; // ��ŷ�� �� ������ ũ��

	unsigned int m_CompressionEfficiency; // �����
	unsigned int m_PositionOnChunk;
};

//------------------------------------------------------------------------------------------------//

#endif
