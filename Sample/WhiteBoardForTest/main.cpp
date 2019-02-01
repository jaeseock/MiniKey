
//------------------------------------------------------------------------------------------------//

#define DEF_STEAM_LOGIN FALSE

#include <windows.h>

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"
#include "MkCore_MkVisitWebPage.h"

#include "MkCore_MkFtpInterface.h"

// steam -----------------------------------------------------
#if DEF_STEAM_LOGIN

#pragma warning(push)
#pragma warning(disable:4819)
#pragma warning(disable:4995)
#pragma warning(disable:4996)

#include "steam_api.h"
#pragma comment (lib, "steam_api.lib")

#pragma warning(pop)

extern "C" void __cdecl SteamAPIDebugTextHook( int nSeverity, const char *pchDebugText )
{
	// if you're running in the debugger, only warnings (nSeverity >= 1) will be sent
	// if you add -debug_steamapi to the command-line, a lot of extra informational messages will also be sent
	MkStr msg;
	msg.ImportMultiByteString(pchDebugText);
	::OutputDebugString( msg.GetPtr() );

	if ( nSeverity >= 1 )
	{
		// place to set a breakpoint for catching API errors
		int x = 3;
		(void)x;
	}
}

#endif

// -----------------------------------------------------------

// single thread
class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
#if DEF_STEAM_LOGIN
		if ( SteamAPI_RestartAppIfNecessary( 838330 ) )
			return false; // EXIT_FAILURE

		//if ( !Steamworks_InitCEGLibrary() )
		//	return false;

		if ( !SteamAPI_Init() )
			return false;

		SteamClient()->SetWarningMessageHook( &SteamAPIDebugTextHook );

		if ( !SteamUser()->BLoggedOn() )
			return false;

		//Steamworks_SelfCheck();

		if ( !SteamController()->Init() )
			return false;
	    
		uint64 steamID = SteamUser()->GetSteamID().ConvertToUint64();
		MK_DEV_PANEL.MsgToLog(L"steamID : " + MkStr(steamID));

		char* ticketBuf = new char[1025];
		::ZeroMemory(ticketBuf, sizeof(char) * 1025);

		uint32 bufSize = 0;
		m_Ticket = SteamUser()->GetAuthSessionTicket(ticketBuf, 1024, &bufSize);
		MK_DEV_PANEL.MsgToLog(L"m_Ticket : " + MkStr(m_Ticket));
		MK_DEV_PANEL.MsgToLog(L"bufSize : " + MkStr(bufSize));

		MkStr ticketStr;
		ticketStr.Reserve(bufSize * 2);
		for (unsigned int i=0; i<bufSize; ++i)
		{
			unsigned int currChar = static_cast<unsigned int>(ticketBuf[i]);
			_AddHexadecimal(ticketStr, (currChar >> 4) & 0xf); // high part
			_AddHexadecimal(ticketStr, currChar & 0xf); // low part
		}

		MK_DEV_PANEL.MsgToLog(L"ticketStr : " + ticketStr);

		if (m_Ticket != k_HAuthTicketInvalid)
		{
			MkStr url;
			url.Reserve(1024);
			url += L"https://partner.steam-api.com/ISteamUserAuth/AuthenticateUserTicket/v1/?key=6D87332D80319E2392EEAF96D1AB1320&appid=838330&ticket=";
			url += ticketStr;

			MkStr rlt;
			if (MkVisitWebPage::Get(url, rlt))
			{
				rlt.WriteToTextFile(L"result.htm");
			}
			
		}
		delete [] ticketBuf;
#endif

		return true; // EXIT_SUCCESS
	}

	virtual void Update(void)
	{
#if DEF_STEAM_LOGIN
		SteamAPI_RunCallbacks();
#endif
	}

	virtual void Clear(void)
	{
#if DEF_STEAM_LOGIN
		if (m_Ticket != 0)
		{
			SteamUser()->CancelAuthTicket(m_Ticket);
		}

		SteamAPI_Shutdown();
		//Steamworks_TermCEGLibrary();
#endif
	}

	TestFramework() : MkBaseFramework()
	{
#if DEF_STEAM_LOGIN
		m_Ticket = 0;
#endif
	}

	virtual ~TestFramework()
	{
	}

protected:

#if DEF_STEAM_LOGIN
	void _AddHexadecimal(MkStr& buffer, unsigned int value)
	{
		switch (value)
		{
		case 0x0: buffer += L"0"; break;
		case 0x1: buffer += L"1"; break;
		case 0x2: buffer += L"2"; break;
		case 0x3: buffer += L"3"; break;
		case 0x4: buffer += L"4"; break;
		case 0x5: buffer += L"5"; break;
		case 0x6: buffer += L"6"; break;
		case 0x7: buffer += L"7"; break;
		case 0x8: buffer += L"8"; break;
		case 0x9: buffer += L"9"; break;
		case 0xa: buffer += L"A"; break;
		case 0xb: buffer += L"B"; break;
		case 0xc: buffer += L"C"; break;
		case 0xd: buffer += L"D"; break;
		case 0xe: buffer += L"E"; break;
		case 0xf: buffer += L"F"; break;
		}
	}
#endif

protected:

#if DEF_STEAM_LOGIN
	HAuthTicket m_Ticket;
#endif
};


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
	application.Run(hI, L"WhiteBoard", L"..\\ResRoot", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 200, 200);

	return 0;
}

//------------------------------------------------------------------------------------------------//

