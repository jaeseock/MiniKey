
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
// multi-thread�� ��� �ּ� ó��
//------------------------------------------------------------------------------------------------//

//#define SAMPLEDEF_SINGLE_THREAD

//------------------------------------------------------------------------------------------------//

MkDataNode gDataNode;
MkDataNode* gTargetNode = NULL;


// �ڷ����� ���� �� �б�� ���ø� �������̽�
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

		resultMsg = MkPrimitiveDataType::GetTag(type) + L" Ÿ��. ";

		if (type != ePDT_Undefined)
		{
			if (dataSize == 1)
			{
				resultMsg += L"���� �ϳ��� ���� : ";
				GetSingleValue(key, resultMsg, 0);
			}
			else // dataSize > 1
			{
				resultMsg += MkStr(dataSize) + L" ũ���� �迭 :" + MkStr::CR;

				for (unsigned int i=0; i<dataSize; ++i)
				{
					GetSingleValue(key, resultMsg, i);
					resultMsg += MkStr::CR;
				}
			}
		}
	}
};


// ���� ġƮ ���
class SampleCheat : public MkCheatMessage
{
public:

	virtual bool ExcuteMsg(const MkStr& command, const MkArray<MkStr>& argument, MkStr& resultMsg) const
	{
		if (MkCheatMessage::ExcuteMsg(command, argument, resultMsg))
			return true;

		// ���� �ε�
		if ((command == L"�ε�") && (argument.GetSize() == 1))
		{
			gDataNode.Clear();
			gTargetNode = NULL;

			bool ok = gDataNode.Load(MkPathName(argument[0]));

			resultMsg = argument[0];
			resultMsg += (ok) ? L" ���� �ε� ����" : L" ���� �ε� ����";

			if (ok)
			{
				resultMsg += L" ���� �ε� ����";
				gTargetNode = &gDataNode;

				MK_DEV_PANEL.MsgToFreeboard(8, L">> Root");
			}
			else
			{
				resultMsg += L" ���� �ε� ����";
			}

			return true;
		}

		// �ش� ����� ������ ���
		if ((gTargetNode != NULL) && (command == L"����") && (argument.GetSize() == 0))
		{
			MkArray<MkHashStr> keyList;
			gTargetNode->GetUnitKeyList(keyList);
			resultMsg = MkStr(gTargetNode->GetUnitCount()) + L"���� ���� ����" + MkStr::CR;

			MK_INDEXING_LOOP(keyList, i)
			{
				resultMsg += L"  - " + keyList[i].GetString() + MkStr::CR;
			}

			keyList.Clear();
			gTargetNode->GetChildNodeList(keyList);
			resultMsg += MkStr(gTargetNode->GetChildNodeCount()) + L"���� �ڽ� ��� ����" + MkStr::CR;

			MK_INDEXING_LOOP(keyList, i)
			{
				resultMsg += L"  - " + keyList[i].GetString() + MkStr::CR;
			}
			return true;
		}

		// �ش� ��忡 ���� �ش� ���� �� ���
		if ((gTargetNode != NULL) && (command == L"��") && (argument.GetSize() == 1))
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

		// ��� �̵�. L".."�̸� �θ� ����, �ƴϸ� �ڽ� ���� �̵�
		if ((gTargetNode != NULL) && (command == L"�̵�") && (argument.GetSize() == 1))
		{
			if (argument[0] == L"..")
			{
				MkDataNode* parentNode = gTargetNode->GetParentNode();
				if (parentNode == NULL)
				{
					resultMsg = L"�̹� �ֻ��� �����";
				}
				else
				{
					gTargetNode = parentNode;
					resultMsg = gTargetNode->GetNodeName().GetString() + L" �θ� ���� �̵�";
					MK_DEV_PANEL.MsgToFreeboard(8, L">> " + gTargetNode->GetNodeName().GetString());
				}
			}
			else
			{
				if (gTargetNode->ChildExist(argument[0]))
				{
					gTargetNode = gTargetNode->GetChildNode(argument[0]);
					resultMsg = gTargetNode->GetNodeName().GetString() + L" ���� �̵�";
					MK_DEV_PANEL.MsgToFreeboard(8, L">> " + gTargetNode->GetNodeName().GetString());
				}
				else
				{
					resultMsg = argument[0] + L" ���� �������� ����";
					return false;
				}
			}
			return true;
		}

		// �ش� ��带 ������ ���Ϸ� ����
		if ((gTargetNode != NULL) && (command == L"����") && (argument.GetSize() == 0))
		{
			MkStr name = gTargetNode->GetNodeName().GetString();

			if (name.Empty())
			{
				name = L"Root";
			}
			name += L".txt";

			gTargetNode->SaveToText(MkPathName(name));

			resultMsg = name + L" ���Ͽ� �ؽ�Ʈ ���� ���� �Ϸ�";

			return true;
		}

		return false;
	}

	SampleCheat() : MkCheatMessage() {}
	virtual ~SampleCheat() {}
};

// TestPage ����(����)
class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		MK_DEV_PANEL.MsgToFreeboard(1, L"��ɾ�");
		MK_DEV_PANEL.MsgToFreeboard(2, L"  - �ε� (file path)");
		MK_DEV_PANEL.MsgToFreeboard(3, L"  - ����");
		MK_DEV_PANEL.MsgToFreeboard(4, L"  - �� (unit key)");
		MK_DEV_PANEL.MsgToFreeboard(5, L"  - �̵� (node key or \"..\")");
		MK_DEV_PANEL.MsgToFreeboard(6, L"  - ����");
		return true;
	}

	virtual void Update(const MkTimeState& timeState) {}
	virtual void Clear(MkDataNode* sharingNode = NULL) {}

	TestPage(const MkHashStr& name) : MkBasePage(name) {}
	virtual ~TestPage() {}	
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
	virtual MkCheatMessage* CreateCheatMessage(void) const { return new SampleCheat(); }
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
	application.Run(hI, L"MkDataNode sample", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 200, 100, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

