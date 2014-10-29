
//------------------------------------------------------------------------------------------------//
// single/multi-thread application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkLogicThreadUnit.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

// for sample
#include "MkCore_MkCheck.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkCheatMessage.h"
#include "MkCore_MkPathName.h"
#include "MkCore_MkDataNode.h"

//------------------------------------------------------------------------------------------------//
// multi-thread의 경우 주석 처리
//------------------------------------------------------------------------------------------------//

//#define SAMPLEDEF_SINGLE_THREAD

//------------------------------------------------------------------------------------------------//

MkDataNode gDataNode;
MkDataNode* gTargetNode = NULL;


// 자료형에 따른 값 읽기용 템플릿 인터페이스
template <class DataType>
class __SampleUnit
{
public:
	
	static void GetSingleValue(const MkHashStr& key, MkStr& resultMsg, unsigned int index)
	{
		DataType value;
		gTargetNode->GetData(key, value, index);
		resultMsg += MkStr(value);
	}

	static void GetInfo(const MkHashStr& key, MkStr& resultMsg)
	{
		unsigned int dataSize = gTargetNode->GetDataSize(key);
		ePrimitiveDataType type = gTargetNode->GetUnitType(key);

		resultMsg = MkPrimitiveDataType::GetTag(type) + L" 타입. ";

		if (type != ePDT_Undefined)
		{
			if (dataSize == 1)
			{
				resultMsg += L"값은 하나만 존재 : ";
				GetSingleValue(key, resultMsg, 0);
			}
			else // dataSize > 1
			{
				resultMsg += MkStr(dataSize) + L" 크기의 배열 :" + MkStr::CR;

				for (unsigned int i=0; i<dataSize; ++i)
				{
					GetSingleValue(key, resultMsg, i);
					resultMsg += MkStr::CR;
				}
			}
		}
	}
};


// 샘플 치트 명령
class SampleCheat : public MkCheatMessage
{
public:

	virtual bool ExcuteMsg(const MkStr& command, const MkArray<MkStr>& argument, MkStr& resultMsg) const
	{
		if (MkCheatMessage::ExcuteMsg(command, argument, resultMsg))
			return true;

		// 파일 로드
		if ((command == L"로드") && (argument.GetSize() == 1))
		{
			gDataNode.Clear();
			gTargetNode = NULL;

			bool ok = gDataNode.Load(MkPathName(argument[0]));

			resultMsg = argument[0];
			resultMsg += (ok) ? L" 파일 로딩 성공" : L" 파일 로딩 실패";

			if (ok)
			{
				resultMsg += L" 파일 로딩 성공";
				gTargetNode = &gDataNode;

				MK_DEV_PANEL.MsgToFreeboard(8, L">> Root");
			}
			else
			{
				resultMsg += L" 파일 로딩 실패";
			}

			return true;
		}

		// 해당 노드의 정보를 출력
		if ((gTargetNode != NULL) && (command == L"정보") && (argument.GetSize() == 0))
		{
			MkArray<MkHashStr> keyList;
			gTargetNode->GetUnitKeyList(keyList);
			resultMsg = MkStr(gTargetNode->GetUnitCount()) + L"개의 유닛 존재" + MkStr::CR;

			MK_INDEXING_LOOP(keyList, i)
			{
				resultMsg += L"  - " + keyList[i].GetString() + MkStr::CR;
			}

			keyList.Clear();
			gTargetNode->GetChildNodeList(keyList);
			resultMsg += MkStr(gTargetNode->GetChildNodeCount()) + L"개의 자식 노드 존재" + MkStr::CR;

			MK_INDEXING_LOOP(keyList, i)
			{
				resultMsg += L"  - " + keyList[i].GetString() + MkStr::CR;
			}
			return true;
		}

		// 해당 노드에 속한 해당 유닛 값 출력
		if ((gTargetNode != NULL) && (command == L"값") && (argument.GetSize() == 1))
		{
			ePrimitiveDataType type = gTargetNode->GetUnitType(argument[0]);
			switch (type)
			{
			case ePDT_Bool:			__SampleUnit<bool>::GetInfo(argument[0], resultMsg); break;
			case ePDT_Int:			__SampleUnit<int>::GetInfo(argument[0], resultMsg); break;
			case ePDT_UnsignedInt:	__SampleUnit<unsigned int>::GetInfo(argument[0], resultMsg); break;
			case ePDT_Float:		__SampleUnit<float>::GetInfo(argument[0], resultMsg); break;
			case ePDT_Int2:			__SampleUnit<MkInt2>::GetInfo(argument[0], resultMsg); break;
			case ePDT_Vec2:			__SampleUnit<MkVec2>::GetInfo(argument[0], resultMsg); break;
			case ePDT_Vec3:			__SampleUnit<MkVec3>::GetInfo(argument[0], resultMsg); break;
			case ePDT_Str:			__SampleUnit<MkStr>::GetInfo(argument[0], resultMsg); break;
			}
			return true;
		}

		// 노드 이동. L".."이면 부모 노드로, 아니면 자식 노드로 이동
		if ((gTargetNode != NULL) && (command == L"이동") && (argument.GetSize() == 1))
		{
			if (argument[0] == L"..")
			{
				MkDataNode* parentNode = gTargetNode->GetParentNode();
				if (parentNode == NULL)
				{
					resultMsg = L"이미 최상위 노드임";
				}
				else
				{
					gTargetNode = parentNode;
					resultMsg = gTargetNode->GetNodeName().GetString() + L" 부모 노드로 이동";
					MK_DEV_PANEL.MsgToFreeboard(8, L">> " + gTargetNode->GetNodeName().GetString());
				}
			}
			else
			{
				if (gTargetNode->ChildExist(argument[0]))
				{
					gTargetNode = gTargetNode->GetChildNode(argument[0]);
					resultMsg = gTargetNode->GetNodeName().GetString() + L" 노드로 이동";
					MK_DEV_PANEL.MsgToFreeboard(8, L">> " + gTargetNode->GetNodeName().GetString());
				}
				else
				{
					resultMsg = argument[0] + L" 노드는 존재하지 않음";
					return false;
				}
			}
			return true;
		}

		// 해당 노드를 별도의 파일로 저장
		if ((gTargetNode != NULL) && (command == L"저장") && (argument.GetSize() == 0))
		{
			MkStr name = gTargetNode->GetNodeName().GetString();

			if (name.Empty())
			{
				name = L"Root";
			}
			name += L".txt";

			gTargetNode->SaveToText(MkPathName(name));

			resultMsg = name + L" 파일에 텍스트 모드로 저장 완료";

			return true;
		}

		return false;
	}

	SampleCheat() : MkCheatMessage() {}
	virtual ~SampleCheat() {}
};

// TestPage 선언(공통)
class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		MK_DEV_PANEL.MsgToFreeboard(1, L"명령어");
		MK_DEV_PANEL.MsgToFreeboard(2, L"  - 로드 (file path)");
		MK_DEV_PANEL.MsgToFreeboard(3, L"  - 정보");
		MK_DEV_PANEL.MsgToFreeboard(4, L"  - 값 (unit key)");
		MK_DEV_PANEL.MsgToFreeboard(5, L"  - 이동 (node key or \"..\")");
		MK_DEV_PANEL.MsgToFreeboard(6, L"  - 저장");
		return true;
	}

	virtual void Update(const MkTimeState& timeState) {}
	virtual void Clear(MkDataNode* sharingNode = NULL) {}

	TestPage(const MkHashStr& name) : MkBasePage(name) {}
	virtual ~TestPage() {}	
};

// single-thread에서의 TestFramework 선언
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

// multi-thread에서의 TestThreadUnit 선언
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

// multi-thread에서의 TestFramework 선언
class TestFramework : public MkBaseFramework
{
public:
	virtual MkBaseThreadUnit* CreateLogicThreadUnit(const MkHashStr& threadName) const { return new TestThreadUnit(threadName); }
	virtual MkCheatMessage* CreateCheatMessage(void) const { return new SampleCheat(); }
	virtual ~TestFramework() {}
};

#endif

// TestApplication 선언(공통)
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
	application.Run(hI, L"MkDataNode sample", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 200, 100, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

