
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>
#include <memory.h>
#include <vector>
#include <map>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkProfilingManager.h"


//------------------------------------------------------------------------------------------------//

const static size_t VECTOR_MEMBER_COUNT = 100000;
const static size_t MAP_MEMBER_COUNT = 100000;


// TestPage 선언
class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		MK_DEV_PANEL.MsgToFreeboard(0, L"vector 멤버 수 : " + MkStr(VECTOR_MEMBER_COUNT));
		MK_DEV_PANEL.MsgToFreeboard(1, L"map 멤버 수 : " + MkStr(MAP_MEMBER_COUNT));

		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		//------------------------------------------------------------------------------------------------//
		// std::vector
		//------------------------------------------------------------------------------------------------//

		std::vector<wchar_t> reservedVector;
		reservedVector.reserve(VECTOR_MEMBER_COUNT); // reserve

		std::vector<wchar_t> unreservedVector;

		bool equal;
		wchar_t buf;

		// push_back()
		{
			MkFocusProfiler fp(L"0. reserved vector<wchar_t>.push_back()");

			for (int i=0; i<VECTOR_MEMBER_COUNT; ++i)
			{
				reservedVector.push_back(L'A'); // excute!!!
			}
		}

		{
			MkFocusProfiler fp(L"1. unreserved vector<wchar_t>.push_back()");

			for (int i=0; i<VECTOR_MEMBER_COUNT; ++i)
			{
				unreservedVector.push_back(L'A'); // excute!!!
			}
		}

		// equals
		{
			MkFocusProfiler fp(L"2. vector<wchar_t>.operator==");

			// bool __CLRCALL_OR_CDECL _Equal(_InIt1 _First1, _InIt1 _Last1, _InIt2 _First2, _InItCats, _Range_checked_iterator_tag)
			// {	// compare [_First1, _Last1) to [First2, ...)
			//		for (; _First1 != _Last1; ++_First1, ++_First2)
			//		if (!(*_First1 == *_First2))
			//			return (false);
			//		return (true);
			// }
			equal = reservedVector == unreservedVector; // excute!!!
		}
		
		{
			MkFocusProfiler fp(L"3. memcmp(vector<wchar_t>)");

			equal = memcmp(&reservedVector[0], &unreservedVector[0], VECTOR_MEMBER_COUNT * sizeof(wchar_t)) == 0; // excute!!!
		}
		unreservedVector.clear();

		// operator[]
		{
			MkFocusProfiler fp(L"4. vector<wchar_t>.operator[]");

			for (int i=0; i<VECTOR_MEMBER_COUNT; ++i)
			{
				buf = reservedVector[i]; // excute!!!
			}
		}

		// clear
		{
			MkFocusProfiler fp(L"5. vector<wchar_t>.clear()");
			
			reservedVector.clear(); // excute!!!
		}

		//------------------------------------------------------------------------------------------------//
		// std::map
		//------------------------------------------------------------------------------------------------//

		std::map<int, wchar_t> sampleMap;

		// insert
		{
			MkFocusProfiler fp(L"6. map<int, wchar_t>.insert()");

			for (int i=0; i<MAP_MEMBER_COUNT; ++i)
			{
				sampleMap.insert(std::map<int, wchar_t>::value_type(i, L'A')); // excute!!!
			}
		}

		// operator[]
		{
			MkFocusProfiler fp(L"7. map<int, wchar_t>.operator[]");

			for (int i=0; i<MAP_MEMBER_COUNT; ++i)
			{
				buf = sampleMap[i]; // excute!!!
			}
		}

		// clear
		{
			MkFocusProfiler fp(L"8. map<int, wchar_t>.clear()");
			
			sampleMap.clear(); // excute!!!
		}
	}

	virtual void Clear(MkDataNode* sharingNode = NULL) {}

	TestPage(const MkHashStr& name) : MkBasePage(name) {}
	virtual ~TestPage() {}
};

// TestFramework 선언
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
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
	application.Run(hI, L"STL profiling", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 300, 100, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

