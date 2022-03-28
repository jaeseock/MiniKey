#pragma once

//------------------------------------------------------------------------------------------------//
// exe���� �ڿ� �����͸� ���� ��Ÿ�Ӷ� ���
//------------------------------------------------------------------------------------------------//


#include "MkCore_MkArray.h"


class MkPathName;

class MkTailData
{
public:

	// exe ���� �Ĺ̿� �����͸� ����
	// (in) exeFilePath : data�� ���� exe ������ ����, Ȥ�� root��� ��� ���
	// (in) data/dataFilePath : exe���� �ڿ� ���� ���� ������, Ȥ�� ������ ����, Ȥ�� root��� ��� ���
	// (in) resultFilePath : ��ģ ��� ������ ����, Ȥ�� root��� ��� ���
	// ex>
	//	bool ok = MkTailData::AttachData(L"exe.exe", L"data.mmd", L"newexe.exe");
	static bool AttachData(const MkPathName& exeFilePath, const MkByteArray& data, const MkPathName& resultFilePath);
	static bool AttachData(const MkPathName& exeFilePath, const MkPathName& dataFilePath, const MkPathName& resultFilePath);

	// exe ���Ͽ��� AttachData()�� data�� ������ �����͸� �о� ��ȯ
	// (in) exeFilePath : exe ������ ����, Ȥ�� root��� ��� ���
	// ex>
	//	MkByteArray tailDataBuffer;
	//	if (MkTailData::GetTailData(L"newexe.exe", tailDataBuffer))
	//	{
	//		MkDataNode tailDataNode;
	//		tailDataNode.Load(tailDataBuffer);
	//	}
	static bool GetTailData(const MkPathName& exeFilePath, MkByteArray& buffer);

	// ����� �ڽ��� exe ���Ͽ��� AttachData()�� data�� ������ �����͸� �о� ��ȯ
	static bool GetTailData(MkByteArray& buffer);

private:

	static bool _CheckTailDataTag(const MkByteArray& src, unsigned int pos);
};
