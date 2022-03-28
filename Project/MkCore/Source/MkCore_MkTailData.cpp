
#include <string>
#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkTailData.h"

const static std::string TailDataTag = "MkTailData";
const static unsigned int HeaderSize = TailDataTag.size() + sizeof(unsigned int);

//------------------------------------------------------------------------------------------------//

bool MkTailData::AttachData(const MkPathName& exeFilePath, const MkByteArray& data, const MkPathName& resultFilePath)
{
	if (exeFilePath.IsDirectoryPath())
		return false;

	MkPathName exeFullPath;
	exeFullPath.ConvertToRootBasisAbsolutePath(exeFilePath);
	if (!exeFullPath.CheckAvailable())
		return false;

	MkByteArray exeBuffer;
	MkInterfaceForFileReading frInterface;
	if (!frInterface.SetUp(exeFullPath))
		return false;

	frInterface.Read(exeBuffer, MkArraySection(0));
	frInterface.Clear();

	exeBuffer.Reserve(exeBuffer.GetSize() + data.GetSize() + HeaderSize);
	exeBuffer += data;

	for (int i = 0; i < TailDataTag.size(); ++i)
	{
		MkByteArrayHelper<unsigned char>::PushBack(exeBuffer, TailDataTag[i]);
	}
	MkByteArrayHelper<unsigned int>::PushBack(exeBuffer, data.GetSize());

	MkPathName resultFullPath;
	resultFullPath.ConvertToRootBasisAbsolutePath(resultFilePath);

	MkInterfaceForFileWriting fwInterface;
	if (!fwInterface.SetUp(resultFullPath, true, true))
		return false;

	if (fwInterface.Write(exeBuffer, MkArraySection(0)) != exeBuffer.GetSize())
		return false;

	fwInterface.Clear();
	return true;
}

bool MkTailData::AttachData(const MkPathName& exeFilePath, const MkPathName& dataFilePath, const MkPathName& resultFilePath)
{
	MkPathName dataFullPath;
	dataFullPath.ConvertToRootBasisAbsolutePath(dataFilePath);
	if (!dataFullPath.CheckAvailable())
		return false;

	MkByteArray dataBuffer;
	MkInterfaceForFileReading frInterface;
	if (!frInterface.SetUp(dataFullPath))
		return false;

	frInterface.Read(dataBuffer, MkArraySection(0));
	frInterface.Clear();

	return AttachData(exeFilePath, dataBuffer, resultFilePath);
}

bool MkTailData::GetTailData(const MkPathName& exeFilePath, MkByteArray& buffer)
{
	buffer.Clear();

	MkInterfaceForFileReading frInterface;
	if (!frInterface.SetUp(exeFilePath))
		return false;

	if (frInterface.GetFileSize() < HeaderSize)
		return false;

	MkByteArray exeBuffer;
	frInterface.Read(exeBuffer, MkArraySection(0));
	frInterface.Clear();

	unsigned int dataSize = 0xffffffff;
	unsigned int dataPos = 0;

	for (unsigned int i = exeBuffer.GetSize() - HeaderSize; i != 0xffffffff; --i)
	{
		if (_CheckTailDataTag(exeBuffer, i))
		{
			dataSize = MkByteArrayHelper<unsigned int>::GetValue(exeBuffer, i + TailDataTag.size());
			dataPos = i - dataSize;
			break;
		}
	}

	if (dataSize == 0xffffffff)
		return false;

	if (dataSize > 0)
	{
		if (!exeBuffer.GetSubArray(MkArraySection(dataPos, dataSize), buffer))
			return false;
	}
	return true;
}

bool MkTailData::GetTailData(MkByteArray& buffer)
{
	wchar_t modulePath[MAX_PATH];
	::GetModuleFileName(NULL, modulePath, MAX_PATH);

	return GetTailData(MkPathName(modulePath), buffer);
}

bool MkTailData::_CheckTailDataTag(const MkByteArray& src, unsigned int pos)
{
	unsigned int hitCount = 0;
	for (unsigned int i = 0; i < TailDataTag.size(); ++i)
	{
		if (src[pos + i] != TailDataTag[i])
			break;

		++hitCount;
	}

	return (hitCount == TailDataTag.size());
}

//------------------------------------------------------------------------------------------------//
