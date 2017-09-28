
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <WinInet.h>

#include "MkCore_MkStr.h"
#include "MkCore_MkStringOp.h"
#include "MkCore_MkVisitWebPage.h"

#pragma comment (lib, "wininet.lib")
#pragma comment (lib, "ws2_32.lib")

//------------------------------------------------------------------------------------------------//

bool MkVisitWebPage::Get(const MkStr& url, MkStr& buffer)
{
	bool ok = false;
	HINTERNET hSession = NULL;
	HINTERNET hObject = NULL;

	do
	{
		hSession = ::InternetOpen(L"MkVisitWebPage", INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);
		if (hSession == NULL)
			break;

		hObject = ::InternetOpenUrl(hSession, url.GetPtr(), NULL, 0, INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_NO_CACHE_WRITE, 0);
		if (hObject == NULL)
			break;

		std::string strBuffer;
        char charBuffer[1024];
        DWORD dwRead = 0;
        
        while ((::InternetReadFile(hObject, charBuffer, 1023, &dwRead) == TRUE) && (dwRead > 0))
        {
            charBuffer[dwRead] = NULL;
            strBuffer += charBuffer;
			::Sleep(1);
        }

		buffer.ImportMultiByteString(strBuffer);
		ok = true;
	}
	while (false);

	if (hObject != NULL)
	{
		::InternetCloseHandle(hObject);
	}
	if (hSession != NULL)
	{
		::InternetCloseHandle(hSession);
	}
	return ok;
}

bool MkVisitWebPage::Post(const MkStr& url, const MkStr& postData, MkStr& buffer)
{
	if (postData.Empty())
		return Get(url, buffer);

	wchar_t szScheme[INTERNET_MAX_URL_LENGTH + 1];
	wchar_t szHostName[INTERNET_MAX_URL_LENGTH + 1];
	wchar_t szUrlPath[INTERNET_MAX_URL_LENGTH + 1];
	wchar_t szUserName[1024];
	wchar_t szPassword[1024];

	URL_COMPONENTS uc;
	::ZeroMemory(&uc, sizeof(URL_COMPONENTS));

	uc.dwStructSize = sizeof(URL_COMPONENTS);
	uc.lpszScheme = szScheme;
	uc.dwSchemeLength = INTERNET_MAX_URL_LENGTH;
	uc.lpszHostName = szHostName;
	uc.dwHostNameLength = INTERNET_MAX_URL_LENGTH;
	uc.lpszUrlPath = szUrlPath;
	uc.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
	uc.lpszUserName = szUserName;
	uc.dwUserNameLength = sizeof(szUserName);
	uc.lpszPassword = szPassword;
	uc.dwPasswordLength = sizeof(szPassword);

	if (::InternetCrackUrl(url.GetPtr(), url.GetSize(), ICU_ESCAPE, &uc) == FALSE)
		return false;

	DWORD dwFlags = (uc.nScheme == INTERNET_SCHEME_HTTPS) ?
		(INTERNET_FLAG_SECURE |
		INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
		INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS |
		INTERNET_FLAG_IGNORE_CERT_DATE_INVALID |
		INTERNET_FLAG_IGNORE_CERT_CN_INVALID) : 0;
	
	bool ok = false;
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hObject = NULL;

	do
	{
		hSession = ::InternetOpen(L"MkVisitWebPage", INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);
		if (hSession == NULL)
			break;

		hConnect = ::InternetConnect(hSession, szHostName, uc.nPort, szUserName, szPassword, INTERNET_SERVICE_HTTP, dwFlags , 0);
		if (hConnect == NULL)
			break;

		hObject = ::HttpOpenRequest(hConnect, L"POST", szUrlPath, HTTP_VERSION, NULL, NULL,
			INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_NO_CACHE_WRITE | dwFlags, 0);
		if (hObject == NULL)
			break;

		std::string postStr;
		MkStringOp::ConvertString(postData.GetPtr(), postStr, CP_UTF8);
		
		MkStr postHeader;
		postHeader.Reserve(512);
		postHeader += L"Accept: text/*";
		postHeader += MkStr::CRLF;
		postHeader += L"User-Agent: Mozilla/4.0 (compatible)";
		postHeader += MkStr::CRLF;
		postHeader += L"Content-type: application/x-www-form-urlencoded";
		postHeader += MkStr::CRLF;
		postHeader += L"Content-length: ";
		postHeader += postStr.size();
		postHeader += MkStr::CRLF;

		if (::HttpAddRequestHeaders(hObject, postHeader.GetPtr(), -1L, HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD) == FALSE)
			break;

		if (::HttpSendRequest(hObject, NULL, 0, const_cast<char*>(postStr.c_str()), postStr.size()) == FALSE)
		{
			if (::GetLastError() == ERROR_INTERNET_INVALID_CA)
			{
				DWORD newFlags;
				DWORD buffLen = sizeof(DWORD);
				::InternetQueryOption(hObject, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&newFlags, &buffLen);

				newFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
				::InternetSetOption(hObject, INTERNET_OPTION_SECURITY_FLAGS, &newFlags, sizeof(DWORD));

				if (::HttpSendRequest(hObject, NULL, 0, const_cast<char*>(postStr.c_str()), postStr.size()) == FALSE)
					break;
			}
			else
				break;
		}
		
        std::string strBuffer;
        char charBuffer[1024];
        DWORD dwRead = 0;
        
        while ((::InternetReadFile(hObject, charBuffer, 1023, &dwRead) == TRUE) && (dwRead > 0))
        {
            charBuffer[dwRead] = NULL;
            strBuffer += charBuffer;
			::Sleep(1);
        }

		buffer.ImportMultiByteString(strBuffer);
		ok = true;
	}
	while (false);

	if (hObject != NULL)
	{
		::InternetCloseHandle(hObject);
	}
	if (hConnect != NULL)
	{
		::InternetCloseHandle(hConnect);
	}
	if (hSession != NULL)
	{
		::InternetCloseHandle(hSession);
	}
	return ok;
}

void MkVisitWebPage::CheckAddress(MkStr& address)
{
	do
	{
		MkArray<MkStr> tokens;
		if (address.Tokenize(tokens, L".") != 4)
			break;

		if (!tokens[0].IsDigit()) break;
		if (!tokens[1].IsDigit()) break;
		if (!tokens[2].IsDigit()) break;
		if (!tokens[3].IsDigit()) break;
		return; // ip
	}
	while (false);

	WSADATA wsaData;
	::WSAStartup(MAKEWORD(1, 1), &wsaData);

	ADDRINFO addrInfo;
	::ZeroMemory(&addrInfo, sizeof(ADDRINFO));
	addrInfo.ai_family = AF_UNSPEC;
	addrInfo.ai_socktype = SOCK_STREAM;
	addrInfo.ai_protocol = IPPROTO_TCP;

	std::string tmpAddr;
	address.ExportMultiByteString(tmpAddr);

	ADDRINFO* pInfo = NULL;
	if (::getaddrinfo(tmpAddr.c_str(), "0", &addrInfo, &pInfo) == 0)
	{
		SOCKADDR_IN* pAddr = reinterpret_cast<SOCKADDR_IN*>(pInfo->ai_addr);
		tmpAddr = ::inet_ntoa(pAddr->sin_addr);
		::freeaddrinfo(pInfo);

		address.ImportMultiByteString(tmpAddr);
	}

	::WSACleanup();
}

//------------------------------------------------------------------------------------------------//
