
#include "zlib/zlib.h"
#include "MkCore_MkZipCompressor.h"

#pragma comment(lib, "zdll")


//------------------------------------------------------------------------------------------------//

unsigned int MkZipCompressor::Compress
(const unsigned char* srcArray, unsigned int srcSize, MkByteArray& destBuffer, int level)
{
	if ((srcArray == 0) || (srcSize == 0))
		return 0;

	uLongf lfSrcSize = static_cast<uLongf>(srcSize);
	uLongf bonusSize = lfSrcSize / 100;
	if (bonusSize < 1024) bonusSize = 1024;
	uLongf destSize = lfSrcSize + bonusSize;  // ������ �뷮�� �ö󰡴� ��쵵 �����Ƿ� 1% Ȥ�� 1kbyte ���� Ȯ��

	unsigned char* destArray = new unsigned char[destSize];
	int rlt = compress2(destArray, &destSize, srcArray, lfSrcSize, level);
	unsigned int newSize;
	if (rlt == Z_OK) // Z_MEM_ERROR, Z_BUF_ERROR
	{
		newSize = static_cast<unsigned int>(destSize);
		destBuffer.Clear();
		destBuffer.Fill(newSize);
		destBuffer.Overwrite(0, MkByteArrayDescriptor(destArray, newSize));
	}
	else
	{
		newSize = 0;
	}
	delete [] destArray;
	return newSize;
}

unsigned int MkZipCompressor::Uncompress
(const unsigned char* srcArray, unsigned int srcSize, MkByteArray& destBuffer, unsigned int destBufRate)
{
	if ((srcArray == 0) || (srcSize == 0))
		return 0;

	// compressedSize�� destBufRate���ϰų� unsigned int�� �ִ� ���� ���ۻ������ ����
	bool onMaxBufferSize = (srcSize >= (0xffffffff / destBufRate));
	unsigned int bufSize = (onMaxBufferSize) ? 0xffffffff : (srcSize * destBufRate);
	uLongf destSize = static_cast<uLongf>(bufSize);

	unsigned char* destArray = new unsigned char[destSize];
	int rlt = uncompress(destArray, &destSize, srcArray, srcSize);
	unsigned int newSize;
	if (rlt == Z_OK)
	{
		newSize = static_cast<unsigned int>(destSize);
		destBuffer.Clear();
		destBuffer.Fill(newSize);
		destBuffer.Overwrite(0, MkByteArrayDescriptor(destArray, newSize));
		delete [] destArray;
	}
	else if (rlt == Z_BUF_ERROR)
	{
		// ���� �뷮 ����
		delete [] destArray;
		if (onMaxBufferSize)
		{
			newSize = 0; // 4G ���۷ε� �ȵ�. ����
		}
		else
		{
			newSize = Uncompress(srcArray, srcSize, destBuffer, destBufRate * 2); // �ι�� ���۸� Ű�� ��õ�
		}
	}
	else
	{
		// Z_MEM_ERROR, Z_DATA_ERROR
		newSize = 0;
		delete [] destArray;
	}
	return newSize;
}

//------------------------------------------------------------------------------------------------//
