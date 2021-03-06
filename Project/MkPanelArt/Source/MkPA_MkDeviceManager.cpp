

#include "MkCore_MkCheck.h"
#include "MkCore_MkListeningWindow.h"
#include "MkCore_MkSystemEnvironment.h"
#include "MkCore_MkDevPanel.h"

#include "MkPA_MkResetableResourcePool.h"
#include "MkPA_MkDeviceManager.h"

#pragma comment(lib, "d3d9")
#pragma comment(lib, "d3dx9")


#define MKDEF_DISPLAY_MODE_MSG_TEXT L"디스플레이 해상도 : "

//------------------------------------------------------------------------------------------------//

bool MkDeviceManager::SetUp(MkListeningWindow* targetWindow, const MkInt2& size, bool fullScreen)
{
	MK_CHECK(targetWindow != NULL, L"NULL window로 MkDeviceManager 초기화 시도")
		return false;

	HWND hWnd = targetWindow->GetWindowHandle();
	MK_CHECK(hWnd != NULL, L"NULL window로 MkDeviceManager 초기화 시도")
		return false;

	m_TargetWindow = targetWindow;
	
	// interface
	m_Interface = Direct3DCreate9(D3D_SDK_VERSION);
	MK_CHECK(m_Interface != NULL, L"d3d9 인터페이스 초기화 실패. d3d9 end user runtime 인스톨 여부 확인")
		return false;

	// caps
	D3DCAPS9 caps;
	HRESULT hr = m_Interface->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	MK_CHECK(hr == D3D_OK, L"그래픽 장치 확인")
		return false;
	m_Caps = caps;

	// vertex processing type
	DWORD vpType = (((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0) && (caps.VertexShaderVersion >= D3DVS_VERSION(1,1))) ?
		D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// 가능한 display mode list
	unsigned int adapterModeCount = m_Interface->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
	MK_CHECK(adapterModeCount > 0, L"그래픽 장치에서 D3DFMT_X8R8G8B8 백버퍼를 지원하지 않음")
		return false;

	// 가능한 depth stencil format. 적어도 D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D16중 하나이어야 함
	D3DFORMAT depthStencilFormat = D3DFMT_UNKNOWN;
	do
	{
		depthStencilFormat = _CheckDepthStencilFormat(D3DFMT_D32);
		if (depthStencilFormat != D3DFMT_UNKNOWN)
			break;

		depthStencilFormat = _CheckDepthStencilFormat(D3DFMT_D24X8);
		if (depthStencilFormat != D3DFMT_UNKNOWN)
			break;

		depthStencilFormat = _CheckDepthStencilFormat(D3DFMT_D16);
		MK_CHECK(depthStencilFormat != D3DFMT_UNKNOWN, L"그래픽 장치에서 depth-stencil 포맷으로 D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D16을 지원하지 않음")
			return false;
	}
	while (false);

	// display mode 탐색
	for (unsigned int i=0; i<adapterModeCount; ++i)
	{
		D3DDISPLAYMODE dm;
		m_Interface->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &dm);
		MkInt2 availableSize(static_cast<int>(dm.Width), static_cast<int>(dm.Height));
		unsigned int index = _GetDisplayModeIndex(availableSize);
		if (!m_AvailableResolutionTable.Exist(index))
		{
			m_AvailableResolutionTable.Create(index, availableSize);
		}
	}

	// size 조정
	MkInt2 currSize;
	if (fullScreen)
	{
		currSize = ((size.x <= 0) || (size.y <= 0)) ? MK_SYS_ENV.GetBackgroundResolution() : size;
		MK_CHECK(CheckAvailableDisplayMode(currSize, fullScreen), MkStr(currSize) + L" 해상도는 풀모드에서 사용 할 수 없음")
			return false;
	}
	else
	{
		currSize = m_TargetWindow->GetClientSize();
	}
	
	// D3DPRESENT_PARAMETERS 세팅
	D3DPRESENT_PARAMETERS pp;
	pp.BackBufferWidth = static_cast<unsigned int>(size.x);
	pp.BackBufferHeight = static_cast<unsigned int>(size.y);
    pp.BackBufferFormat = D3DFMT_X8R8G8B8;
    pp.BackBufferCount = 0;

    pp.MultiSampleType = D3DMULTISAMPLE_NONE;
    pp.MultiSampleQuality = 0;

    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.hDeviceWindow = hWnd;
	pp.Windowed = (fullScreen) ? FALSE : TRUE;
    pp.EnableAutoDepthStencil = true;
    pp.AutoDepthStencilFormat = depthStencilFormat;
    pp.Flags = 0;

    pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// device 생성
	hr = m_Interface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vpType, &pp, &m_Device);
	MK_CHECK(SUCCEEDED(hr), L"D3D device 초기화 실패")
		return false;

	// D3DPRESENT_PARAMETERS 백업
	m_CurrentPresentParameters = pp;

	// log msg
	D3DADAPTER_IDENTIFIER9 adapterIdentifier;
	m_Interface->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapterIdentifier);
	MkStr adapterDesc;
	adapterDesc.ImportMultiByteString(adapterIdentifier.Description);

	MkStr dmMsg = GetCurrentDisplayModeString();
	MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_DISPLAYMODE, MKDEF_DISPLAY_MODE_MSG_TEXT + dmMsg);

	MkStr fullMsg;
	fullMsg.Reserve(256);
	fullMsg += L"   - Adapter : ";
	fullMsg += adapterDesc;
	fullMsg += MkStr::CRLF;
	fullMsg += L"   - Display mode : ";
	fullMsg += dmMsg;
	MK_DEV_PANEL.MsgToLog(fullMsg, false);

	// aspect
	m_AspectRatio = static_cast<float>(pp.BackBufferWidth) / static_cast<float>(pp.BackBufferHeight);

	return true;
}

void MkDeviceManager::Clear(void)
{
	MK_RELEASE(m_Device);
	MK_RELEASE(m_Interface);
	m_TargetWindow = NULL;
}

bool MkDeviceManager::ResetDevice(void)
{
	return (m_Device == NULL) ? false : _Reset();
}

bool MkDeviceManager::ResetDeviceByClientSize(void)
{
	bool ok = ((m_TargetWindow != NULL) && _Reset(m_TargetWindow->GetClientSize(), IsFullScreen()));
	if (ok)
	{
		MK_DEV_PANEL.__ResetAlignmentPosition();
	}
	return ok;
}

bool MkDeviceManager::ChangeDisplayMode(const MkInt2& size, bool fullScreen)
{
	if (m_TargetWindow == NULL)
		return false;

	// 이전 모드
	MkInt2 lastSize = GetCurrentResolution();
	bool lastFullScreen = IsFullScreen();

	// 현재 모드
	MkInt2 currSize;
	if (fullScreen)
	{
		currSize = ((size.x <= 0) || (size.y <= 0)) ? MK_SYS_ENV.GetBackgroundResolution() : size;
		MK_CHECK(CheckAvailableDisplayMode(currSize, fullScreen), MkStr(currSize) + L" 해상도는 풀모드에서 사용 할 수 없음")
			return false;
	}
	else
	{
		if ((size.x <= 0) || (size.y <= 0))
		{
			MkInt2 ws = MkBaseWindow::GetWorkspaceSize();
			if (ws.IsZero())
				return false;
			currSize = m_TargetWindow->ConvertWindowToClientSize(ws);
		}
		else
		{
			currSize = size;
		}
	}

	// 아무런 차이점이 없으면 무시
	if ((currSize == lastSize) && (fullScreen == lastFullScreen))
		return true;
	
	bool ok = _Reset(currSize, fullScreen);
	if (ok)
	{
		if ((!lastFullScreen) && fullScreen) // window -> fullscreen
		{
			m_TargetWindow->ChangeToFullModeStyle();
		}
		else if (lastFullScreen && (!fullScreen)) // fullscreen -> window
		{
			m_TargetWindow->ChangeToWindowModeStyle();
		}

		if (!fullScreen)
		{
			MkInt2 ws = m_TargetWindow->ConvertClientToWindowSize(currSize);
			ok = (SetWindowPos(m_TargetWindow->GetWindowHandle(), NULL, 0, 0, currSize.x, currSize.y, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE) != 0);
		}

		MK_DEV_PANEL.__ResetAlignmentPosition();
	}
	return ok;
}

bool MkDeviceManager::CheckAvailableDisplayMode(const MkInt2& size, bool fullScreen) const
{
	if (!fullScreen)
		return true;

	unsigned int index = _GetDisplayModeIndex(size);
	return m_AvailableResolutionTable.Exist(index);
}

MkInt2 MkDeviceManager::GetCurrentResolution(void) const
{
	return MkInt2(static_cast<int>(m_CurrentPresentParameters.BackBufferWidth), static_cast<int>(m_CurrentPresentParameters.BackBufferHeight));
}

bool MkDeviceManager::IsFullScreen(void) const
{
	return (m_CurrentPresentParameters.Windowed == FALSE);
}

void MkDeviceManager::GetAvailableResolutionList(MkArray<MkInt2>& buffer) const
{
	m_AvailableResolutionTable.GetFieldList(buffer);
}

MkDeviceManager::MkDeviceManager() : MkSingletonPattern<MkDeviceManager>()
{
	m_Interface = NULL;
	m_Device = NULL;
	m_TargetWindow = NULL;
}

//------------------------------------------------------------------------------------------------//

D3DFORMAT MkDeviceManager::_CheckDepthStencilFormat(D3DFORMAT targetFormat) const
{
	HRESULT hr = m_Interface->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8, targetFormat);
	return (SUCCEEDED(hr)) ? targetFormat : D3DFMT_UNKNOWN;
}

unsigned int MkDeviceManager::_GetDisplayModeIndex(const MkInt2& size) const
{
	return (static_cast<unsigned int>(size.x) << 16) + static_cast<unsigned int>(size.y);
}

MkStr MkDeviceManager::GetCurrentDisplayModeString(void) const
{
	MkStr buffer;
	buffer.Reserve(128);
	buffer += MkStr(m_CurrentPresentParameters.BackBufferWidth);
	buffer += L" * ";
	buffer += MkStr(m_CurrentPresentParameters.BackBufferHeight);
	buffer += L" ";
	buffer += (m_CurrentPresentParameters.Windowed == FALSE) ? L"fullscreen" : L"window";
	buffer.OptimizeMemory();
	return buffer;
}

bool MkDeviceManager::_Reset(void)
{
	MK_RESETABLE_RESPOOL.UnloadResources();

	bool ok = SUCCEEDED(m_Device->Reset(&m_CurrentPresentParameters));
	if (ok)
	{
		MK_RESETABLE_RESPOOL.ReloadResources(m_Device);

		MkStr dmMsg = GetCurrentDisplayModeString();
		MK_DEV_PANEL.MsgToLog(dmMsg + L" 모드", true);
		MK_DEV_PANEL.__MsgToSystemBoard(MKDEF_PREDEFINED_SYSTEM_INDEX_DISPLAYMODE, MKDEF_DISPLAY_MODE_MSG_TEXT + dmMsg);
	}
	return ok;
}

bool MkDeviceManager::_Reset(const MkInt2& size, bool fullScreen)
{
	m_CurrentPresentParameters.BackBufferWidth = static_cast<unsigned int>(size.x);
	m_CurrentPresentParameters.BackBufferHeight = static_cast<unsigned int>(size.y);
	m_CurrentPresentParameters.Windowed = (fullScreen) ? FALSE : TRUE;

	bool ok = _Reset();
	if (ok)
	{
		m_AspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
	}
	return ok;
}

//------------------------------------------------------------------------------------------------//
