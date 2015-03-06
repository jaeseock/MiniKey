
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>
#include <string.h>
#include <fstream>
#include <string>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkVec3.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDevPanel.h"


//------------------------------------------------------------------------------------------------//

const static int SIZE_COUNT = 10;


// TestPage 선언
class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		// [ data 형태 정의 ]
		//------------------------------------------------------------------------------------------------//
		// int a;
		// float b;
		// MkVec3 c;
		// wchar_t d[64];
		//------------------------------------------------------------------------------------------------//

		// data 크기 계산
		const unsigned int sizeOfUnit =
			sizeof(int) // a
			+ sizeof(float) // b
			+ sizeof(MkVec3) // c
			+ sizeof(wchar_t) * 64; // d

		const unsigned int sizeOfData = sizeOfUnit * SIZE_COUNT;

		// chunk data 생성
		unsigned char* chunkData = new unsigned char[sizeOfData];
		ZeroMemory(chunkData, sizeOfData);

		unsigned int recordPosition = 0;
		for (int i=0; i<SIZE_COUNT; ++i)
		{
			float f = static_cast<float>(i);

			// a
			const int a = i;
			memcpy_s(&chunkData[recordPosition], sizeof(int), reinterpret_cast<const unsigned char*>(&a), sizeof(int));
			recordPosition += sizeof(int);

			// b
			const float b = f * -10.f;
			memcpy_s(&chunkData[recordPosition], sizeof(float), reinterpret_cast<const unsigned char*>(&b), sizeof(float));
			recordPosition += sizeof(float);

			// c
			const MkVec3 c = MkVec3(f, f * -2.f, f * 3.f);
			memcpy_s(&chunkData[recordPosition], sizeof(MkVec3), reinterpret_cast<const unsigned char*>(&c), sizeof(MkVec3));
			recordPosition += sizeof(MkVec3);

			// d
			wcscpy_s(reinterpret_cast<wchar_t*>(&chunkData[recordPosition]), 64, L"잘생겨서 죄송요~");
			recordPosition += sizeof(wchar_t) * 64;
		}

		// chunkData -> 파일로 출력
		std::ofstream ostream;
		ostream.open(L"test.data", std::ios::out | std::ios::binary | std::ios::trunc);
		if (ostream.is_open())
		{
			ostream.write(reinterpret_cast<const char*>(chunkData), sizeOfData);
			ostream.close();
		}

		delete [] chunkData;

		// chunk data 버퍼 생성 및 초기화
		unsigned char* chunkBuffer = new unsigned char[sizeOfData];
		ZeroMemory(chunkBuffer, sizeOfData);

		// 파일에서 읽기 -> chunkBuffer
		std::ifstream rstream;
		rstream.open(L"test.data", std::ios::in | std::ios::binary);
		if (rstream.is_open())
		{
			rstream.read(reinterpret_cast<char*>(chunkBuffer), sizeOfData);
			rstream.close();
		}

		MK_DEV_PANEL.MsgToLog(MkStr::EMPTY); // 빈 칸 하나 넣고,

		// 읽어들인 결과 출력. 첫번째와 마지막 unit 대상
		unsigned int readPosition = 0;

		// 첫번째 unit
		int a = 0;
		memcpy_s(reinterpret_cast<unsigned char*>(&a), sizeof(int), &chunkBuffer[readPosition], sizeof(int));
		readPosition += sizeof(int);

		float b = 0.f;
		memcpy_s(reinterpret_cast<unsigned char*>(&b), sizeof(float), &chunkBuffer[readPosition], sizeof(float));
		readPosition += sizeof(float);

		MkVec3 c = MkVec3::Zero;
		memcpy_s(reinterpret_cast<unsigned char*>(&c), sizeof(MkVec3), &chunkBuffer[readPosition], sizeof(MkVec3));
		readPosition += sizeof(MkVec3);

		wchar_t d[64] = {0, };
		memcpy_s(reinterpret_cast<unsigned char*>(d), sizeof(wchar_t) * 64, &chunkBuffer[readPosition], sizeof(wchar_t) * 64);

		MK_DEV_PANEL.MsgToLog(L"[ First unit ]");
		MK_DEV_PANEL.MsgToLog(L"   a : " + MkStr(a));
		MK_DEV_PANEL.MsgToLog(L"   b : " + MkStr(b));
		MK_DEV_PANEL.MsgToLog(L"   c : " + MkStr(c));
		MK_DEV_PANEL.MsgToLog(L"   d : " + MkStr(d));

		// 마지막 unit
		readPosition = sizeOfUnit * (SIZE_COUNT - 1);

		a = 0;
		memcpy_s(reinterpret_cast<unsigned char*>(&a), sizeof(int), &chunkBuffer[readPosition], sizeof(int));
		readPosition += sizeof(int);

		b = 0.f;
		memcpy_s(reinterpret_cast<unsigned char*>(&b), sizeof(float), &chunkBuffer[readPosition], sizeof(float));
		readPosition += sizeof(float);

		c = MkVec3::Zero;
		memcpy_s(reinterpret_cast<unsigned char*>(&c), sizeof(MkVec3), &chunkBuffer[readPosition], sizeof(MkVec3));
		readPosition += sizeof(MkVec3);

		ZeroMemory(d, sizeof(wchar_t) * 64);
		memcpy_s(reinterpret_cast<unsigned char*>(d), sizeof(wchar_t) * 64, &chunkBuffer[readPosition], sizeof(wchar_t) * 64);

		MK_DEV_PANEL.MsgToLog(L"[ Last unit ]");
		MK_DEV_PANEL.MsgToLog(L"   a : " + MkStr(a));
		MK_DEV_PANEL.MsgToLog(L"   b : " + MkStr(b));
		MK_DEV_PANEL.MsgToLog(L"   c : " + MkStr(c));
		MK_DEV_PANEL.MsgToLog(L"   d : " + MkStr(d));

		delete [] chunkBuffer;

		// 출력 폴더 열기
		MkPathName targetDir = MkPathName::GetRootDirectory() + L"..\\..\\"; // project foler
		targetDir.OpenDirectoryInExplorer();

		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		
	}

	virtual void Clear(MkDataNode* sharingNode = NULL) {}

	TestPage(const MkHashStr& name) : MkBasePage(name) {}
	virtual ~TestPage() {}
};

// TestFramework 선언
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
	{
		MK_PAGE_MGR.SetUp(new TestPage(L"TestPage"));
		MK_PAGE_MGR.ChangePageDirectly(L"TestPage");

		return true;
	}

	virtual ~TestFramework() {}
};

// TestApplication 선언
class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

//------------------------------------------------------------------------------------------------//

// 엔트리 포인트에서의 TestApplication 실행
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"Standard memory operation", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

