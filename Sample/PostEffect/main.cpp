
#include <d3dx9.h>

#include <windows.h>
#include "MkCore_MkCheck.h"
//#include "MkCore_MkTimeManager.h"
#include "MkCore_MkWindowUpdateListener.h"
#include "MkCore_MkBaseFramework.h"
#include "MkCore_MkWin32Application.h"
//#include "MkCore_MkProfilingManager.h"

#include "MkPE_MkPostEffectRenderer.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "mkcore")


//------------------------------------------------------------------------------------------------//

class TempListener : public MkWindowUpdateListener
{
public:
	inline void SetHWND(HWND hWnd) { m_hWnd = hWnd; }

	virtual void ListenWindowSizeUpdate(const MkIntRect& newRect)
	{
		//MK_DEVICE_MGR.ResetDeviceByClientSize();
	}

	virtual void ListenShowCmdUpdate(unsigned int newShowCmd, const MkIntRect& newRect)
	{
		switch (newShowCmd)
		{
		case SW_SHOWNORMAL:
		case SW_SHOWMAXIMIZED:
			{
				RECT rect;
				::GetClientRect(m_hWnd, &rect);
				MK_POST_EFFECT.Resize(MkInt2(static_cast<int>(rect.right), static_cast<int>(rect.bottom)));
			}
			
			//MK_DEVICE_MGR.ResetDeviceByClientSize();
			break;

		case SW_SHOWMINIMIZED:
			//m_RendererState.Set(eMinimized);
			break;
		}
	}

	virtual void ListenWindowRestored(const MkIntRect& newRect)
	{
		//m_RendererState.Clear(eMinimized);
	}

	TempListener() { m_hWnd = NULL; }

protected:

	HWND m_hWnd;
};

class TestFramework : public MkBaseFramework
{
public:
	virtual bool SetUp(int clientWidth, int clientHeight, bool fullScreen, const MkCmdLine& cmdLine)
	{
		HWND hWnd = m_MainWindow.GetWindowHandle();
	
		// interface
		m_Interface = Direct3DCreate9(D3D_SDK_VERSION);
		MK_CHECK(m_Interface != NULL, L"d3d9 인터페이스 초기화 실패. d3d9 end user runtime 인스톨 여부 확인")
			return false;

		// caps
		D3DCAPS9 caps;
		HRESULT hr = m_Interface->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
		MK_CHECK(hr == D3D_OK, L"그래픽 장치 확인")
			return false;

		// vertex processing type
		DWORD vpType = (((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0) && (caps.VertexShaderVersion >= D3DVS_VERSION(1,1))) ?
			D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		// 가능한 display mode list
		unsigned int adapterModeCount = m_Interface->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
		MK_CHECK(adapterModeCount > 0, L"그래픽 장치에서 D3DFMT_X8R8G8B8 백버퍼를 지원하지 않음")
			return false;

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
			currSize = MkInt2(clientWidth, clientHeight);
			MK_CHECK(m_AvailableResolutionTable.Exist(_GetDisplayModeIndex(currSize)), MkStr(currSize) + L" 해상도는 풀모드에서 사용 할 수 없음")
				return false;
		}
		else
		{
			currSize = m_MainWindow.GetClientSize();
		}
		
		// D3DPRESENT_PARAMETERS 세팅
		D3DPRESENT_PARAMETERS pp;
		pp.BackBufferWidth = static_cast<unsigned int>(currSize.x);
		pp.BackBufferHeight = static_cast<unsigned int>(currSize.y);
		pp.BackBufferFormat = D3DFMT_X8R8G8B8;
		pp.BackBufferCount = 0;

		pp.MultiSampleType = D3DMULTISAMPLE_NONE;
		pp.MultiSampleQuality = 0;

		pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pp.hDeviceWindow = hWnd;
		pp.Windowed = (fullScreen) ? FALSE : TRUE;
		pp.EnableAutoDepthStencil = true;
		pp.AutoDepthStencilFormat = D3DFMT_D24X8;
		pp.Flags = 0;

		pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		// device 생성
		hr = m_Interface->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vpType, &pp, &m_Device);
		MK_CHECK(SUCCEEDED(hr), L"D3D device 초기화 실패")
			return false;
		
		// D3DPRESENT_PARAMETERS 백업
		m_CurrentPresentParameters = pp;

		// 오~ 예
		m_Renderable = true;

		// listener에 등록
		m_TempListener.SetHWND(m_MainWindow.GetWindowHandle());
		m_MainWindow.RegisterWindowUpdateListener(&m_TempListener);

		//
		MkArray<MkStr> fxos;
		fxos.PushBack(L"..\\Shader\\Grayscale.fxo");
		fxos.PushBack(L"..\\Shader\\GaussianBlur.fxo");
		fxos.PushBack(L"..\\Shader\\CircularZoomingBlur.fxo");
		fxos.PushBack(L"..\\Shader\\Box48BlurBlur.fxo");
		fxos.PushBack(L"..\\Shader\\HDR_Reduction.fxo");
		
		MK_POST_EFFECT.SetUp(m_Device, currSize, fxos, true);
		MK_POST_EFFECT.CreateSourceRenderTarget(MkRenderToTexture::eTF_RGB);
		return true;
	}

	virtual void StartLooping(void)
	{
		// source
		m_SourceDC.SetTexture(0, L"..\\base.png");

		// post effect
		MkScreen* screen0 = MK_POST_EFFECT.CreateScreen(0, MkRenderToTexture::eTF_RGB);
		{
			MkDrawCall* drawCall = screen0->CreateDrawCall(0);
			drawCall->SetTexture(0, 0);
			drawCall->SetTexture(1, L"..\\depth.png");
		}

		MkScreen* screen1 = MK_POST_EFFECT.CreateScreen(1, MkInt2(128, 96), MkRenderToTexture::eTF_RGB);
		{
			MkDrawCall* drawCall = screen1->CreateDrawCall(0);
			drawCall->SetTexture(0, screen0->GetTargetTexture(0));
			drawCall->SetShaderEffect(L"HDR_Reduction");
		}

		MkScreen* screen2 = MK_POST_EFFECT.CreateScreen(2);
		{
			{
				MkDrawCall* drawCall = screen2->CreateDrawCall(0);
				drawCall->SetTexture(0, screen1->GetTargetTexture(0));
			}
			{
				MkDrawCall* drawCall = screen2->CreateDrawCall(1);
				drawCall->SetTexture(0, L"..\\ui.png");
			}
		}
	}

	virtual void Update(void)
	{
		if (m_Device == NULL)
			return;

		if (m_DeviceLost)
		{
			HRESULT hr = m_Device->TestCooperativeLevel();
			if (hr == D3DERR_DEVICELOST)
			{
				return; // 아직 device lost 상태
			}
			else if (hr == D3DERR_DEVICENOTRESET)
			{
				MK_POST_EFFECT.UnloadResource();

				if (SUCCEEDED(m_Device->Reset(&m_CurrentPresentParameters)))
				{
					MK_POST_EFFECT.ReloadResource();
					m_DeviceLost = false;
				}
			}
		}
		else
		{
			HWND hWnd = m_MainWindow.GetWindowHandle();
			m_Renderable = ((IsWindowVisible(hWnd) == TRUE) && (IsIconic(hWnd) == FALSE));

			if (m_Renderable)
			{
				// source
				m_Device->BeginScene();
				MK_POST_EFFECT.GetSourceRenderTarget().__Push();

				m_SourceDC.__Draw(m_Device, MkFloat2(1024.f, 768.f));

				MK_POST_EFFECT.GetSourceRenderTarget().__Pop();
				m_Device->EndScene();

				// poset effect
				MK_POST_EFFECT.Draw();

				m_DeviceLost = (m_Device->Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST);
			}
		}
	}

	virtual void Clear(void)
	{
		m_SourceDC.Clear();
		MK_POST_EFFECT.Clear();

		MK_RELEASE(m_Device);
		MK_RELEASE(m_Interface);
	}

	TestFramework() : MkBaseFramework()
	{
		m_Interface = NULL;
		m_Device = NULL;

		m_Renderable = false;
		m_DeviceLost = false;
	}

	virtual ~TestFramework()
	{
		Clear();
	}

protected:

	unsigned int _GetDisplayModeIndex(const MkInt2& size) const { return (static_cast<unsigned int>(size.x) << 16) + static_cast<unsigned int>(size.y); }

protected:

	// device
	IDirect3D9* m_Interface;
	LPDIRECT3DDEVICE9 m_Device;
	MkMap<unsigned int, MkInt2> m_AvailableResolutionTable;
	D3DPRESENT_PARAMETERS m_CurrentPresentParameters;

	TempListener m_TempListener;

	// render
	bool m_Renderable;
	bool m_DeviceLost;

	MkDrawCall m_SourceDC;
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
	application.Run(hI, L"test MkPostEffect", L"", true, eSWP_All, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, false);

	return 0;
}

//------------------------------------------------------------------------------------------------//

