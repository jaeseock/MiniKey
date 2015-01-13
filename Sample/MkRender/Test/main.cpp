
//------------------------------------------------------------------------------------------------//
// single/multi-thread rendering application sample
//------------------------------------------------------------------------------------------------//

#include <windows.h>

#include "MkCore_MkPageManager.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkTimeState.h"
#include "MkCore_MkWin32Application.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkSlangFilter.h"

#include "MkCore_MkDataNode.h"

#include "MkPA_MkRenderFramework.h"

#include "MkCore_MkUniformDice.h"

//#include "MkS2D_MkTexturePool.h"
#include "MkPA_MkFontManager.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkRenderer.h"

#include "MkPA_MkTextNode.h"

//#include "MkS2D_MkSceneNode.h"
//#include "MkS2D_MkBaseWindowNode.h"
//#include "MkS2D_MkSpreadButtonNode.h"
//#include "MkS2D_MkCheckButtonNode.h"
//#include "MkS2D_MkScrollBarNode.h"
//#include "MkS2D_MkEditBoxNode.h"
//#include "MkS2D_MkTabWindowNode.h"

//#include "MkS2D_MkDrawStep.h"
//#include "MkS2D_MkWindowEventManager.h"

//#include "MkCore_MkNameSelection.h"

#include "MkCore_MkDevPanel.h"

//------------------------------------------------------------------------------------------------//

// TestPage ¼±¾ð
class TestPage : public MkBasePage
{
public:
	virtual bool SetUp(MkDataNode& sharingNode)
	{
		MkTextNode tn;
		tn.SetUp(L"DecoString.txt");

		MkTextNode tnCopy = tn;

		MkDataNode dn;
		tnCopy.Export(dn);
		dn.SaveToText(L"DecoString_.txt");
		//MkStr txtStr;
		//txtStr.ReadTextFile(L"DecoString.txt");
		//MkDecoStr decoStr(txtStr);

		//MkInt2 size01F = MK_FONT_MGR.GetTextSize(MK_FONT_MGR.DSF(), test01, false);
		return true;
	}

	virtual void Update(const MkTimeState& timeState)
	{
		if (MK_INPUT_MGR.GetKeyReleased(VK_F1))
		{
			MkDataNode dataNode;
			if (dataNode.Load(L"MkRenderConfig.txt"))
			{
				MK_FONT_MGR.ChangeFontType(dataNode.GetChildNode(L"FontResource"), L"¸¼Àº°íµñ");
			}
		}
		else if (MK_INPUT_MGR.GetKeyReleased(VK_F2))
		{
			MkDataNode dataNode;
			if (dataNode.Load(L"MkRenderConfig.txt"))
			{
				MK_FONT_MGR.ChangeFontType(dataNode.GetChildNode(L"FontResource"), L"±¼¸²");
			}
		}
		else if (MK_INPUT_MGR.GetKeyReleased(VK_F3))
		{
			MkDataNode dataNode;
			if (dataNode.Load(L"MkRenderConfig.txt"))
			{
				MK_FONT_MGR.ChangeFontType(dataNode.GetChildNode(L"FontResource"), L"³ª´®°íµñ");
			}
		}
		
	}

	virtual void Clear(MkDataNode* sharingNode = NULL)
	{
		
	}

	TestPage(const MkHashStr& name) : MkBasePage(name)
	{
		
	}

	virtual ~TestPage() { Clear(); }

protected:
};

class TestFramework : public MkRenderFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const char* arg)
	{
		MK_PAGE_MGR.SetUp(new MkBasePage(L"Root"));
		MK_PAGE_MGR.RegisterChildPage(L"Root", new TestPage(L"TestPage"));
		
		MK_PAGE_MGR.ChangePageDirectly(L"TestPage");
		
		return MkRenderFramework::SetUp(clientWidth, clientHeight, fullScreen, arg);
	}

	virtual ~TestFramework() {}
};

// TestApplication ¼±¾ð
class TestApplication : public MkWin32Application
{
public:

	virtual MkBaseFramework* CreateFramework(void) const { return new TestFramework; }

public:
	TestApplication() : MkWin32Application() {}
	virtual ~TestApplication() {}
};

//------------------------------------------------------------------------------------------------//

// ¿£Æ®¸® Æ÷ÀÎÆ®¿¡¼­ÀÇ TestApplication ½ÇÇà
int WINAPI WinMain(HINSTANCE hI, HINSTANCE hPI, LPSTR cmdline, int iWinMode)
{
	TestApplication application;
	application.Run(hI, L"¶¼½º¶Ç", L"..\\FileRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, false, false, NULL, cmdline);

	return 0;
}

//------------------------------------------------------------------------------------------------//

