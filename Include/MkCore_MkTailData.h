#pragma once

//------------------------------------------------------------------------------------------------//
// exe파일 뒤에 데이터를 붙혀 런타임때 사용
//------------------------------------------------------------------------------------------------//


#include "MkCore_MkArray.h"


class MkPathName;

class MkTailData
{
public:

	// exe 파일 후미에 데이터를 붙힘
	// (in) exeFilePath : data를 붙힐 exe 파일의 절대, 혹은 root기반 상대 경로
	// (in) data/dataFilePath : exe파일 뒤에 붙힐 실제 데이터, 혹은 파일의 절대, 혹은 root기반 상대 경로
	// (in) resultFilePath : 합친 결과 파일의 절대, 혹은 root기반 상대 경로
	// ex>
	//	bool ok = MkTailData::AttachData(L"exe.exe", L"data.mmd", L"newexe.exe");
	static bool AttachData(const MkPathName& exeFilePath, const MkByteArray& data, const MkPathName& resultFilePath);
	static bool AttachData(const MkPathName& exeFilePath, const MkPathName& dataFilePath, const MkPathName& resultFilePath);

	// exe 파일에서 AttachData()로 data를 붙혀진 데이터를 읽어 반환
	// (in) exeFilePath : exe 파일의 절대, 혹은 root기반 상대 경로
	// ex>
	//	MkByteArray tailDataBuffer;
	//	if (MkTailData::GetTailData(L"newexe.exe", tailDataBuffer))
	//	{
	//		MkDataNode tailDataNode;
	//		tailDataNode.Load(tailDataBuffer);
	//	}
	static bool GetTailData(const MkPathName& exeFilePath, MkByteArray& buffer);

	// 실행된 자신의 exe 파일에서 AttachData()로 data를 붙혀진 데이터를 읽어 반환
	static bool GetTailData(MkByteArray& buffer);

private:

	static bool _CheckTailDataTag(const MkByteArray& src, unsigned int pos);
};
