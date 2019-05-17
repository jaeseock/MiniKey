#pragma once

//------------------------------------------------------------------------------------------------//
// ��ó���� ������
// ex>
//	str #Target = "KR-DEV";
//
//	str #Services = "KR-DEV" / "KR-IDC" / "US" / "CN";
//	str #ExportTo = "KR-DEV.h" / "KR-IDC.h" / "US.h" / "CN.h";
//
//	Node "DEF_TEST_INDEX"
//	{
//		int #Default = 0;
//		int KR-DEV = 1;
//		int US = 2;
//	}
//
//	Node "DEF_TEST_SWITCH"
//	{
//		bool #Default = false;
//		bool KR-IDC = true;
//		bool CN = true;
//	}
//
//	Node "DEF_TEST_HEIGHT"
//	{
//		float KR-DEV = 180.4;
//		float KR-IDC = 200.1;
//		float US = 195.2;
//		float CN = 167;
//	}
//
//	Node "DEF_TEST_INT2"
//	{
//		int2 #Default = (1, 2);
//		int2 US = (-3, -4);
//	}
//
//	Node "DEF_TEST_STR"
//	{
//		str #Default = "default";
//		str KR-DEV = "�̳�";
//		str US = "�Դϴ�";
//		str CN = "�źδ� �Ұ�";
//	}
//
// -> [test.h]
//	
//	#pragma once
//
//	//...
//
//	#define DEF_TEST_SWITCH false
//	#define DEF_TEST_STR L"�Դϴ�"
//	#define DEF_TEST_HEIGHT 195.200
//	#define DEF_TEST_INT2 MkInt2(-3, -4)
//	#define DEF_TEST_INDEX 2
//------------------------------------------------------------------------------------------------//

#include "MkCore_MkStr.h"


class MkCodeDefinitionConverter
{
public:

	// filePathdml MkDataNode ������ �о� ��ȯ
	// target�� �����ϴ� ��� �켱 ������� ��� ������ ����� "#Target" �׸��� ���
	static bool Convert(const MkPathName& filePath, const MkStr& target = MkStr::EMPTY);
};
