
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkLogicThreadUnit.h"

#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

#include "MkCore_MkUniformDice.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkProfilingManager.h"


#pragma comment (lib, "Winmm.lib") // timeGetTime


//------------------------------------------------------------------------------------------------//
// multi-thread�� ��� �ּ� ó��
//------------------------------------------------------------------------------------------------//

#define SAMPLEDEF_SINGLE_THREAD

//------------------------------------------------------------------------------------------------//

// TestPage ����(����)
class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		// ���� �� ���̺���� pair ��
		const unsigned int sampleCount = 100000;
		MK_DEV_PANEL.MsgToFreeboard(0, L"���� �� ���̺���� pair �� : " + MkStr(sampleCount));

		// ���� ���ڿ��� �ּ� ����
		const unsigned int minLengthOfString = 20;
		
		// ���� ���ڿ��� �ִ� ����
		const unsigned int maxLengthOfString = 50;
		MK_DEV_PANEL.MsgToFreeboard(1, L"���� ���ڿ��� ���� : " + MkStr(minLengthOfString) + L" ~ " + MkStr(maxLengthOfString));

		m_SelectionDice.SetMinMax(0, sampleCount - 1);
		m_Keys.Reserve(sampleCount);

		// ���� ���̺� ����
		MkUniformDice lengthDice(minLengthOfString, maxLengthOfString);
		lengthDice.SetSeed(static_cast<unsigned int>(timeGetTime()));

		MkUniformDice charDice(32, 126);
		charDice.SetSeed(static_cast<unsigned int>(timeGetTime()) + 12345);

		wchar_t buffer[maxLengthOfString + 1];
		unsigned int bufSize = sizeof(wchar_t) * (maxLengthOfString + 1);
		unsigned int length;
		MkHashStr keyStr;

		for (unsigned int i=0; i<sampleCount; ++i)
		{
			ZeroMemory(buffer, bufSize);

			length = lengthDice.GenerateRandomNumber();
			for (unsigned int j=0; j<length; ++j)
			{
				buffer[j] = static_cast<wchar_t>(charDice.GenerateRandomNumber());
			}

			keyStr = buffer;

			// key list
			m_Keys.PushBack(keyStr);

			// MkMap & int
			m_MapAndInt.Create(i, i);

			// MkMap & MkStr
			m_MapAndStr.Create(keyStr.GetString(), i);

			// MkHashMap & int
			m_HashMapAndInt.Create(i, i);

			// MkHashMap & MkHashStr
			m_HashMapAndHashStr.Create(keyStr, i);
		}

		m_HashMapAndInt.Rehash();
		m_HashMapAndHashStr.Rehash();

		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		// �� �����ӿ� �˻� �� Ƚ��
		// ���������� sampleCount ũ���� ���̺��� findCount �� �˻� �� ����� �������ϸ� ��
		const unsigned int findCount = 1000;
		MK_DEV_PANEL.MsgToFreeboard(2, L"�˻� Ƚ�� : " + MkStr(findCount));

		MkArray<unsigned int> index(findCount);
		MkArray<MkHashStr> hashKey(findCount);
		MkArray<MkStr> strKey(findCount);

		unsigned int target;
		for (unsigned int i=0; i<findCount; ++i)
		{
			target = m_SelectionDice.GenerateRandomNumber();
			index.PushBack(target);
			hashKey.PushBack(m_Keys[target]);
			strKey.PushBack(m_Keys[target].GetString());
		}

		const static MkHashStr ProfKeyMI = L"MkMap<int>";
		const static MkHashStr ProfKeyMS = L"MkMap<MkStr>";
		const static MkHashStr ProfKeyHI = L"MkHashMap<int>";
		const static MkHashStr ProfKeyHS = L"MkHashMap<MkHashStr>";

		unsigned int tmp = 0;

		// MkMap & int
		MK_PROF_MGR.Begin(ProfKeyMI);
		MK_INDEXING_LOOP(index, i)
		{
			tmp = m_MapAndInt[index[i]];
		}
		MK_PROF_MGR.End(ProfKeyMI);

		// MkMap & MkStr
		MK_PROF_MGR.Begin(ProfKeyMS);
		MK_INDEXING_LOOP(strKey, i)
		{
			tmp = m_MapAndStr[strKey[i]];
		}
		MK_PROF_MGR.End(ProfKeyMS);

		// MkHashMap & int
		MK_PROF_MGR.Begin(ProfKeyHI);
		MK_INDEXING_LOOP(index, i)
		{
			tmp = m_HashMapAndInt[index[i]];
		}
		MK_PROF_MGR.End(ProfKeyHI);

		// MkHashMap & MkHashStr
		MK_PROF_MGR.Begin(ProfKeyHS);
		MK_INDEXING_LOOP(hashKey, i)
		{
			tmp = m_HashMapAndHashStr[hashKey[i]];
		}
		MK_PROF_MGR.End(ProfKeyHS);

		MK_DEV_PANEL.MsgToFreeboard(4, MkStr(tmp));
	}

	virtual void Clear(MkDataNode* sharingNode = NULL)
	{
		m_Keys.Clear();
		m_MapAndInt.Clear();
		m_MapAndStr.Clear();
		m_HashMapAndInt.Clear();
		m_HashMapAndHashStr.Clear();
	}

	TestPage(const MkHashStr& name) : MkBasePage(name) {}
	virtual ~TestPage() {}

protected:

	MkArray<MkHashStr> m_Keys;

	MkMap<int, int> m_MapAndInt;
	MkMap<MkStr, int> m_MapAndStr;
	MkHashMap<int, int> m_HashMapAndInt;
	MkHashMap<MkHashStr, int> m_HashMapAndHashStr;

	MkUniformDice m_SelectionDice;
};

// single-thread������ TestFramework ����
#ifdef SAMPLEDEF_SINGLE_THREAD

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

#else

// multi-thread������ TestThreadUnit ����
class TestThreadUnit : public MkLogicThreadUnit
{
public:

	virtual bool SetUp(void)
	{
		MK_PAGE_MGR.SetUp(new TestPage(L"TestPage"));
		MK_PAGE_MGR.ChangePageDirectly(L"TestPage");

		return MkLogicThreadUnit::SetUp();
	}

	TestThreadUnit(const MkHashStr& threadName) : MkLogicThreadUnit(threadName) {}
	virtual ~TestThreadUnit() {}
};

// multi-thread������ TestFramework ����
class TestFramework : public MkBaseFramework
{
public:
	virtual MkBaseThreadUnit* CreateLogicThreadUnit(const MkHashStr& threadName) const { return new TestThreadUnit(threadName); }
	virtual ~TestFramework() {}
};

#endif

// TestApplication ����(����)
class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

//------------------------------------------------------------------------------------------------//

// ��Ʈ�� ����Ʈ������ TestApplication ����
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"Hash map profiler", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

