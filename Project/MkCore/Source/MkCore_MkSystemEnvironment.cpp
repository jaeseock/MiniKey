
#include "MkCore_MkPathName.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkSystemEnvironment.h"


//------------------------------------------------------------------------------------------------//

MkStr MkSystemEnvironment::GetCurrentSystemDate(void) const
{
	wchar_t dateBuf[9];
	_wstrdate_s(dateBuf, 9);
	MkStr tmpBuffer = dateBuf;

	// "월/일/연" -> "연.월.일"
	MkStr month, day, year;
	unsigned int pos = tmpBuffer.GetFirstBlock(0, L"/", month);
	pos = tmpBuffer.GetFirstBlock(pos, L"/", day);
	tmpBuffer.GetSubStr(MkArraySection(pos), year);
	
	MkStr result;
	result.Reserve(10);
	result += L"20";
	result += year;
	result += L".";
	result += month;
	result += L".";
	result += day;
	return result;
}

MkStr MkSystemEnvironment::GetCurrentSystemTime(void) const
{
	wchar_t timeBuf[9];
	_wstrtime_s(timeBuf, 9);
	MkStr result = timeBuf;
	return result;
}

void MkSystemEnvironment::__PrintSystemInformationToLog(void) const
{
	// 시스템 기본 정보 출력
	MkStr buffer;
	buffer.Reserve(2048);
	buffer += L"< System environment >";
	buffer += MkStr::CRLF;

	// application version
	buffer += L"   - Application version : ";
	buffer += m_ApplicationVersion.ToString();
	buffer += MkStr::CRLF;

	// minikey version
	buffer += L"   - MiniKey version : ";
	buffer += m_MiniKeyVersion.ToString();
	buffer += MkStr::CRLF;

	// build mode
	MkStr currMode;
	buffer += L"   - Build mode : ";
	switch (m_BuildMode)
	{
	case eDebug: buffer += L"Debug"; break;
	case eRelease: buffer += L"Release"; break;
	case eShipping: buffer += L"Shipping"; break;
	}
	buffer += MkStr::CRLF;

	// user
	buffer += L"   - User name : ";
	buffer += m_CurrentUserName;
	buffer += MkStr::CRLF;

	// OS
	buffer += L"   - OS : Windows ";
	buffer += m_WindowsVersion;
	switch (m_ProcessorArchitecture)
	{
	case eEtcPA: buffer += L" (EtcPA)"; break;
	case eX86: buffer += L" (x86)"; break;
	case eX64: buffer += L" (x64)"; break;
	}
	buffer += MkStr::CRLF;

	// core
	buffer += L"   - Cores : ";
	buffer += MkStr(m_NumberOfProcessors);
	buffer += MkStr::CRLF;

	// locale
	buffer += L"   - Code page : ";
	buffer += MkStr(MkStr::GetCodePage());
	buffer += MkStr::CRLF;

	// background resolution
	buffer += L"   - Background resolution : ";
	buffer += m_BackgroundResolution.x;
	buffer += L" * ";
	buffer += m_BackgroundResolution.y;
	buffer += MkStr::CRLF;

	// module directory
	buffer += L"   - Module directory :";
	buffer += MkStr::CRLF;
	buffer += L"     ";
	buffer += MkStr(MkPathName::GetModuleDirectory());
	buffer += MkStr::CRLF;

	// root directory
	buffer += L"   - Root directory :";
	buffer += MkStr::CRLF;
	buffer += L"     ";
	buffer += MkStr(MkPathName::GetRootDirectory());
	buffer += MkStr::CRLF;

	// print to log
	MK_DEV_PANEL.MsgToLog(buffer, false);
}

MkSystemEnvironment::MkSystemEnvironment() : MkSingletonPattern<MkSystemEnvironment>()
{
	wchar_t fullPath[MAX_PATH];
	GetModuleFileName(NULL, fullPath, MAX_PATH);
	m_ApplicationVersion.SetUp(fullPath);

	m_MiniKeyVersion.SetUp(MKDEF_CORE_MAJOR_VERSION, MKDEF_CORE_MINOR_VERSION, MKDEF_CORE_BUILD_VERSION, MKDEF_CORE_REVISION_VERSION);

	_Initialize();
}

//------------------------------------------------------------------------------------------------//

void MkSystemEnvironment::_Initialize(void)
{
	// build mode
#ifdef _DEBUG
	m_BuildMode = eDebug;
#else

#ifdef MK_SHIPPING
	m_BuildMode = eShipping;
#else
	m_BuildMode = eRelease;
#endif	// MK_SHIPPING

#endif	// _DEBUG

	// system 정보
	SYSTEM_INFO systemInfo;
	::GetNativeSystemInfo(&systemInfo);

	// cpu 연산방식
	switch (systemInfo.wProcessorArchitecture)
	{
	case PROCESSOR_ARCHITECTURE_INTEL:
		m_ProcessorArchitecture = eX86;
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
	case PROCESSOR_ARCHITECTURE_AMD64:
		m_ProcessorArchitecture = eX64;
		break;
	default:
		m_ProcessorArchitecture = eEtcPA;
		break;
	}

	// core 수
	m_NumberOfProcessors = static_cast<unsigned int>(systemInfo.dwNumberOfProcessors);

	// OS 버전
	OSVERSIONINFO osVer;
	osVer.dwOSVersionInfoSize = sizeof(osVer);
	::GetVersionEx(&osVer);
	unsigned int osMajor = static_cast<unsigned int>(osVer.dwMajorVersion);
	unsigned int osMinor = static_cast<unsigned int>(osVer.dwMinorVersion);
	m_WindowsVersion = MkStr(osMajor) + L"." + MkStr(osMinor);
	if (osVer.szCSDVersion[0] != NULL)
	{
		m_WindowsVersion += MkStr::SPACE;
		m_WindowsVersion += MkStr(osVer.szCSDVersion);
	}
	m_WindowsIsXpOrHigher = ((osMajor * 10 + osMinor) >= 51); // XP == 5.1
	m_WindowsIsVistaOrHigher = ((osMajor * 10) >= 60); // Vista == 6.0

	// 바탕화면 해상도
	RECT rect;
	::GetWindowRect(GetDesktopWindow(), &rect);
	m_BackgroundResolution.x = static_cast<int>(rect.right);
	m_BackgroundResolution.y = static_cast<int>(rect.bottom);

	// 유저 정보
	DWORD userNameSize = 256;
	wchar_t userName[256];
	::GetUserName(userName, &userNameSize);
	m_CurrentUserName = userName;
}

//------------------------------------------------------------------------------------------------//

