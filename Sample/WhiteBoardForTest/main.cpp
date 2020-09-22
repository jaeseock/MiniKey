
//------------------------------------------------------------------------------------------------//

#define DEF_STEAM_LOGIN FALSE

#include <windows.h>

#include "minizip/unzip.h"

#include "MkCore_MkDevPanel.h"
#include "MkCore_MkDataNode.h"
//#include "MkCore_MkInputManager.h"
//#include "MkCore_MkFileManager.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"

//#include "MkCore_MkInterfaceForFileReading.h"
#include "MkCore_MkInterfaceForFileWriting.h"
#include "MkCore_MkInterfaceForZipFileReading.h"

#include "MkCore_MkFileManager.h"


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

class _SteamBillingCallback
{
public:
	inline void SetCashProductIndex(unsigned int productIndex) { m_CashProductIndex = productIndex; }
	inline void SetOrderID(unsigned __int64 orderID) { m_OrderID = orderID; }

	_SteamBillingCallback()
	{
		SetCashProductIndex(0);
		SetOrderID(0);
	}

private:
	STEAM_CALLBACK(_SteamBillingCallback, _MicroTxnAuthorizationResponse, MicroTxnAuthorizationResponse_t);

	unsigned int m_CashProductIndex;
	unsigned __int64 m_OrderID;
};

void _SteamBillingCallback::_MicroTxnAuthorizationResponse(MicroTxnAuthorizationResponse_t* callback)
{
	UINT appID = callback->m_unAppID;
	unsigned __int64 orderID = callback->m_ulOrderID;
	BYTE auth = callback->m_bAuthorized;

	/*
	MkStr msg;
	msg.Reserve(512);
	msg += L"m_CashProductIndex : ";
	msg += m_CashProductIndex;
	msg += MkStr::CRLF;
	msg += L"m_OrderID : ";
	msg += m_OrderID;

	msg += MkStr::CRLF;
	msg += MkStr::CRLF;
	msg += L"appID : ";
	msg += appID;
	msg += MkStr::CRLF;
	msg += L"orderID : ";
	msg += orderID;
	msg += MkStr::CRLF;
	msg += L"auth : ";
	msg += static_cast<int>(auth);
	
	::MessageBox(NULL, msg.GetPtr(), L"STEAM CALLBACK", MB_OK);
	*/

	if ((appID == 838330) && (orderID == m_OrderID))
	{
		bool isTransaction = (auth != 0);

		NETWORK::SendPacket<NETWORK::EloaPacketHeader> kSendPacket( PACKET::CZ_QRY_STEAM_FINALIZETXN );
		kSendPacket << m_OrderID;
		kSendPacket << m_CashProductIndex;
		kSendPacket << isTransaction;
		ClientNetwork::GetSingleton()->SendToSync( kSendPacket );
	}
}

_SteamBillingCallback gSBCallback;

// 서버에서 거래 패킷을 받으면 설정
//gSBCallback.SetOrderID(ui64Orderid);
//gSBCallback.SetCashProductIndex(productIndex);

#endif


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

	virtual void StartLooping(void)
	{
		//MkPathName filePath = L"D:\\인수인계\\02.정기업무\\";
		//MkDataNode node;
		//filePath.ExportSystemStructure(node, true);
		//node.SaveToText(L"test.txt");

		/*
		do
		{
			MkInterfaceForZipFileReading etcZip;
			if (!etcZip.SetUp(L"etc\\etc.SPAK"))
				break;

			MkArray<MkStr> tokens;
			{
				MkByteArray byteArray;
				etcZip.Read(L"etc\\crccheckfile.sst", byteArray);

				MkStr crcCheckFileBuffer;
				crcCheckFileBuffer.ReadTextStream(byteArray);
				crcCheckFileBuffer.Tokenize(tokens, L"\n");
			}
			if (tokens.Empty())
				break;

			unsigned int fileSize = tokens[0].ToUnsignedInteger();
			MkArray<MkPathName> crcFileList(fileSize);

			unsigned int index = 1;
			while (crcFileList.GetSize() < fileSize)
			{
				MkStr& currToken = tokens[index++];
				currToken.RemoveRearSideBlank();
				crcFileList.PushBack(currToken);
			}

			if (crcFileList.Empty())
				break;

			MkInterfaceForZipFileReading mapZip;
			if (!mapZip.SetUp(L"map\\map.SPAK"))
				break;

			MkArray<DWORD> crcList;
			crcList.Fill(crcFileList.GetSize());

			unsigned int accCrc = 0;

			MK_INDEXING_LOOP(crcFileList, i)
			{
				MkByteArray byteArray;
				const MkPathName& currFile = crcFileList[i];
				MkPathName fullPath;
				fullPath.ConvertToRootBasisAbsolutePath(currFile);
				if (fullPath.CheckAvailable())
				{
					MkInterfaceForFileReading frInterface;
					frInterface.SetUp(fullPath);
					frInterface.Read(byteArray, MkArraySection(0));
					frInterface.Clear();
				}
				else if (currFile.CheckPrefix(L"etc"))
				{
					etcZip.Read(currFile, byteArray);
				}
				else if (currFile.CheckPrefix(L"map"))
				{
					mapZip.Read(currFile, byteArray);
				}
				else
				{
					// error
				}

				if (!byteArray.Empty())
				{
					accCrc = crc32(accCrc, byteArray.GetPtr(), byteArray.GetSize());
				}
			}

			::MessageBox(m_MainWindow.GetWindowHandle(), MkStr(accCrc).GetPtr(), L"CRC", MB_OK);
			MK_DEV_PANEL.MsgToLog(L"CRC : " + MkStr(accCrc));
		}
		while (false);
		*/
	}

	virtual void Update(void)
	{
#if DEF_STEAM_LOGIN
		// steam dlc
		if (SteamApps()->BIsDlcInstalled(dlcID))
		{
		}

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

