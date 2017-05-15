#ifndef __MINIKEY_CORE_MKZIPCOMPRESSOR_H__
#define __MINIKEY_CORE_MKZIPCOMPRESSOR_H__

//------------------------------------------------------------------------------------------------//
// zip 압축/해제용 인터페이스
//------------------------------------------------------------------------------------------------//
// ex>
//	MkInterfaceForFileReading frInterface;
//	MkInterfaceForFileWriting fwInterface;
//
//	// file -> srcArray -> compress -> file -> destBuffer -> uncompress -> file
//	for (unsigned int i=0; i<4; ++i)
//	{
//		// file -> srcArray
//		MkByteArray srcArray;
//		frInterface.SetUp(L"src_" + BsStr(i) + L".jpg");
//		frInterface.Read(srcArray, MkArraySection(0));
//		frInterface.Clear();
//
//		// srcArray -> compress
//		unsigned int srcSize = srcArray.GetSize();
//		MkByteArray destBuffer;
//		unsigned int compressedSize = MkZipCompressor::Compress(srcArray.GetPtr(), srcArray.GetSize(), destBuffer);
//		srcArray.Clear();
//
//		// compress -> file
//		fwInterface.SetUp(L"comp_" + BsStr(i) + L".bin", true, true);
//		fwInterface.Write(destBuffer, MkArraySection(0));
//		fwInterface.Clear();
//		destBuffer.Clear();
//
//		// file -> destBuffer
//		frInterface.SetUp(L"comp_" + BsStr(i) + L".bin");
//		frInterface.Read(srcArray, MkArraySection(0));
//		frInterface.Clear();
//
//		// destBuffer -> uncompress
//		unsigned int uncompressedSize = MkZipCompressor::Uncompress(srcArray.GetPtr(), srcArray.GetSize(), destBuffer);
//		srcArray.Clear();
//
//		// uncompress -> file
//		fwInterface.SetUp(L"uncomp_" + BsStr(i) + L".jpg", true, true);
//		fwInterface.Write(destBuffer, MkArraySection(0));
//		fwInterface.Clear();
//		destBuffer.Clear();
//	}
//------------------------------------------------------------------------------------------------//


#include "MkCore_MkArray.h"


class MkZipCompressor
{
public:

	// 압축 실행
	// srcArray : 압축할 원본 데이터
	// srcSize : 원본 데이터 크기
	// destBuffer : 압축된 데이터가 담길 버퍼
	// level : 압축강도(0 ~ 9). 9가 최대
	// 반환값은 원본 데이터의 압축 후 용량
	static unsigned int Compress(const unsigned char* srcArray, unsigned int srcSize, MkByteArray& destBuffer, int level = 9);

	// 압축 해제
	// srcArray : 압축된 데이터
	// srcSize : 압축된 데이터 크기
	// destBuffer : 압축이 풀린 데이터가 담길 버퍼
	// destBufRate : 압축해제중 사용될 중간 버퍼 크기를 결정하기 위한 compressedSize 배수
	// 반환값은 원본 데이터의 압축 해제 후 용량
	static unsigned int Uncompress(const unsigned char* srcArray, unsigned int srcSize, MkByteArray& destBuffer, unsigned int destBufRate = 8);
};

#endif
