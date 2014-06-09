
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
	uLongf destSize = lfSrcSize + bonusSize;  // 압축후 용량이 올라가는 경우도 있으므로 1% 혹은 1kbyte 여유 확보

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

	// compressedSize를 destBufRate배하거나 unsigned int의 최대 수를 버퍼사이즈로 잡음
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
		// 버퍼 용량 부족
		delete [] destArray;
		if (onMaxBufferSize)
		{
			newSize = 0; // 4G 버퍼로도 안됨. 실패
		}
		else
		{
			newSize = Uncompress(srcArray, srcSize, destBuffer, destBufRate * 2); // 두배로 버퍼를 키워 재시도
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
