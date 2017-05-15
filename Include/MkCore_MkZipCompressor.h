#ifndef __MINIKEY_CORE_MKZIPCOMPRESSOR_H__
#define __MINIKEY_CORE_MKZIPCOMPRESSOR_H__

//------------------------------------------------------------------------------------------------//
// zip ����/������ �������̽�
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

	// ���� ����
	// srcArray : ������ ���� ������
	// srcSize : ���� ������ ũ��
	// destBuffer : ����� �����Ͱ� ��� ����
	// level : ���భ��(0 ~ 9). 9�� �ִ�
	// ��ȯ���� ���� �������� ���� �� �뷮
	static unsigned int Compress(const unsigned char* srcArray, unsigned int srcSize, MkByteArray& destBuffer, int level = 9);

	// ���� ����
	// srcArray : ����� ������
	// srcSize : ����� ������ ũ��
	// destBuffer : ������ Ǯ�� �����Ͱ� ��� ����
	// destBufRate : ���������� ���� �߰� ���� ũ�⸦ �����ϱ� ���� compressedSize ���
	// ��ȯ���� ���� �������� ���� ���� �� �뷮
	static unsigned int Uncompress(const unsigned char* srcArray, unsigned int srcSize, MkByteArray& destBuffer, unsigned int destBufRate = 8);
};

#endif
