
#include "MkCore_MkCheck.h"
#include "MkCore_MkColor.h"
#include "MkCore_MkProfilingManager.h"
#include "MkCore_MkDevPanel.h"
#include "MkCore_MkInputManager.h"
#include "MkCore_MkListeningWindow.h"
#include "MkCore_MkDataNode.h"

#include "MkPA_MkProjectDefinition.h"
#include "MkPA_MkDeviceManager.h"
#include "MkPA_MkFontManager.h"
#include "MkPA_MkRenderStateSetter.h"
#include "MkPA_MkBitmapPool.h"
#include "MkPA_MkCursorManager.h"
#include "MkPA_MkDrawingMonitor.h"
#include "MkPA_MkStaticResourceContainer.h"
#include "MkPA_MkShaderEffectPool.h"
#include "MkPA_MkHiddenEditBox.h"
#include "MkPA_MkSceneObject.h"

#include "MkPA_MkRenderer.h"


#define MKDEF_EMPTY_FRAME_COLOR D3DCOLOR_XRGB(0, 0, 0)


//------------------------------------------------------------------------------------------------//

bool MkRenderer::SetUp(MkListeningWindow* targetWindow, int clientWidth, int clientHeight, bool fullScreen)
{
	MK_CHECK((targetWindow != NULL) && (targetWindow->GetWindowHandle() != NULL), L"NULL window로 MkRenderer 초기화 시도")
		return false;

	// listener에 등록
	targetWindow->RegisterWindowUpdateListener(this);

	// log msg
	MkStr msg;
	msg.Reserve(1024);
	msg += L"< Renderer >";
	msg += MkStr::CRLF;
	msg += L"   - MkRender version : ";
	msg += m_RendererVersion.ToString();
	MK_DEV_PANEL.MsgToLog(msg, false);
	
	// 장치 초기화
	if (!MK_DEVICE_MGR.SetUp(targetWindow, MkInt2(clientWidth, clientHeight), fullScreen))
		return false;

	MK_DEV_PANEL.InsertEmptyLine();

	// hidden edit box
	MK_EDIT_BOX.SetUp(targetWindow->GetWindowHandle(), ::GetModuleHandle(NULL));

	// initialize scene object
	MkSceneObject::InitSceneObject();

	// config file 로딩
	// 시스템간 의존성을 고려해 순서대로 시스템 초기화
	MkDataNode renderConfigNode;
	bool configExist = renderConfigNode.Load(MKDEF_PA_CONFIG_FILE_NAME);

	// 0. font resource
	MK_FONT_MGR.SetUp(renderConfigNode.GetChildNode(L"FontResource"));

	// 1. static resource
	const MkDataNode* staticResourceNode = renderConfigNode.GetChildNode(L"StaticResource");

	// 1.0. text
	MK_STATIC_RES.LoadTextNode(staticResourceNode);

	// 1.1. theme
	if (!MK_STATIC_RES.LoadWindowThemeSet(staticResourceNode->GetChildNode(L"Theme")))
		return false;
	
	// 2. cursor list
	MK_CURSOR_MGR.SetUp(renderConfigNode.GetChildNode(L"CursorType"));
	MK_CURSOR_MGR.SetCursorType(MkCursorManager::eNormal);

	// 3. shader effect pool
	const MkDataNode* shaderNode = renderConfigNode.GetChildNode(L"Shader");
	if (shaderNode != NULL)
	{
		MkArray<MkStr> fxoPathList;
		if (shaderNode->GetData(L"FilePath", fxoPathList))
		{
			MK_SHADER_POOL.SetUp(fxoPathList);
		}
	}

	return true;
}

void MkRenderer::Update(void)
{
	MkFocusProfiler focusProfiler(L"MkRenderer::Update");

	LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
	if (device == NULL)
		return;

	if (m_RendererState.Check(eDeviceLost))
	{
		HRESULT hr = device->TestCooperativeLevel();
		if (hr == D3DERR_DEVICELOST)
		{
			return; // 아직 device lost 상태
		}
		else if (hr == D3DERR_DEVICENOTRESET)
		{
			if (MK_DEVICE_MGR.ResetDevice())
			{
				m_RendererState.Clear(eDeviceLost);
			}
		}
	}
	else
	{
		// renderer 이벤트 처리
		{
			MkFocusProfiler focusProfiler(L"MkRenderer::Update(0) event op.");

			MkArray<_RendererEvent> eventQueue;
			if (m_EventQueue.Consume(eventQueue)) // event 처리
			{
				MK_INDEXING_LOOP(eventQueue, i)
				{
					const _RendererEvent& evt = eventQueue[i];
					switch (evt.eventType)
					{
					case eChangeDisplayMode:
						{
							bool fullScreen = (evt.arg1 == TRUE);
							if (fullScreen)
							{
								m_WebBrowser.Close();
							}

							MK_DEVICE_MGR.ChangeDisplayMode(evt.arg0, fullScreen);
							MK_INPUT_MGR.UpdateTargetWindowClientSize(NULL);
						}
						break;
					case eSetDefaultBackbufferColor:
						m_DefaultBackbufferColor = evt.arg1;
						break;
					}
				}
				eventQueue.Clear();
			}
		}

		// 최소화상태가 아니면 렌더링 진행
		if (!m_RendererState.Check(eMinimized))
		{
			MK_DRAWING_MONITOR.Reset();

			{
				MkFocusProfiler focusProfiler(L"MkRenderer::Update(1) update");

				MK_STATIC_RES.Update();
			}

			{
				MkFocusProfiler focusProfiler(L"MkRenderer::Update(2) draw");

				MK_RENDER_STATE.Begin();

				if (!m_DrawQueue.Draw())
				{
					device->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), MKDEF_EMPTY_FRAME_COLOR, 1.f, 0);
				}
			}
			
			MK_DRAWING_MONITOR.EndOfFrame();

			{
				MkFocusProfiler focusProfiler(L"MkRenderer::Update(3) present");

				// 하위 dialog 없음
				if (m_WebBrowser.GetBrowserHandle() == NULL)
				{
					if (device->Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST)
					{
						m_RendererState.Set(eDeviceLost);
					}
				}
				// 하위 dialog가 있다면 그 부분을 회피해 그림
				else
				{
					MkInt2 currResolution = MK_DEVICE_MGR.GetCurrentResolution();
					RECT topRect = { 0, 0, currResolution.x, m_DialogRect.top };   
					RECT bottomRect = { 0, m_DialogRect.bottom, currResolution.x, currResolution.y };
					RECT leftRect = { 0, m_DialogRect.top, m_DialogRect.left, m_DialogRect.bottom };
					RECT rightRect = { m_DialogRect.right, m_DialogRect.top, currResolution.x, m_DialogRect.bottom };
 
					bool deviceLost = true;
					do
					{
						if (device->Present(&topRect, &topRect,  NULL, NULL) == D3DERR_DEVICELOST)
							break;
						if (device->Present(&bottomRect, &bottomRect, NULL, NULL) == D3DERR_DEVICELOST)
							break;
						if (device->Present(&leftRect, &leftRect, NULL, NULL) == D3DERR_DEVICELOST)
							break;
						if (device->Present(&rightRect, &rightRect, NULL, NULL) == D3DERR_DEVICELOST)
							break;

						//if (device->Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST)
						//	break;

						deviceLost = false;
					}
					while (false);

					if (deviceLost)
					{
						m_RendererState.Set(eDeviceLost);
					}
				}
			}
		}
	}
}

void MkRenderer::Clear(void)
{
	m_WebBrowser.Close();

	m_EventQueue.Clear();
	m_DrawQueue.Clear();

	MK_SHADER_POOL.Clear();
	MK_STATIC_RES.Clear();
	MK_CURSOR_MGR.Clear();
	MK_BITMAP_POOL.Clear();
	MK_FONT_MGR.Clear();
	MK_DEVICE_MGR.Clear();
}

bool MkRenderer::OpenWebDialog(const MkStr& url, const MkIntRect& rect)
{
	if ((!MK_DEVICE_MGR.IsFullScreen()) && (!url.Empty()) && rect.SizeIsNotZero() && (m_WebBrowser.GetBrowserHandle() == NULL))
	{
		LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
		if ((device != NULL) && (device->SetDialogBoxMode(TRUE) == D3D_OK))
		{
			HWND hMyWnd = MK_DEVICE_MGR.GetTargetWindow()->GetWindowHandle();

			if (m_WebBrowser.Open(hMyWnd, url, rect.position.x, rect.position.y, rect.size.x, rect.size.y, false))
			{
				HWND hWebWnd = m_WebBrowser.GetBrowserHandle();

				RECT dr;
				::GetWindowRect(hWebWnd, &dr);
				
				POINT pt = { dr.left, dr.top };
				::ScreenToClient(hMyWnd, &pt);

				m_DialogRect.left = pt.x;
				m_DialogRect.right = dr.right - dr.left + pt.x;
				m_DialogRect.top = pt.y;
				m_DialogRect.bottom = dr.bottom - dr.top + pt.y;

				return true;
			}
			
			device->SetDialogBoxMode(FALSE);
		}
	}
	return false;
}

void MkRenderer::CloseWebDialog(void)
{
	if (m_WebBrowser.GetBrowserHandle() != NULL)
	{
		LPDIRECT3DDEVICE9 device = MK_DEVICE_MGR.GetDevice();
		if ((device != NULL) && (device->SetDialogBoxMode(FALSE) == D3D_OK))
		{
			m_WebBrowser.Close();
			m_DialogRect.left = m_DialogRect.right = m_DialogRect.top = m_DialogRect.bottom = 0;
		}
	}
}

//------------------------------------------------------------------------------------------------//

void MkRenderer::ChangeDisplayMode(const MkInt2& size, bool fullScreen)
{
	m_EventQueue.RegisterEvent(_RendererEvent(eChangeDisplayMode, size, (fullScreen) ? TRUE : FALSE));
}

void MkRenderer::SetDefaultBackbufferColor(const MkColor& color)
{
	m_EventQueue.RegisterEvent(_RendererEvent(eSetDefaultBackbufferColor, MkInt2::Zero, color.ConvertToD3DCOLOR()));
}

//------------------------------------------------------------------------------------------------//

void MkRenderer::ListenWindowSizeUpdate(const MkIntRect& newRect)
{
	MK_DEVICE_MGR.ResetDeviceByClientSize();
}

void MkRenderer::ListenShowCmdUpdate(unsigned int newShowCmd, const MkIntRect& newRect)
{
	switch (newShowCmd)
	{
	case SW_SHOWNORMAL:
	case SW_SHOWMAXIMIZED:
		MK_DEVICE_MGR.ResetDeviceByClientSize();
		break;

	case SW_SHOWMINIMIZED:
		m_RendererState.Set(eMinimized);
		break;
	}
}

void MkRenderer::ListenWindowRestored(const MkIntRect& newRect)
{
	m_RendererState.Clear(eMinimized);
}

MkRenderer::MkRenderer() : MkSingletonPattern<MkRenderer>()
{
	m_RendererVersion.SetUp(MKDEF_PA_MAJOR_VERSION, MKDEF_PA_MINOR_VERSION, MKDEF_PA_BUILD_VERSION, MKDEF_PA_REVISION_VERSION);
	m_RendererState.Clear();
	m_DefaultBackbufferColor = D3DCOLOR_XRGB(0, 0, 0);
	m_DialogRect.left = m_DialogRect.right = m_DialogRect.top = m_DialogRect.bottom = 0;
}

//------------------------------------------------------------------------------------------------//
